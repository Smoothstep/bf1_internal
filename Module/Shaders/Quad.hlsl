struct VS_INPUT
{
    float3 vPos : POSITION;
    float2 vTex : TEXCOORD0;
};

struct PS_INPUT
{
    float4 vPos : SV_POSITION;
    float2 vTex : TEXCOORD0;
};

PS_INPUT VSMain(VS_INPUT IN)
{
    PS_INPUT OUT;

    float4 pos = float4(IN.vPos, 1.0f);

    OUT.vPos = pos;
    OUT.vTex = IN.vTex;

    return OUT;
}

PS_INPUT VSMain2(uint VertexID: SV_VertexID)
{
	PS_INPUT Out;
	
	Out.vTex = float2((VertexID << 1) & 2, VertexID & 2);
	Out.vPos = float4(Out.vTex * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f), 0.0f, 1.0f);

	return Out;
}