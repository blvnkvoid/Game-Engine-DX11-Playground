float4 ShadeLampGlow(float4 texColor, float2 uv)
{
    // keep original texture
    float3 base = texColor.rgb;

    // fake glow mask from UV center
    float2 center = uv - float2(0.5f, 0.5f);
    float dist = length(center);

    float glow = saturate(1.0f - dist * 2.2f);
    glow = glow * glow;

    float3 glowColor = float3(1.0f, 0.82f, 0.45f);

    float3 finalColor = base + glowColor * glow * 0.8f;

    return float4(finalColor, texColor.a);
}