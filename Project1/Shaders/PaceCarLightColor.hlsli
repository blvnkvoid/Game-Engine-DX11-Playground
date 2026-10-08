float3 PaceCarLightColor()
{
    float flash = step(0.5f, frac(time * 4.0f));

    float3 blue = float3(0.05f, 0.25f, 2.5f);
    float3 amber = float3(2.5f, 0.9f, 0.05f);

    return lerp(blue, amber, flash);
}