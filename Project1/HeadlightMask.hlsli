float HeadlightMask(float3 worldPos)
{
    float3 toPixel = worldPos - carPosition.xyz;
    toPixel.y = 0.0f;

    float distance = length(toPixel);

    if (distance < 0.001f)
        return 0.0f;

    float3 dirToPixel = normalize(toPixel);

    float3 forward = normalize(carForward.xyz);
    forward.y = 0.0f;
    forward = normalize(forward);

    float forwardAmount = dot(dirToPixel, forward);

    float cone = smoothstep(0.82f, 0.995f, forwardAmount);
    float range = 1.0f - saturate(distance / 350.0f);
    range *= range;
    float vertical = 1.0f - saturate(abs(worldPos.y - carPosition.y) / 24.0f);
    vertical *= vertical;

    float nearFade = smoothstep(0.0f, 9.0f, distance);

    return cone * range * nearFade * vertical;
}