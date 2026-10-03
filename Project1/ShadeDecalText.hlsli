float4 ShadeDecalText(float4 texColor, float3 N, float3 L)
{
    clip(texColor.a - 0.5f);

    float ndotl =
        saturate(dot(N, L));

    float ambientLight =
        0.25f;

    float directLight =
        ndotl *
        0.75f
        ;

    float light =
        ambientLight +
        directLight;

    return float4(
        texColor.rgb * light,
        1.0f
        );
}