float4 ShadeCarPaint(
    float4 texColor,
    float3 N,
    float3 L,
    float3 V,
    float3 H,
    float3 R,
    float3 worldPos
)
{
    float ndotl =
        saturate(dot(N, L));

    float fresnel =
        pow(
            1.0f - saturate(dot(N, V)),
            5.0f
        );

    float3 base =
        texColor.rgb;// * material.diffuseColor.rgb;



    // Constant indirect/ambient component.
    float3 ambientDiffuse =
        base *
        0.15f;

    // Sun-controlled diffuse component.
    float3 sunDiffuse =
        base *
        ndotl *
        0.25f;

    float clearCoat =
        pow(
            saturate(dot(N, H)),
            384.0f
        ) *
        2.0f;

    float broadSpec =
        pow(
            saturate(dot(N, H)),
            48.0f
        ) *
        0.25f;

    // Environment reflection remains visible in shadow.
    float3 sky =
        GetSkyReflection(R) *
        fresnel *
        0.35f;

    // Direct sunlight specular disappears in shadow.
    float3 sunSpecular =
        lightColor.rgb *
        material.specularColor *
        (clearCoat + broadSpec);

    float3 color =
        ambientDiffuse +
        sunDiffuse +
        sunSpecular;

    return float4(
        color,
        1.0f
        );
}