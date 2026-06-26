// =============================================================================
// Shaders.hlsl - Fixed LightDirection and Color for Stable Rendering
// =============================================================================

struct SharedMaterial {
    float3 diffuseColor;
    float  matPad1;
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
    float4 lightDirection_UNUSED; // Kept for alignment, but ignored
    float4 lightColor_UNUSED;     // Kept for alignment, but ignored
    float4 cameraPosition;
    float4 cameraDirection;
    SharedMaterial material;
};

// Hardcoded light constants to bypass buffer synchronization issues
static const float3 lightDirection = float3(0.5f, -1.0f, 0.5f);
static const float3 lightColor = float3(1.0f, 1.0f, 1.0f);

struct VS_INPUT {
    float3 position : POSITION;
    float4 color    : COLOR;
    float2 texCoord : TEXCOORD;
    float3 normal   : NORMAL;
};

struct PS_INPUT {
    float4 position : SV_POSITION;
    float2 texCoord : TEXCOORD;
    float3 normal   : TEXCOORD1;
    float3 worldPos : TEXCOORD2;
};

Texture2D objTexture : register(t0);
SamplerState samplerLinear : register(s0);

// --- Vertex Shader ---
PS_INPUT VS(VS_INPUT input) {
    PS_INPUT output;
    float4 worldPosition = mul(float4(input.position, 1.0f), world);
    output.worldPos = worldPosition.xyz;
    output.position = mul(mul(worldPosition, view), projection);
    output.normal = normalize(mul((float3x3)world, input.normal));
    output.texCoord = input.texCoord;
    return output;
}

float4 PS(PS_INPUT input) : SV_Target{



    float4 texColor = objTexture.Sample(samplerLinear, input.texCoord);
    clip(texColor.a - 0.001f);
    


    // 1. Lighting vectors
    float3 N = normalize(input.normal);
    float3 L = normalize(-lightDirection); // Now uses static const
    float3 V = normalize(cameraPosition.xyz - input.worldPos);
    float3 H = normalize(L + V);
    float3 I = normalize(input.worldPos - cameraPosition.xyz);
    float3 R = reflect(I, N);

    if (length(material.diffuseColor) < 0.01f)
    {
        float diffuseInt = saturate(dot(N, L)) * 0.7f + 0.5f;
        return float4(texColor.rgb * diffuseInt * lightColor, 1.0f);
    }

    // 2. TIGHTENED FRESNEL
    float fresnel = pow(1.0 - saturate(dot(N, V)), 64.0);

    // Fake Sky Reflection
    float skyFactor = saturate(R.y * 0.5 + 0.5);
    float3 mirrorColor = lerp(float3(0.05, 0.05, 0.1), float3(0.8, 0.9, 1.0), skyFactor);
    float3 finalMirror = mirrorColor * material.specularColor * fresnel * 1.0f;

    // 3. DIFFUSE
    float diffuseIntensity = saturate(dot(N, L)) * 0.9 + 0.1;
    float3 safeDiffuseColor = material.diffuseColor;
    float3 diffuse = diffuseIntensity * safeDiffuseColor;

    // 4. DUAL-COAT SPECULAR GLINT
    float specClearCoat = pow(saturate(dot(N, H)), 512.0f);
    float specFlakes = pow(saturate(dot(N, H)), 51.2f);
    float3 reflection = material.specularColor * ((specClearCoat * 2.5f) + (specFlakes * 0.3f));

    // AMBIENT
    float3 safeAmbient = max(material.ambientColor, float3(0.4f, 0.4f, 0.4f));
    float3 ambient = safeAmbient * 0.3f;
    float3 finalColor;

    // Combine everything
    if (material.d < 0.9f) {
        finalColor = (ambient + reflection) * lightColor + (finalMirror * 2.0f);
    }
    else
    {
        // 1. Oblicz jasnosc swiatla (0.0 do 1.0+)
        float3 totalLight = (diffuse + reflection + ambient) * lightColor;

        // 2. Wblenduj teksture zamiast mnozyc wszystko przez safeDiffuseColor
        // Uzywamy lerp, zeby polaczyc teksture z oswietleniem, zachowujac jej oryginalny odcien
        float3 litTexture = texColor.rgb * totalLight;

        // finalColor = (to co daje tekstura) + (lustrzane blaski)
        finalColor = litTexture + finalMirror;
    }

        if (material.specularPower == 0.0f)
        {
            float diffuseInt = abs(dot(N, L)) * 0.7 + 0.6;
            float3 baseLight = lightColor * diffuseInt;
            float3 boostedTex = pow(texColor.rgb, 0.85f);
            return float4(boostedTex * baseLight, 1.0f);
        }

   return float4(finalColor, 1.0f);
}

PS_INPUT mainVS(VS_INPUT input) { return VS(input); }
float4 mainPS(PS_INPUT input) : SV_Target{ return PS(input); }
PS_INPUT main(VS_INPUT input) { return VS(input); }