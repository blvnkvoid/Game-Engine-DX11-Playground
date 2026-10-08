float4 ShadeCarLivery(float4 texColor, float3 N, float3 L, float3 V, float3 H, float3 R, float3 localLighting)
{
    float ndotl =
        saturate(dot(N, L));

    float fresnel =
        pow(
            1.0f - saturate(dot(N, V)),
            5.0f
        );

    // Livery texture remains authoritative.
    float3 base =
        texColor.rgb;

    float3 ambientDiffuse =
        base *
        0.25f;

    float3 sunDiffuse =
        base *
        ndotl *
        0.75f;

    float clearCoat =
        pow(
            saturate(dot(N, H)),
            384.0f
        ) *
        1.3f;

    float broadSpec =
        pow(
            saturate(dot(N, H)),
            48.0f
        ) *
        0.18f;

    float3 sky =
        GetSkyReflection(R) *
        fresnel *
        0.22f;

    float3 sunSpecular =
        material.specularColor *
        lightColor.rgb *
        (clearCoat + broadSpec);

    float3 color =
        ambientDiffuse +
        sunDiffuse +
        sky +
        sunSpecular +
        localLighting;

    return float4(
        color,
        1.0f
        );
}