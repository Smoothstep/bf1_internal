Texture2D<float> DepthScene : register(t0);
Texture2D<float> Normals : register(t1);
Texture2D<float> DepthFriendly : register(t2);
Texture2D<float> DepthEnemy : register(t3);
Texture2D<float> DepthNeutral : register(t4);

SamplerState SampleDepth : register(s0);

cbuffer GlobalConstants
{
    float4x4 View;
    float4x4 ViewTransposed;
    float4x4 ViewInverse;
    float4x4 WorldViewProjection;
    float4x4 WorldViewProjectionInverse;
    float4 ScreenSize;
    float4 CameraOrigin;
    float4 CameraDirection;
    float4 ColorAlly;
    float4 ColorAllyOccluded;
    float4 ColorEnemy;
    float4 ColorEnemyOccluded;
    float4 ColorNeutral;
    float4 ColorNeutralOccluded;
    float FadeDistance;
    float FadeDistanceNameTags;
    float FadeDistanceIndicators;
    float FadeDistanceEngine;
};

struct PS_INPUT
{
    float4 vPos : SV_POSITION;
    float2 vTex : TEXCOORD0;
};

struct PS_OUTPUT
{
    float4 Normals : SV_Target0;
    float4 Albedo : SV_Target1;
    float4 Spec : SV_Target2;
    float3 Shadows : SV_Target3;
    float Depth : SV_Depth;
};

float SampleNearby(Texture2D<float> Tex, float2 Uv)
{
    //
    // float Q = 0.2 * pow(1 - D, 1) * exp(D);
    float PxdH = 0.005;
    float PxdV = 0.005;
    float Q = 0.1;
    PxdH *= Q;
    PxdV *= Q;

    float Dr0 = Tex.Sample(SampleDepth, Uv + float2(PxdH, 0));
    float Dr1 = Tex.Sample(SampleDepth, Uv + float2(PxdH, PxdV));
    float Dr2 = Tex.Sample(SampleDepth, Uv + float2(0, PxdV));
    float Dr3 = Tex.Sample(SampleDepth, Uv + float2(-PxdH, 0));
    float Dr4 = Tex.Sample(SampleDepth, Uv + float2(-PxdH, -PxdV));
    float Dr5 = Tex.Sample(SampleDepth, Uv + float2(0, -PxdV));
    float Dr6 = Tex.Sample(SampleDepth, Uv + float2(-PxdH, PxdV));
    float Dr7 = Tex.Sample(SampleDepth, Uv + float2(PxdH, -PxdV));

    return step(Dr0 * Dr1 * Dr2 * Dr3 * Dr4 * Dr5 * Dr6 * Dr7, 0);
}

PS_OUTPUT main(PS_INPUT In)
{
    float Ds = DepthScene.Sample(SampleDepth, In.vTex);
    float DrFriendly = DepthFriendly.Sample(SampleDepth, In.vTex);
    float DrEnemy = DepthEnemy.Sample(SampleDepth, In.vTex);

    float4 Albedo = lerp(
        lerp(ColorEnemyOccluded, ColorEnemy, step(Ds, DrEnemy)),
        lerp(ColorAllyOccluded, ColorAlly, step(Ds, DrFriendly)),
        step(DrEnemy, DrFriendly));

    //Albedo = (1.0 - step(DrEnemy, 0.00001)) * lerp(ColorEnemyOccluded, ColorEnemy, step(DrEnemy, Ds));
    //Albedo.a = 0;

    float Dr = max(DrEnemy, DrFriendly);
    float D = max(0.000001, Dr);
    float Dc = Ds >= Dr ? 1.0 : 0.0;
    float Outline = SampleNearby(DepthFriendly, In.vTex);

    PS_OUTPUT Out;

    float Drc = step(0.00001, Dr);
    float4 Result = Albedo;
    float4 N = Normals.Sample(SampleDepth, In.vTex);
    Out.Normals = N * 1.1;
	Out.Normals = CameraDirection;
    Out.Albedo = Result;
	Out.Albedo.a = 1.0;
    Out.Shadows = float3(0.1, 0.2, 0.7) * 100.0 * Result.a * Drc;
    Out.Spec = float4(0.5f, 0.5f, 0.8f, 1.0f);
    Out.Depth = Drc;
    return Out;
}