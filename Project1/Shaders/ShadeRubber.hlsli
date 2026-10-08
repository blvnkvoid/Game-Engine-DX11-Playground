float4 ShadeRubber(float4 texColor, float3 N, float3 L)
{
    float ndotl =
        saturate(dot(N, L));

    float3 base =
        texColor.rgb;

    base =
        lerp(
            base,
            float3(
                0.02f,
                0.02f,
                0.02f
                ),
            0.20f
        );

    float ambientLight =
        0.35f;

    float directLight =
        ndotl *
        0.55f;

    float light =
        ambientLight +
        directLight;

    return float4(
        base * light,
        1.0f
        );
}