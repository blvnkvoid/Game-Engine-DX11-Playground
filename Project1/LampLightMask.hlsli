float LampLightMask(float3 worldPos)
{
    float closest = 1e9f;
    for (uint i = 0; i < lampCount; ++i)
    {
        float2 roadPos = worldPos.xz;
        float2 lampPos = lampLights[i].position.xz;

        float d = distance(roadPos, lampPos);
        closest = min(closest, d);
    }

    return saturate(1.0f - closest / 45.0f);
}