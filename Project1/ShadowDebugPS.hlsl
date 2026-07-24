Texture2D shadowMap : register(t0);
SamplerState shadowSampler : register(s0);

struct PSInput
{
    float4 position : SV_POSITION;
    float2 uv       : TEXCOORD0;
};

float4 main(PSInput input) : SV_TARGET
{
float depth = shadowMap.Sample(shadowSampler, input.uv).r;
return float4(depth, depth, depth, 1.0f);
}