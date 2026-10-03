
float4 ShadeAsphalt(float4 texColor, float3 N, float3 L, float3 worldPos, float ambient, float headlightIntensity, float3 H, float3 V)
{
    float ndotl =
        saturate(dot(N, L));

    float brakeSpill =
        BrakeLightMask(worldPos);

    // Direct sunlight receives the shadow factor.
    float3 sunDiffuse =
        texColor.rgb *
        lightColor.rgb *
        ndotl *
        0.55f;

    // Ambient remains visible underneath shadows.
    float3 ambientDiffuse =
        texColor.rgb *
        ambient;

    float3 finalColor =
        ambientDiffuse +
        sunDiffuse;

    // ---------------------------------------------------------
    // Headlights
    // ---------------------------------------------------------

    float headlight =
        HeadlightMask(worldPos);

    float3 beamColor =
        float3(
            1.0f,
            0.92f,
            0.72f
            );

    finalColor +=
        beamColor *
        headlight *
        0.5f *
        headlightIntensity;

    // ---------------------------------------------------------
    // Lamps
    // ---------------------------------------------------------

    float lampLight =
        LampLightMask(worldPos);

    float3 lampColor =
        float3(
            1.0f,
            0.82f,
            0.45f
            );

    finalColor +=
        lampColor *
        lampLight *
        0.8f;

    // ---------------------------------------------------------
    // Brake-light spill
    // ---------------------------------------------------------

    finalColor +=
        float3(
            1.0f,
            0.05f,
            0.02f
            ) *
        brakeSpill *
        0.5f;

    // ---------------------------------------------------------
    // Broad sun response on asphalt
    // ---------------------------------------------------------

    float3 toCamera =
        cameraPosition.xyz -
        worldPos;

    float distanceToCamera =
        length(toCamera);

    V =
        normalize(toCamera);

    float ndoth =
        saturate(dot(N, H));

    float beamWidth =
        12.0f;

    float beamStrength =
        0.35f;

    float beamLength =
        500.0f;

    float beam =
        pow(
            ndoth,
            beamWidth
        );

    float distanceMask =
        1.0f -
        smoothstep(
            beamLength * 0.8f,
            beamLength,
            distanceToCamera
        );


    beam *=
        beamStrength;

    beam *=
        distanceMask;


    // This appears to represent direct sunlight,
    // so it must disappear underneath shadows.
    finalColor +=
        lightColor.rgb *
        beam;

    return float4(
        finalColor,
        1.0f
        );
}