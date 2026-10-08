float SampleCascadeShadow(
    uint cascadeIndex,
    float2 uv,
    float depth)
{
    if (cascadeIndex == 0)
    {
        return shadowMap0.SampleCmpLevelZero(
            shadowComparisonSampler,
            uv,
            depth
        );
    }
    else if (cascadeIndex == 1)
    {
        return shadowMap1.SampleCmpLevelZero(
            shadowComparisonSampler,
            uv,
            depth
        );
    }

    return shadowMap2.SampleCmpLevelZero(
        shadowComparisonSampler,
        uv,
        depth
    );
}


float2 GetCascadeTexelSize(uint cascadeIndex)
{
    uint width = 1;
    uint height = 1;

    if (cascadeIndex == 0)
    {
        shadowMap0.GetDimensions(width, height);
    }
    else if (cascadeIndex == 1)
    {
        shadowMap1.GetDimensions(width, height);
    }
    else
    {
        shadowMap2.GetDimensions(width, height);
    }

    return 1.0f / float2(width, height);
}


float CalculateShadowFactor(
    float4 lightSpacePos,
    float3 normal,
    float3 lightDir,
    uint cascadeIndex)
{
    if (lightSpacePos.w <= 0.0f)
        return 1.0f;

    float3 lightNDC =
        lightSpacePos.xyz / lightSpacePos.w;

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

    float2 texelSize =
        GetCascadeTexelSize(cascadeIndex);

    float2 offset =
        texelSize * 0.5f;

    float compareDepth =
        currentDepth - bias;

    float visibility = 0.0f;

    visibility += SampleCascadeShadow(
        cascadeIndex,
        shadowUV + float2(-offset.x, -offset.y),
        compareDepth
    );

    visibility += SampleCascadeShadow(
        cascadeIndex,
        shadowUV + float2(offset.x, -offset.y),
        compareDepth
    );

    visibility += SampleCascadeShadow(
        cascadeIndex,
        shadowUV + float2(-offset.x, offset.y),
        compareDepth
    );

    visibility += SampleCascadeShadow(
        cascadeIndex,
        shadowUV + float2(offset.x, offset.y),
        compareDepth
    );

    return visibility * 0.25f;
}