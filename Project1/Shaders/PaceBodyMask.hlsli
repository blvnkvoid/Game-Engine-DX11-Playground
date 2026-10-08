float PaceBodyMask(float3 localPos)
{
    float roofY = smoothstep(0.8f, 1.15f, localPos.y);

    float centerX =
        1.0f - smoothstep(0.6f, 1.2f, abs(localPos.x));

    float roofZ =
        1.0f - smoothstep(0.5f, 1.8f, abs(localPos.z));

    return roofY * centerX * roofZ;
}