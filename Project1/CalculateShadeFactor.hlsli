float CalculateShadowFactor(float4 lightSpacePos, float3 normal, float3 lightDir)
{
    if (lightSpacePos.w <= 0.0f)
        return 1.0f;

    float3 lightNDC =
        lightSpacePos.xyz /
        lightSpacePos.w;

    float2 shadowUV;
    shadowUV.x = lightNDC.x * 0.5f + 0.5f;
    shadowUV.y = -lightNDC.y * 0.5f + 0.5f;

    float currentDepth = lightNDC.z;

    if (shadowUV.x < 0.0f ||
        shadowUV.x > 1.0f ||
        shadowUV.y < 0.0f ||
        shadowUV.y > 1.0f ||
        currentDepth < 0.0f ||
        currentDepth > 1.0f)
    {
        return 1.0f;
    }

    float ndotl =
        saturate(
            dot(
                normalize(normal),
                normalize(lightDir)
            )
        );

    float bias =
        max(
            0.005f * (1.0f - ndotl),
            0.0005f
        );

    uint width;
    uint height;
    shadowMap.GetDimensions(width, height);

    float2 texelSize =
        1.0f / float2(width, height);

    float2 offset =
        texelSize * 0.5f;

    float visibility = 0.0f;

    visibility += shadowMap.SampleCmpLevelZero(
        shadowComparisonSampler,
        shadowUV + float2(-offset.x, -offset.y),
        currentDepth - bias
    );

    visibility += shadowMap.SampleCmpLevelZero(
        shadowComparisonSampler,
        shadowUV + float2(offset.x, -offset.y),
        currentDepth - bias
    );

    visibility += shadowMap.SampleCmpLevelZero(
        shadowComparisonSampler,
        shadowUV + float2(-offset.x, offset.y),
        currentDepth - bias
    );

    visibility += shadowMap.SampleCmpLevelZero(
        shadowComparisonSampler,
        shadowUV + float2(offset.x, offset.y),
        currentDepth - bias
    );

    return visibility * 0.25f;
}