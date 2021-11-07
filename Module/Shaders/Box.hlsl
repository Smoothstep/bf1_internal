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

struct PInput
{
    float4 Pos : SV_POSITION;
    float2 Uv : TEXCOORD0;
    float2 Wv : TEXCOORD1;
    float4 Col : COLOR;
};

struct VInput
{
    float4 Pos : POSITION;
    float2 Uv : TEXCOORD0;
    float2 Wv : TEXCOORD1;
    float4 Col : COLOR;
    float4 Origin : TEXCOORD2;
};

float3x3 RotationMat(float3 Direction)
{
    Direction = normalize(Direction);
    float3 Dx = normalize(cross(float3(0, 1, 0), Direction));
    float3 Dy = normalize(cross(Direction, Dx));
    /*
    return float3x3(
        float3(Dx.x, Dx.y, Dx.z),
        float3(Dy.x, Dy.y, Dy.z),
        float3(Direction.x, Direction.y, Direction.z)
        );
        */
    return float3x3(
        float3(Dx.x, Dy.x, Direction.x),
        float3(Dx.y, Dy.y, Direction.y),
        float3(Dx.z, Dy.z, Direction.z)
        );
}

PInput VMain(VInput In)
{
    PInput Out;
    Out.Uv = In.Uv;
    Out.Wv = In.Wv;
    
    float3 Pos = float3(In.Pos.x, In.Pos.y, In.Origin.z);
    float3 ViewPos = View[3].xyz;

    float3 LocalDelta = Pos.xyz - In.Origin.xyz;
    float3 LocalDir = normalize(LocalDelta);

    float3 Forward = float3(1, 0, 0);
    float3 Delta = normalize(ViewPos - Pos);
    
    float3 ViewDir = normalize(View[2].xyz);
    float3 DeltaView = normalize(cross(ViewDir,Delta));
    float3 Fd = normalize(cross(Forward,Delta));
    //Fd *= 1;
    float3x3 Rot = RotationMat(ViewDir);
    //Rot = look_at_matrix(ViewPos, Pos, ViewInverse[1].xyz);
    //Rot = float3x3(float3(1, 0, 0), float3(0, 1, 0), float3(0, 0, 1));
    Out.Pos.xyz = In.Origin.xyz;
    Out.Pos.w = 1.0;
    //Out.Pos.xyz += 10 * mul(LocalDelta, Rot);
    //Out.Pos.xyz += mul(LocalDelta, (Rot));
	//Out.Pos.xyz =
    Out.Pos = mul(Out.Pos, WorldViewProjection);
    //Out.Pos.xyz += mul(mul(Pos.xyz - In.Origin.xyz, Rot), WorldViewProjection);
    //Out.Pos.xyz += (Pos - In.Origin.xyz) * cross(ViewDir, Delta) * 10.0;
    //Out.Pos.xyz += ((cross(Delta, LocalDir), cross(Delta, ViewDir))) * 2.0;
    //Out.Pos.xyz += normalize(cross(cross(LocalDir, Delta), cross(ViewDir, Delta)));
    //Out.Pos.xyz += mul(Pos - In.Origin.xyz, Rot) * 1;

    //Out.Pos.xyz = Pos;
    //Out.Pos.xyz = In.Origin.xyz + mul(float4(LocalDelta.x, LocalDelta.y, LocalDelta.z, 1), (look_at_matrix(ViewPos, Pos, -ViewInverse[1].xyz))).xyz;
    Out.Col = In.Col;
    return Out;
}

float4 PMain(PInput In) : SV_Target
{
    float Ratio = 2.0f;
    float4 fragColor;
    float2 uv = In.Uv;

    float dX = abs(uv.x - 0.5);
    float dY = abs(uv.y - 0.5);

    if (dX < 0.2)
    {
        fragColor = 0.0;
        //return fragColor;
    }

    uv.x = clamp(uv.x, 0.0, 0.99);
    uv.y = clamp(uv.y, 0.0, 0.99);
//float DistX = 1.0 * smoothstep(0.0, 0.01, pow(abs(uv.x - 0.9) * abs(uv.x - 0.01), 1.0f));
    float DistX = (1.5 / Ratio) * pow(abs(uv.x - 1.0) * abs(uv.x), 0.1f);
//float DistY = 1.0 * smoothstep(0.0, 0.1, abs(uv.y - 0.985) * abs(uv.y - 0.01));
    float DistY = 0.9 * pow(abs(uv.y - 1.0) * abs(uv.y), 0.1f);
    DistX /= clamp(dX - 0.5, 0.7, 1.0);
    DistY /= clamp(dY - 0.5, 0.7, 1.0);
    float Mod = 0.5 + clamp(dX - 0.0, 0.0, 1.0);
    float Color = min(DistX, DistY);

    return float4(In.Col.rgb * 0.2, 1.0 - clamp(Color, 0.0, 1.0) * 0.95);
//Mod *= 0.5 + clamp(dY,0.0,1.0);
    fragColor = float4(1.0, 1.0, 1.0, Color) - float4(0.0, 1. - Color, 0.0, 0.0);
    fragColor.a *= 0.02;
//fragColor = fragColor - vec4((DistY),0.0,0.0,1.0);
    return fragColor;
    
    if ((uv.x > 0.01 && uv.y > 0.01 && uv.x < 0.99 && uv.y < 0.99) || (dX < 0.3 || dY < 0.3))
    {
        fragColor = 0.0;
        return fragColor;
    }
    
// Time varying pixel color
//vec3 col = 0.5 + 0.5*cos(iTime+uv.xyx+vec3(0,2,4));

// Output to screen
    return 0.0;
}