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
    float2 paddingNearFar;

    float3 cloudColor;
    float padding0;

    float3 cloudShadowColor;
    float padding1;
};


cbuffer CloudCameraConstants : register(b3)
{
    float4x4 inverseViewProjection;
    float3 cameraWorldPosition;
    float cloudHeight;
};


Texture2D cloudNoiseTexture          : register(t0);
Texture2D cloudDetailNoiseTexture    : register(t1);
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

    output.position =
        float4(input.position, 1.0f);

    output.texCoord =
        input.texCoord;

    return output;
}


float LinearizeDepth(float depth)
{
    return (nearPlane * farPlane) /
        (farPlane - depth * (farPlane - nearPlane));
}


float4 PSMain(PS_INPUT input) : SV_TARGET
{
    int2 pixel =
        int2(input.position.xy);

    float sceneDepth =
        sceneDepthTexture.Load(pixel, 0);

    // Standard DX11 depth:
    // 1.0 means nothing was rendered here.
    float skyMask =
        step(0.9999f, sceneDepth);

    float2 ndc;

    ndc.x =
        input.texCoord.x * 2.0f - 1.0f;

    ndc.y =
        1.0f - input.texCoord.y * 2.0f;

    float4 worldFar =
        mul(
            float4(ndc, 1.0f, 1.0f),
            inverseViewProjection);

    worldFar.xyz /=
        worldFar.w;

    float3 rayOrigin =
        cameraWorldPosition;

    float3 rayDirection =
        normalize(
            worldFar.xyz - rayOrigin);

    // The cloud plane exists above the camera.
    // Rays pointing downward or almost parallel to it cannot hit it.
    if (rayDirection.y <= 0.0001f)
    {
        return float4(
            0.0f,
            0.0f,
            0.0f,
            0.0f);
    }

    float distanceToCloudPlane =
        (cloudHeight - rayOrigin.y) /
        rayDirection.y;

    if (distanceToCloudPlane <= 0.0f)
    {
        return float4(
            0.0f,
            0.0f,
            0.0f,
            0.0f);
    }

    float3 cloudWorldPosition =
        rayOrigin +
        rayDirection * distanceToCloudPlane;

    // Converts world units into usable noise coordinates.
    // This controls the overall physical size of the cloud formations.
    float2 worldUV =
        cloudWorldPosition.xz * 0.0005f;

    float2 windOffset =
        windDirection * time * speed;

    // Low-frequency coordinates used to bend the cloud field.
    float2 warpUV =
        worldUV * scale * 0.45f
        + windOffset * 0.25f;

    float warpX =
        cloudDetailNoiseTexture.Sample(
            cloudSampler,
            warpUV).r;

    float warpY =
        cloudDetailNoiseTexture.Sample(
            cloudSampler,
            warpUV + float2(0.417f, 0.731f)).r;

    float2 warp =
        float2(warpX, warpY) * 2.0f - 1.0f;

    warp *= 0.01f;

    float2 baseUV =
        worldUV * scale
        + windOffset
        + warp;

    float2 detailUV =
        worldUV * scale * 2.0f
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
        worldUV * scale * 6.7f
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

    float cloudMask =
        smoothstep(
            coverage,
            coverage + 0.12f,
            combinedNoise);

    cloudMask *= density;
    cloudMask = saturate(cloudMask);

    // Fade the plane near the horizon, where the intersection distance
    // approaches infinity and causes severe stretching.
    float horizonFade =
        smoothstep(
            0.015f,
            0.08f,
            rayDirection.y);

    cloudMask *= horizonFade;

    // Clouds only render where the scene depth contains the sky.
    cloudMask *= skyMask;

    float3 finalCloudColor =
        cloudColor *
        brightness *
        cloudMask;

    return float4(
        finalCloudColor,
        cloudMask);
}