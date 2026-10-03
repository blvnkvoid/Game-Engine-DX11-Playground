#include "MaterialTypes.hlsli"

struct SharedMaterial {
    float3 diffuseColor;
    float  materialType;
    float3 specularColor;
    float  specularPower;
    float3 ambientColor;
    float d;
};

cbuffer SharedSceneData : register(b0)
{
    matrix world;
    matrix view;
    matrix projection;

    matrix lightView;
    matrix lightProjection;

    float4 lightDirection;
    float4 lightColor;
    float4 cameraPosition;
    float4 cameraDirection;

    float4 carPosition;
    float4 carForward;

    SharedMaterial material;

    float brakeAmount;
    float time;
    float2 padding;

    float ambientIntensity;
    float headlightIntensity;

    float2 environmentPadding;
};

struct LampData
{
    float3 position;
    float radius;
};

StructuredBuffer<LampData> lampLights : register(t1);

cbuffer LampInfo : register(b1)
{
    int lampCount;
    float3 lampInfoPadding;
};

struct VS_INPUT {
    float3 position : POSITION;
    float4 color    : COLOR;
    float2 texCoord : TEXCOORD;
    float3 normal   : NORMAL;
    float4 tangent  : TANGENT;
    
};

struct PS_INPUT {
    float4 position : SV_POSITION;
    float2 texCoord : TEXCOORD;
    float3 normal   : TEXCOORD1;
    float3 worldPos : TEXCOORD2;
    float3 localPos : TEXCOORD3;
    float4 lightSpacePos : TEXCOORD4;
    float clipW : TEXCOORD5;
    float3 tangent : TEXCOORD6;
};

Texture2D objTexture : register(t0);
Texture2D shadowMap : register(t2);
Texture2D normalTexture : register(t3);
Texture2D detailTexture : register(t4);
Texture2D normalDetailTexture : register(t5);
Texture2D mapsTexture : register(t6);
Texture2D normalMapTexture : register(t8);
Texture2D detailMapTexture : register(t9);
Texture2D normalDetailMapTexture : register(t10);
Texture2D mapsMapTexture : register(t11);

SamplerState samplerLinear : register(s0);
SamplerComparisonState shadowComparisonSampler : register(s1);

// --- Vertex Shader ---
PS_INPUT VS(VS_INPUT input) {
    PS_INPUT output;
    float4 worldPosition = mul(float4(input.position, 1.0f), world);
    output.lightSpacePos = mul(worldPosition, lightView);
    output.lightSpacePos = mul(output.lightSpacePos, lightProjection);
    output.worldPos = worldPosition.xyz;
    output.position = mul(mul(worldPosition, view), projection);
    output.normal = normalize(mul((float3x3)world, input.normal));
    output.texCoord = input.texCoord;
    output.localPos = input.position.xyz;
    output.tangent =  normalize(mul((float3x3)world, input.tangent.xyz));
    float4 clipPos = mul(mul(worldPosition, view), projection);

    output.position = clipPos;
    output.clipW = clipPos.w;
    return output;
}

#include "GetSkyReflection.hlsli"
#include "ShadeAlcantara.hlsli"
#include "ShadeCarLivery.hlsli"
#include "ShadeCarPaint.hlsli"
#include "PaceCarLightColor.hlsli"
#include "PaceBodyMask.hlsli"
#include "ShadeSafetyCarPaint.hlsli"
#include "ShadeGlass.hlsli"
#include "ShadeRubber.hlsli"
#include "HeadlightMask.hlsli"
#include "BrakeLightMask.hlsli"
#include "ShadeBrakeLight.hlsli"
#include "ShadePaceLight.hlsli"
#include "PaceLightMask.hlsli"
#include "LampLightMask.hlsli"
#include "ShadeLampGlow.hlsli"
#include "ShadeAsphalt.hlsli"
#include "ShadeHeadLight.hlsli"
#include "ShadeDecalText.hlsli"
#include "CalculateShadeFactor.hlsli"

// --- Pixel Shader ---
float4 PS(PS_INPUT input) : SV_Target
{
    int matType = (int)material.materialType;


// ---------------------------------------------------------
// DIFFUSE
// ---------------------------------------------------------

float4 texColor =
    objTexture.Sample(
        samplerLinear,
        input.texCoord
    );


float alphaMask =
    texColor.a;

float diffuseMask =
    texColor.r;


// ---------------------------------------------------------
// CAR TEXTURE DATA
// t3 - t6
// ---------------------------------------------------------

float AO =
    mapsTexture.Sample(
        samplerLinear,
        input.texCoord
    ).r;


float4 detailSample =
    detailTexture.Sample(
        samplerLinear,
        input.texCoord * 60.0f
    );

float leatherDetail =
    detailSample.r;


// ---------------------------------------------------------
// MAP TEXTURE DATA
// t8 - t11
// ---------------------------------------------------------

float mapAO =
    mapsMapTexture.Sample(
        samplerLinear,
        input.texCoord
    ).r;


float4 mapDetailSample =
    detailMapTexture.Sample(
        samplerLinear,
        input.texCoord * 60.0f
    );

float mapDetail =
    mapDetailSample.r;

#include "NormalMapping.hlsli"

// ---------------------------------------------------------
// Lighting vectors
// ---------------------------------------------------------

float3 L =
    normalize(
        -lightDirection
    );

float3 V =
    normalize(
        cameraPosition.xyz -
        input.worldPos
    );

float3 H =
    normalize(
        L + V
    );

float3 I =
    normalize(
        input.worldPos -
        cameraPosition.xyz
    );

float3 R =
    reflect(
        I,
        N
    );

// ---------------------------------------------------------
// Shadow mapping
// ---------------------------------------------------------

float shadowFactor =
    CalculateShadowFactor(
        input.lightSpacePos,
        N,
        L
    );

// ---------------------------------------------------------
// Local lighting
// ---------------------------------------------------------

#include "LocalLighting.hlsli"

// ---------------------------------------------------------
// Material dispatch
// ---------------------------------------------------------

#include "Materials.hlsli"

// ---------------------------------------------------------
// Fallback
// ---------------------------------------------------------

#include "Fallback.hlsli"
}
PS_INPUT mainVS(VS_INPUT input) { return VS(input); }

PS_INPUT main(VS_INPUT input) { return VS(input); }