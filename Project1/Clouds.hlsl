cbuffer CloudConstants : register(b2)
{
    float2 windDirection;

    float time;
    float speed;

    float coverage;
    float density;

    float scale;
    float brightness;

    float nearPlane;
    float farPlane;

    float2 padding0;
};

Texture2D cloudNoiseTexture       : register(t0);
Texture2D cloudDetailNoiseTexture : register(t1);
Texture2DMS<float> sceneDepthTexture : register(t2);

SamplerState cloudSampler : register(s0);

struct VS_INPUT
{
    float3 position : POSITION;
    float2 texCoord : TEXCOORD;
};

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float2 texCoord : TEXCOORD;
};

PS_INPUT main(VS_INPUT input)
{
    PS_INPUT output;

    output.position = float4(input.position, 1.0f);
    output.texCoord = input.texCoord;

    return output;
}
float LinearizeDepth(float depth)
{
    return (nearPlane * farPlane) /
        (farPlane - depth * (farPlane - nearPlane));
}


float4 PSMain(PS_INPUT input) : SV_TARGET
{
    int2 pixel = int2(input.position.xy);

    float sceneDepth =
        sceneDepthTexture.Load(pixel, 0);

    // Standard DX11 depth:
    // 1.0 means nothing was rendered here — sky/background.
    float skyMask =
        step(0.9999f, sceneDepth);

    float2 windOffset =
        windDirection * time * speed;

    // Low-frequency coordinates used only to bend/distort the cloud field.
    float2 warpUV =
        input.texCoord * scale * 0.45f
        + windOffset * 0.25f;

    // Use two differently offset samples to construct a 2D warp vector.
    float warpX =
        cloudDetailNoiseTexture.Sample(
            cloudSampler,
            warpUV).r;

    float warpY =
        cloudDetailNoiseTexture.Sample(
            cloudSampler,
            warpUV + float2(0.417f, 0.731f)).r;

    // Convert [0, 1] noise into a signed [-1, 1] displacement.
    float2 warp =
        float2(warpX, warpY) * 2.0f - 1.0f;

    // Controls how strongly the cloud coordinates are bent.
    warp *= 0.01f;

    float2 baseUV =
        input.texCoord * scale
        + windOffset
        + warp;

    float2 detailUV =
        input.texCoord * scale * 2.0f
        + windOffset * 1.5f
        + warp * 1.4f;

    float baseNoise =
        cloudNoiseTexture.Sample(
            cloudSampler,
            baseUV).r;

    float detailNoise1 =
        cloudDetailNoiseTexture.Sample(
            cloudSampler,
            detailUV).r;

    float2 detailUV2 =
        input.texCoord * scale * 6.7f
        + float2(-windOffset.y, windOffset.x) * 1.15f
        + float2(0.371f, 0.619f)
        + warp * 2.2f;

    float detailNoise2 =
        cloudDetailNoiseTexture.Sample(
            cloudSampler,
            detailUV2).r;

    float detailNoise =
        detailNoise1 * 0.65f
        + detailNoise2 * 0.35f;

    float combinedNoise =
        baseNoise * 0.8f
        + detailNoise * 0.2f;

    // Cut the noise into distinct cloud shapes.
    float cloudMask =
        smoothstep(
            coverage,
            coverage + 0.12f,
            combinedNoise);

    cloudMask *= density;
    cloudMask = saturate(cloudMask);

    // Clouds only exist where the depth buffer contains sky.
    cloudMask *= skyMask;

    float cloudValue =
        brightness * cloudMask;

    return float4(
        cloudValue,
        cloudValue,
        cloudValue,
        cloudMask);
}