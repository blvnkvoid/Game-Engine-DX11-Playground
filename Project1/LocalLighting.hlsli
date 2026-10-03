float headlight =
HeadlightMask(
    input.worldPos
);

float brakeSpill =
BrakeLightMask(
    input.worldPos
);

float lampLight =
LampLightMask(
    input.worldPos
);


float carLampMultiplier =
0.05f;

lampLight *=
carLampMultiplier;


float3 localLighting =
0.0f;


localLighting +=
float3(
    1.0f,
    0.92f,
    0.72f
    ) *
    headlight *
    0.5f *
    headlightIntensity;


localLighting +=
float3(
    1.0f,
    0.05f,
    0.02f
    ) *
    brakeSpill *
    0.5f;


localLighting +=
float3(
    1.0f,
    0.82f,
    0.45f
    ) *
    lampLight *
    0.8f;