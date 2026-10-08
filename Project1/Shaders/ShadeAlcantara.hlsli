float4 ShadeAlcantara(float4 texColor, float3 N, float3 L)
{
    float ndotl =
        saturate(dot(N, L));

    float3 base =
        texColor.rgb;

    float lum =
        dot(
            base,
            float3(
                0.299f,
                0.587f,
                0.114f
                )
        );

    base =
        lerp(
            base,
            lum.xxx,
            0.25f
        );

    float ambientLight =
        0.18f;

    float directLight =
        ndotl *
        0.35f;

    float light =
        ambientLight +
        directLight;

    return float4(
        base * light,
        1.0f
        );
}