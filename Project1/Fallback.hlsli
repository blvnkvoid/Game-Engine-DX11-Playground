// ---------------------------------------------------------
// Generic fallback material
// ---------------------------------------------------------

float ndotl =
saturate(
    dot(N, L)
);


float3 baseColor = texColor.rgb;


if (length(material.diffuseColor) >= 0.01f)
{
    baseColor *=
        material.diffuseColor;
}


// ---------------------------------------------------------
// DETAIL TEST
//
// Keep OFF for now.
//
// This uses CAR t4.
// ---------------------------------------------------------

//baseColor *= leatherDetail;


// ---------------------------------------------------------
// MAP DETAIL TEST
//
// Also keep OFF for now.
//
// This uses MAP t9.
// ---------------------------------------------------------

// baseColor *= mapDetail;


float modelAmbientMultiplier =
3.0f;


// ---------------------------------------------------------
// Ambient
// ---------------------------------------------------------

float3 ambient =
baseColor *
ambientIntensity *
modelAmbientMultiplier;


// ---------------------------------------------------------
// CAR AO
//
// Still using CAR t6 here.
// Keep this in mind during the map test.
// ---------------------------------------------------------

ambient *= AO;


// ---------------------------------------------------------
// Direct sunlight
// ---------------------------------------------------------

float3 diffuse =
baseColor *
lightColor.rgb *
ndotl *
0.9f *
shadowFactor;


// ---------------------------------------------------------
// Non-specular material
// ---------------------------------------------------------

if (material.specularPower == 0.0f)
{
    float3 finalColor =
        ambient +
        diffuse +
        localLighting;

    return float4(
        finalColor,
        1.0f
        );
}


// ---------------------------------------------------------
// Fresnel
// ---------------------------------------------------------

float fresnel =
pow(
    1.0f -
    saturate(
        dot(N, V)
    ),
    64.0f
);


// ---------------------------------------------------------
// Fake sky reflection
// ---------------------------------------------------------

float skyFactor =
saturate(
    R.y * 0.5f +
    0.5f
);


float3 mirrorColor =
lerp(
    float3(
        0.05f,
        0.05f,
        0.10f
        ),
    float3(
        0.80f,
        0.90f,
        1.00f
        ),
    skyFactor
);


float3 finalMirror =
float3(
    0.0f,
    0.0f,
    0.0f
    );


// ---------------------------------------------------------
// Direct sunlight specular
// ---------------------------------------------------------

float specClearCoat =
pow(
    saturate(
        dot(N, H)
    ),
    512.0f
) *
2.5f;


float specFlakes =
pow(
    saturate(
        dot(N, H)
    ),
    51.2f
) *
0.3f;


float3 reflection =
material.specularColor *
(
    specClearCoat +
    specFlakes
    ) *
    lightColor.rgb *
    shadowFactor;


// ---------------------------------------------------------
// Transparent fallback
// ---------------------------------------------------------

if (material.d < 0.9f)
{
    float3 finalColor =
        ambient +
        reflection +
        finalMirror * 2.0f +
        localLighting;

    return float4(
        finalColor,
        material.d
        );
}


// ---------------------------------------------------------
// Opaque fallback
// ---------------------------------------------------------

float3 finalColor =
baseColor +
ambient +
diffuse +
reflection +
finalMirror +
localLighting;


return float4(
    finalColor,
    1.0f
    );