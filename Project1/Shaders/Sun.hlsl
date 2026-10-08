cbuffer SunConstants : register(b0)
{
    matrix viewProjection;

    float4 sunPosition;
    float4 cameraRight;
    float4 cameraUp;

    float4 sunColor;

    float sunSize;
    float sunIntensity;
    float2 padding;
};

struct VSInput
{
    float2 position : POSITION;
    float2 texCoord : TEXCOORD;
};

struct PSInput
{
    float4 position : SV_POSITION;
    float2 texCoord : TEXCOORD;
};

PSInput main(VSInput input)
{
    PSInput output;

    float3 worldPosition =
        sunPosition.xyz
        + cameraRight.xyz * input.position.x * sunSize
        + cameraUp.xyz * input.position.y * sunSize;

    output.position =
        mul(float4(worldPosition, 1.0f), viewProjection);

    output.texCoord = input.texCoord;

    return output;
}

float4 SunPS(PSInput input) : SV_Target
{
    // Convert UV into coordinates from -1 to +1.
    float2 centeredUV = input.texCoord * 2.0f - 1.0f;

    float distanceFromCenter = length(centeredUV);

    // Hard central sun disc.
    float disc =
        1.0f - smoothstep(
            0.72f,
            0.78f,
            distanceFromCenter);

    // Broad soft glow around the sun.
    float glow =
        1.0f - smoothstep(
            0.15f,
            1.0f,
            distanceFromCenter);

    glow = glow * glow;

    // Very broad atmospheric halo.
    float halo =
        1.0f - saturate(distanceFromCenter);

    halo = pow(halo, 3.0f);

    float intensity =
        disc * 1.4f
        + glow * 0.65f
        + halo * 0.25f;

    float3 color =
        sunColor.rgb
        * intensity
        * sunIntensity;

    // Alpha drives additive blending strength.
    float alpha =
        saturate(
            disc
            + glow * 0.75f
            + halo * 0.25f);
    return float4(color, alpha);
}
