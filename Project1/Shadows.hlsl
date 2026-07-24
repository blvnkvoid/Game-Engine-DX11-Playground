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

struct VSInput
{
    float3 position : POSITION;
};

struct VSOutput
{
    float4 position : SV_POSITION;
};

VSOutput main(VSInput input)
{
    VSOutput output;

    float4 worldPosition =
        mul(float4(input.position, 1.0f), world);

    float4 viewPosition =
        mul(worldPosition, view);

    output.position =
        mul(viewPosition, projection);

    // Preserve triangle position and shape,
    // but force all rasterized geometry to depth 0.5.
    output.position.z = 0.5f * output.position.w;

    return output;


}