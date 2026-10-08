float PaceLightMask(float3 worldPos)
{
    float3 toPixel = worldPos - carPosition.xyz;
    toPixel.y = 0.0f;

    float distance = length(toPixel);

    float range = 1.0f - saturate(distance / 1.0f);
    range *= range;

    return range;
}
