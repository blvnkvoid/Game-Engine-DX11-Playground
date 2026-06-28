// =============================================================================
// Shaders.hlsl - Fixed LightDirection and Color for Stable Rendering
// =============================================================================

#define MATERIAL_DEFAULT     0
#define MATERIAL_CAR_PAINT   1
#define MATERIAL_GLASS       2
#define MATERIAL_RUBBER      3
#define MATERIAL_ASPHALT     4
#define MATERIAL_GRASS       5
#define MATERIAL_KERB        6
#define MATERIAL_BRAKE_LIGHT 7
#define MATERIAL_HEAD_LIGHT  8
#define MATERIAL_PACE_LIGHT  9
#define MATERIAL_SOLID_PAINT 10
#define MATERIAL_LIVERY      11
#define MATERIAL_ALCANTARA   12
#define MATERIAL_DECAL_TEXT  13  

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

    float4 lightDirection_UNUSED;
    float4 lightColor_UNUSED;
    float4 cameraPosition;
    float4 cameraDirection;

    float4 carPosition;
    float4 carForward;

    SharedMaterial material;

    float brakeAmount;
    float time;
    float2 padding;
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

float3 GetSkyReflection(float3 R)
{
    float skyFactor = saturate(R.y * 0.5f + 0.5f);
    return lerp(float3(0.04f, 0.04f, 0.06f), float3(0.65f, 0.78f, 1.0f), skyFactor);
}

float3 PaceCarLightColor()
{
    float flash = step(0.5f, frac(time * 4.0f));

    float3 blue = float3(0.05f, 0.25f, 2.5f);
    float3 amber = float3(2.5f, 0.9f, 0.05f);

    return lerp(blue, amber, flash);
}

float PaceBodyMask(float3 worldPos)
{
    float3 beaconPos = carPosition.xyz + float3(0.0f, 1.2f, 0.0f);

    float d = length(worldPos - beaconPos);

    float range = 1.0f - saturate(d / 1.0f);
    range *= range;

    return range;
}

float4 ShadeCarPaint(float4 texColor, float3 N, float3 L, float3 V, float3 H, float3 R, float3 worldPos)
{
    float ndotl = saturate(dot(N, L));
    float fresnel = pow(1.0f - saturate(dot(N, V)), 5.0f);

    float3 base = texColor.rgb;


    if (base.r > 0.85f && base.g > 0.85f && base.b > 0.85f)
        base *= material.diffuseColor;
    float3 diffuse = base * (ndotl * 0.75f + 0.25f);

    float clearCoat = pow(saturate(dot(N, H)), 384.0f) * 2.0f;
    float broadSpec = pow(saturate(dot(N, H)), 48.0f) * 0.25f;

    float3 sky = GetSkyReflection(R) * fresnel * 0.35f;
    float paceFresnel = pow(1.0f - saturate(dot(N, V)), 2.0f);

    float3 paceGlow =
        PaceCarLightColor() *
        paceFresnel *
        0.35f;

    float topBias = saturate(N.y * 0.5f + 0.5f);

    float paceMask = PaceBodyMask(worldPos);

    float rim = pow(1.0f - saturate(dot(N, V)), 24.0f);

    float3 color = diffuse + sky + material.specularColor * (clearCoat + broadSpec);

    color += PaceCarLightColor() * paceFresnel * topBias * rim * 0.2f;

    return float4(color, 1.0f);
}


float4 ShadeCarLivery(float4 texColor, float3 N, float3 L, float3 V, float3 H, float3 R)
{
    float ndotl = saturate(dot(N, L));
    float fresnel = pow(1.0f - saturate(dot(N, V)), 5.0f);

    // Livery texture is king. Do not tint it with Kd.
    float3 base = texColor.rgb;

    float3 diffuse = base * (ndotl * 0.75f + 0.25f);

    // Slightly softer than solid car paint so decals/logos don't blow out too much
    float clearCoat = pow(saturate(dot(N, H)), 384.0f) * 1.3f;
    float broadSpec = pow(saturate(dot(N, H)), 48.0f) * 0.18f;

    float3 sky = GetSkyReflection(R) * fresnel * 0.22f;

    float3 spec = material.specularColor * (clearCoat + broadSpec);

    float3 color = diffuse + sky + spec;

    return float4(color, 1.0f);
}


float4 ShadeAlcantara(float4 texColor, float3 N, float3 L)
{
    float ndotl = saturate(dot(N, L));

    float3 base = texColor.rgb;

    // keep it dark/fabric-like
    float light = ndotl * 0.35f + 0.18f;

    // optional: slightly desaturate
    float lum = dot(base, float3(0.299f, 0.587f, 0.114f));
    base = lerp(base, lum.xxx, 0.25f);

    return float4(base * light, 1.0f);
}

float4 ShadeGlass(float4 texColor, float3 N, float3 L, float3 V, float3 H, float3 R)
{
    float fresnel = pow(1.0f - saturate(dot(N, V)), 3.0f);
    float ndotv = saturate(abs(dot(N, V)));
    float alpha = 0.06f + (1.0f - ndotv) * 0.12f;

    float3 tint = float3(0.03f, 0.04f, 0.05f);
    float3 sky = 0; //GetSkyReflection(R) * (0.35f + fresnel * 0.9f);

    float spec = pow(saturate(dot(N, H)), 256.0f) * 1.5f;

    float3 color = tint + sky + spec.xxx;

  //  return float4(color, 0.00f);
    return float4(0,0,0,alpha);
}

float4 ShadeRubber(float4 texColor, float3 N, float3 L)
{
    float ndotl = saturate(dot(N, L));

    float3 base = texColor.rgb;

    // tiny rubber tint, not murder
    base = lerp(base, float3(0.02f, 0.02f, 0.02f), 0.20f);

    float light = ndotl * 0.55f + 0.35f;

    return float4(base * light, 1.0f);
}

float HeadlightMask(float3 worldPos)
{
    float3 toPixel = worldPos - carPosition.xyz;
    toPixel.y = 0.0f;

    float distance = length(toPixel);

    if (distance < 0.001f)
        return 0.0f;

    float3 dirToPixel = normalize(toPixel);

    float3 forward = normalize(carForward.xyz);
    forward.y = 0.0f;
    forward = normalize(forward);

    float forwardAmount = dot(dirToPixel, forward);

    float cone = smoothstep(0.82f, 0.995f, forwardAmount);
    float range = 1.0f - saturate(distance / 350.0f);
    range *= range;
    float vertical = 1.0f - saturate(abs(worldPos.y - carPosition.y) / 24.0f);
    vertical *= vertical;

    float nearFade = smoothstep(0.0f, 9.0f, distance);

    return cone * range * nearFade * vertical;
}

float BrakeLightMask(float3 worldPos)
{
    float3 toPixel = worldPos - carPosition.xyz;
    toPixel.y = 0.0f;

    float distance = length(toPixel);
    if (distance < 0.001f)
        return 0.0f;

    float3 dirToPixel = normalize(toPixel);

    float3 rear = -normalize(carForward.xyz);
    rear.y = 0.0f;
    rear = normalize(rear);

    float rearAmount = dot(dirToPixel, rear);

    float cone = smoothstep(0.86f, 0.97f, rearAmount);
    float range = 1.0f - saturate(distance / 10.0f);
    range *= range;

    return cone * range * saturate(brakeAmount);

}
float4 ShadeBrakeLight(float4 texColor)
{
    float3 base = texColor.rgb;
    float brake = saturate(brakeAmount);
    float3 running = base * 0.6f + float3(0.10f, 0.0f, 0.0f);
    float3 brakeTint = lerp(base, float3(1.0f, 0.05f, 0.02f), brake);
    float3 brakeGlow = float3(6.0f, 0.2f, 0.08f) * brake;
    float3 color = lerp(running, brakeTint + brakeGlow, brake);
    return float4(color, 0.5f);
}
float4 ShadePaceLight(float4 texColor)
{
    // Flash between blue and amber
    float flash = step(0.5f, frac(time * 4.0f));

    float3 blue = float3(0.1f, 0.6f, 18.0f);
    float3 amber = float3(18.0f, 7.0f, 0.2f);

    float3 color = lerp(blue, amber, flash);

    return float4(color, 1.0f);
}



float PaceLightMask(float3 worldPos)
{
    float3 toPixel = worldPos - carPosition.xyz;
    toPixel.y = 0.0f;

    float distance = length(toPixel);

    float range = 1.0f - saturate(distance / 1.0f);
    range *= range;

    return range;
}

float4 ShadeAsphalt(float4 texColor, float3 N, float3 L, float3 worldPos)
{
    float ndotl = saturate(dot(N, L));
    float brakeSpill = BrakeLightMask(worldPos);
    float3 base = texColor.rgb * (ndotl * 0.18f + 0.035f);

    float headlight = HeadlightMask(worldPos);

    float3 beamColor = float3(1.0f, 0.92f, 0.72f); 
    float3 finalColor = base + beamColor * headlight * 0.5f;
    finalColor += float3(1.0f, 0.05f, 0.02f) * brakeSpill * 0.5f;


    return float4(finalColor, 1.0f);
}


float4 ShadeHeadLight(float4 texColor)
{
    float3 base = texColor.rgb;

    // Dim DRL / parking light
    float3 running = base * 0.35f;

    // Bright white projector
    float3 beam = float3(25.0f, 24.0f, 20.0f);

    // Slightly blue HID tint
    beam *= float3(0.95f, 0.98f, 1.05f);

    float3 color = running + beam;

    return float4(color, 1.0f);
}


float4 ShadeDecalText(float4 texColor, float3 N, float3 L)
{
    clip(texColor.a - 0.5f);

    float ndotl = saturate(dot(N, L));
    float light = ndotl * 0.75f + 0.25f;

    return float4(texColor.rgb * light, 1.0f);
}



float4 PS(PS_INPUT input) : SV_Target{


    int matType = (int)material.materialType;


    float4 texColor = objTexture.Sample(samplerLinear, input.texCoord);



    if (matType == MATERIAL_DEFAULT)
    {
        clip(texColor.a - 0.001f);
    }
    

    // 1. Lighting vectors
    float3 N = normalize(input.normal);
    float3 L = normalize(-lightDirection); // Now uses static const
    float3 V = normalize(cameraPosition.xyz - input.worldPos);
    float3 H = normalize(L + V);
    float3 I = normalize(input.worldPos - cameraPosition.xyz);
    float3 R = reflect(I, N);

    if (matType == MATERIAL_BRAKE_LIGHT)
        return ShadeBrakeLight(texColor);

    if (matType == MATERIAL_HEAD_LIGHT)
        return ShadeHeadLight(texColor);

    if (matType == MATERIAL_PACE_LIGHT)
        return ShadePaceLight(texColor);
    
    if (matType == MATERIAL_SOLID_PAINT)
        return ShadeCarPaint(texColor, N, L, V, H, R, input.worldPos);

    if (matType == MATERIAL_GLASS)
        return ShadeGlass(texColor, N, L, V, H, R);

    if (matType == MATERIAL_RUBBER)
        return ShadeRubber(texColor, N, L);

    if (matType == MATERIAL_ASPHALT)
        return ShadeAsphalt(texColor, N, L, input.worldPos);

    if (matType == MATERIAL_LIVERY)
        return ShadeCarLivery(texColor, N, L, V, H, R);

    if (matType == MATERIAL_ALCANTARA)
        return ShadeAlcantara(texColor, N, L);    
    
    if (matType == MATERIAL_DECAL_TEXT)
        return ShadeDecalText(texColor, N, L);



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