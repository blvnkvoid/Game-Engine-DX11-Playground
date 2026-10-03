
// ---------------------------------------------------------
// Tangent basis
// ---------------------------------------------------------

float3 N =
normalize(input.normal);

float3 T =
normalize(input.tangent);

T =
normalize(
    T - N * dot(T, N)
);

float3 B =
normalize(
    cross(N, T)
);

float3x3 TBN =
float3x3(
    T,
    B,
    N
    );


// ---------------------------------------------------------
// CAR BASE NORMAL
// t3
// ---------------------------------------------------------

float3 baseNormal =
normalTexture.Sample(
    samplerLinear,
    input.texCoord
).xyz;

baseNormal =
baseNormal * 2.0f - 1.0f;

baseNormal =
normalize(baseNormal);


// ---------------------------------------------------------
// CAR DETAIL NORMAL
// t5
// ---------------------------------------------------------

float3 detailNormal =
normalDetailTexture.Sample(
    samplerLinear,
    input.texCoord * 60.0f
).xyz;

detailNormal =
detailNormal * 2.0f - 1.0f;

float DETAIL_NORMAL_STRENGTH =
1.0f;

detailNormal.xy *=
DETAIL_NORMAL_STRENGTH;

detailNormal =
normalize(detailNormal);


// ---------------------------------------------------------
// MAP BASE NORMAL
// t8
// ---------------------------------------------------------

float3 mapBaseNormal =
normalMapTexture.Sample(
    samplerLinear,
    input.texCoord
).xyz;

mapBaseNormal =
mapBaseNormal * 2.0f - 1.0f;

mapBaseNormal =
normalize(mapBaseNormal);


// ---------------------------------------------------------
// MAP DETAIL NORMAL
// t10
// ---------------------------------------------------------

float3 mapDetailNormal =
normalDetailMapTexture.Sample(
    samplerLinear,
    input.texCoord * 60.0f
).xyz;

mapDetailNormal =
mapDetailNormal * 2.0f - 1.0f;

mapDetailNormal.xy *=
DETAIL_NORMAL_STRENGTH;

mapDetailNormal =
normalize(mapDetailNormal);


// ---------------------------------------------------------
// Combine CAR normals in tangent space
// ---------------------------------------------------------

float3 combinedNormal =
normalize(
    float3(
        baseNormal.xy +
        detailNormal.xy,

        baseNormal.z *
        detailNormal.z
        )
);


// ---------------------------------------------------------
// Tangent -> world
//
// IMPORTANT:
// Still using CAR normal data here.
// MAP normal data is sampled above but NOT used yet.
// ---------------------------------------------------------

N =
normalize(
    mul(
        combinedNormal,
        TBN
    )
);
