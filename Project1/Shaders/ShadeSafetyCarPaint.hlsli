float4 ShadeSafetyCarPaint(float4 texColor, float3 N, float3 L, float3 V, float3 H, float3 R, float3 localPos)
{
    float ndotl =
        saturate(dot(N, L));

    float fresnel =
        pow(
            1.0f - saturate(dot(N, V)),
            5.0f
        );

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
        2.0f;

    float broadSpec =
        pow(
            saturate(dot(N, H)),
            48.0f
        ) *
        0.25f;

    float3 sky =
        GetSkyReflection(R) *
        fresnel *
        0.35f;

    float3 sunSpecular =
        material.specularColor *
        lightColor.rgb *
        (clearCoat + broadSpec);

    float paceFresnel =
        pow(
            1.0f - saturate(dot(N, V)),
            2.0f
        );

    float topBias =
        saturate(
            N.y * 0.5f +
            0.5f
        );

    float paceMask =
        PaceBodyMask(localPos);

    float rim =
        pow(
            1.0f - saturate(dot(N, V)),
            24.0f
        );

    float3 paceReflection =
        PaceCarLightColor() *
        lightColor.rgb *
        paceFresnel *
        topBias *
        rim *
        paceMask *
        12.2f;

    float3 color =
        ambientDiffuse +
        sunDiffuse +
        sky +
        sunSpecular +
        paceReflection;

    return float4(
        color,
        1.0f
        );
}