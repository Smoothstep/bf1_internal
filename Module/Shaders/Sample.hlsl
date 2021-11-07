Texture2D Texture : register(t0);
Texture2D TextureBlur : register(t1);

SamplerState Sampler : register(s0);

cbuffer ShaderConstants
{
	uint2 Resolution;
};

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

static const float KernelOffsets[3] = { 0.0f, 1.3846153846f, 3.2307692308f };
static const float BlurWeights[3] = { 0.2270270270f, 0.3162162162f, 0.0702702703f };

float4 MainReverse(PS_INPUT IN) : SV_Target
{
    return Texture.Sample(Sampler, float2(IN.vTex.x, 1-IN.vTex.y)).rgba;
}

  
float3 BlurValue3(float2 uv) 
{
    float Pi2 = 6.28318530718;
	float Directions = 32.0f;
	float Quality = 32.0f;
	float Size = 16.0f;
	float Radius = Size / float2(2560.0f, 1440.0f);
	
	float3 color = TextureBlur.Sample(Sampler, uv).rgb;
	for (float d = 0.0f; d < Pi2; d += Pi2 / Directions)
	{
		for (float i = 1.0f / Quality; i <= 1.0; i += 1.0f / Quality)
		{
			color += TextureBlur.Sample(Sampler, uv + float2(cos(d), sin(d)) * Radius).rgb; 
		}
	}
	
	return color / (Quality * Directions - 15.0f);
}

  
float3 BlurValue2(float2 uv, float2 resolution, float2 direction) 
{
	float3 color = 0.0f;
	float2 off1 = direction * 1.3846153846f;
	float2 off2 = direction * 3.2307692308f;
	float2 off3 = direction * 5.176470588235294f;
	color += TextureBlur.Sample(Sampler, uv).rgb * 0.1964825501511404f;
	color += TextureBlur.Sample(Sampler, uv + (off1 / resolution)).rgb * 0.2969069646728344f;
	color += TextureBlur.Sample(Sampler, uv - (off1 / resolution)).rgb * 0.2969069646728344f;
	color += TextureBlur.Sample(Sampler, uv + (off2 / resolution)).rgb * 0.09447039785044732f;
	color += TextureBlur.Sample(Sampler, uv - (off2 / resolution)).rgb * 0.09447039785044732f;
	color += TextureBlur.Sample(Sampler, uv + (off3 / resolution)).rgb * 0.010381362401148057f;
	color += TextureBlur.Sample(Sampler, uv - (off3 / resolution)).rgb * 0.010381362401148057f;
	return color;
}

float3 BlurValue(float2 uv, float alpha)
{
	if (alpha <= 0) {
		return 0.0f;
	}
	
	float3 textureColor = TextureBlur.Sample(Sampler, uv).xyz * BlurWeights[0];
	
	for (int i = 1; i < 3; i++)
	{
		float2 normalizedOffset = float2(KernelOffsets[i], 0.0f) / 2560.0f;
		textureColor += TextureBlur.Sample(Sampler, uv + normalizedOffset).xyz * BlurWeights[i];
		textureColor += TextureBlur.Sample(Sampler, uv - normalizedOffset).xyz * BlurWeights[i];
	}
	
	textureColor *= alpha;
	
	return textureColor;
}

float4 MainBlurReverse(PS_INPUT IN) : SV_Target
{
    float4 color = Texture.Sample(Sampler, float2(IN.vTex.x, 1-IN.vTex.y));
	float3 blur = BlurValue2(IN.vTex, float2(2560.0f, 1440.0f), float2(0.0f, 1.0f));
	
	return float4(lerp(color.xyz, blur, color.a), color.a);
}

float4 Main(PS_INPUT IN) : SV_Target
{
    return Texture.Sample(Sampler, IN.vTex).rgba;
}

float4 MainBlur(PS_INPUT IN) : SV_Target
{
    float4 color = Texture.Sample(Sampler, IN.vTex);
	float3 blur = BlurValue2(IN.vTex, float2(2560.0f, 1440.0f), float2(0.0f, 5.0f * color.a));
	
	return float4(color.rgb * color.a + blur.rgb * (1.0f - color.a), color.a);
}

float4 MainA(PS_INPUT IN) : SV_Target
{
    return Texture.Sample(Sampler, IN.vTex);
}