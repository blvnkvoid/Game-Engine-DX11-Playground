float BrakeLightMask(float3 worldPos)
{
    float3 toPixel = worldPos - carPosition.xyz;
    toPixel.y = 0.0f;

    float distance = length(toPixel);
    if (distance < 0.001f)
        return 0.0f;

    float3 dirToPixel = normalize(toPixel);

    float3 rear = -normalize(carForward.xyz);
    rear.y = 0.0f;
    rear = normalize(rear);

    float rearAmount = dot(dirToPixel, rear);

    float cone = smoothstep(0.86f, 0.97f, rearAmount);
    float range = 1.0f - saturate(distance / 10.0f);
    range *= range;

    return cone * range * saturate(brakeAmount);

}