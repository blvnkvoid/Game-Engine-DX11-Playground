float4 ShadePaceLight(float4 texColor)
{
    // Flash between blue and amber
    float flash = step(0.5f, frac(time * 4.0f));

    float3 blue = float3(0.1f, 0.6f, 18.0f);
    float3 amber = float3(18.0f, 7.0f, 0.2f);

    float3 color = lerp(blue, amber, flash);

    return float4(color, 1.0f);
}