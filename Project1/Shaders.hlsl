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
#define MATERIAL_TREE        14
#define MATERIAL_LAMP        15
#define MATERIAL_SAFETYCAR_PAINT 16

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
};

struct PS_INPUT {
    float4 position : SV_POSITION;
    float2 texCoord : TEXCOORD;
    float3 normal   : TEXCOORD1;
    float3 worldPos : TEXCOORD2;
    float3 localPos : TEXCOORD3;
    float4 lightSpacePos : TEXCOORD4;
};

Texture2D objTexture : register(t0);
Texture2D shadowMap : register(t2);
SamplerState samplerLinear : register(s0);

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

float PaceBodyMask(float3 localPos)
{
    float roofY = smoothstep(0.8f, 1.15f, localPos.y);

    float centerX =
        1.0f - smoothstep(0.6f, 1.2f, abs(localPos.x));

    float roofZ =
        1.0f - smoothstep(0.5f, 1.8f, abs(localPos.z));

    return roofY * centerX * roofZ;
}

float4 ShadeCarPaint(
    float4 texColor,
    float3 N,
    float3 L,
    float3 V,
    float3 H,
    float3 R,
    float3 worldPos,
    float shadowFactor
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
        texColor.rgb;

    if (base.r > 0.85f &&
        base.g > 0.85f &&
        base.b > 0.85f)
    {
        base *= material.diffuseColor;
    }

    // Constant indirect/ambient component.
    float3 ambientDiffuse =
        base *
        0.25f;

    // Sun-controlled diffuse component.
    float3 sunDiffuse =
        base *
        ndotl *
        0.75f *
        shadowFactor;

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
        (clearCoat + broadSpec) *
        shadowFactor;

    float3 color =
        ambientDiffuse +
        sunDiffuse +
        sky +
        sunSpecular;

    return float4(
        color,
        1.0f
        );
}


float4 ShadeSafetyCarPaint(
    float4 texColor,
    float3 N,
    float3 L,
    float3 V,
    float3 H,
    float3 R,
    float3 localPos,
    float shadowFactor
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
        texColor.rgb;

    float3 ambientDiffuse =
        base *
        0.25f;

    float3 sunDiffuse =
        base *
        ndotl *
        0.75f *
        shadowFactor;

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
        (clearCoat + broadSpec) *
        shadowFactor;

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


float4 ShadeCarLivery(
    float4 texColor,
    float3 N,
    float3 L,
    float3 V,
    float3 H,
    float3 R,
    float3 localLighting,
    float shadowFactor
)
{
    float ndotl =
        saturate(dot(N, L));

    float fresnel =
        pow(
            1.0f - saturate(dot(N, V)),
            5.0f
        );

    // Livery texture remains authoritative.
    float3 base =
        texColor.rgb;

    float3 ambientDiffuse =
        base *
        0.25f;

    float3 sunDiffuse =
        base *
        ndotl *
        0.75f *
        shadowFactor;

    float clearCoat =
        pow(
            saturate(dot(N, H)),
            384.0f
        ) *
        1.3f;

    float broadSpec =
        pow(
            saturate(dot(N, H)),
            48.0f
        ) *
        0.18f;

    float3 sky =
        GetSkyReflection(R) *
        fresnel *
        0.22f;

    float3 sunSpecular =
        material.specularColor *
        lightColor.rgb *
        (clearCoat + broadSpec) *
        shadowFactor;

    float3 color =
        ambientDiffuse +
        sunDiffuse +
        sky +
        sunSpecular +
        localLighting;

    return float4(
        color,
        1.0f
        );
}

float4 ShadeAlcantara(
    float4 texColor,
    float3 N,
    float3 L,
    float shadowFactor
)
{
    float ndotl =
        saturate(dot(N, L));

    float3 base =
        texColor.rgb;

    float lum =
        dot(
            base,
            float3(
                0.299f,
                0.587f,
                0.114f
                )
        );

    base =
        lerp(
            base,
            lum.xxx,
            0.25f
        );

    float ambientLight =
        0.18f;

    float directLight =
        ndotl *
        0.35f *
        shadowFactor;

    float light =
        ambientLight +
        directLight;

    return float4(
        base * light,
        1.0f
        );
}

float4 ShadeGlass(float4 texColor, float3 N, float3 L, float3 V, float3 H, float3 R, float shadowFactor)
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

float4 ShadeRubber(
    float4 texColor,
    float3 N,
    float3 L,
    float shadowFactor
)
{
    float ndotl =
        saturate(dot(N, L));

    float3 base =
        texColor.rgb;

    base =
        lerp(
            base,
            float3(
                0.02f,
                0.02f,
                0.02f
                ),
            0.20f
        );

    float ambientLight =
        0.35f;

    float directLight =
        ndotl *
        0.55f *
        shadowFactor;

    float light =
        ambientLight +
        directLight;

    return float4(
        base * light,
        1.0f
        );
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

float LampLightMask(float3 worldPos)
{  
    float closest = 1e9f;
    for (uint i = 0; i < lampCount; ++i)
    {
        float2 roadPos = worldPos.xz;
        float2 lampPos = lampLights[i].position.xz;

        float d = distance(roadPos, lampPos);
        closest = min(closest, d);
    }

    return saturate(1.0f - closest / 45.0f);
}

float4 ShadeLampGlow(float4 texColor, float2 uv)
{
    // keep original texture
    float3 base = texColor.rgb;

    // fake glow mask from UV center
    float2 center = uv - float2(0.5f, 0.5f);
    float dist = length(center);

    float glow = saturate(1.0f - dist * 2.2f);
    glow = glow * glow;

    float3 glowColor = float3(1.0f, 0.82f, 0.45f);

    float3 finalColor = base + glowColor * glow * 0.8f;

    return float4(finalColor, texColor.a);
}

float4 ShadeAsphalt(
    float4 texColor,
    float3 N,
    float3 L,
    float3 worldPos,
    float ambient,
    float headlightIntensity,
    float3 H,
    float3 V,
    float shadowFactor
)
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
        0.55f *
        shadowFactor;

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
        beam *
        shadowFactor;

    return float4(
        finalColor,
        1.0f
        );
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


float4 ShadeDecalText(
    float4 texColor,
    float3 N,
    float3 L,
    float shadowFactor
)
{
    clip(texColor.a - 0.5f);

    float ndotl =
        saturate(dot(N, L));

    float ambientLight =
        0.25f;

    float directLight =
        ndotl *
        0.75f *
        shadowFactor;

    float light =
        ambientLight +
        directLight;

    return float4(
        texColor.rgb * light,
        1.0f
        );
}

float CalculateShadowFactor(
    float4 lightSpacePos,
    float3 normal,
    float3 lightDir
)
{
    if (lightSpacePos.w <= 0.0f)
        return 1.0f;

    float3 lightNDC =
        lightSpacePos.xyz /
        lightSpacePos.w;

    float2 shadowUV;
    shadowUV.x = lightNDC.x * 0.5f + 0.5f;
    shadowUV.y = -lightNDC.y * 0.5f + 0.5f;

    float currentDepth = lightNDC.z;

    if (shadowUV.x < 0.0f ||
        shadowUV.x > 1.0f ||
        shadowUV.y < 0.0f ||
        shadowUV.y > 1.0f ||
        currentDepth < 0.0f ||
        currentDepth > 1.0f)
    {
        return 1.0f;
    }

    float ndotl =
        saturate(
            dot(
                normalize(normal),
                normalize(lightDir)
            )
        );

    float bias =
        max(
            0.005f * (1.0f - ndotl),
            0.0005f
        );

    uint width;
    uint height;
    shadowMap.GetDimensions(width, height);

    float2 texelSize =
        1.0f / float2(width, height);

    float visibility = 0.0f;

    [unroll]
    for (int y = -1; y <= 1; ++y)
    {
        [unroll]
        for (int x = -1; x <= 1; ++x)
        {
            float sampledDepth =
                shadowMap.Sample(
                    samplerLinear,
                    shadowUV +
                    float2(x, y) *
                    texelSize
                ).r;

            visibility +=
                currentDepth - bias <= sampledDepth
                ? 1.0f
                : 0.0f;
        }
    }

    return visibility / 9.0f;
}

float4 PS(PS_INPUT input) : SV_Target
{
    int matType =
        (int)material.materialType;

    float4 texColor =
        objTexture.Sample(
            samplerLinear,
            input.texCoord
        );

    // ---------------------------------------------------------
    // Lighting vectors
    // ---------------------------------------------------------

    float3 N =
        normalize(input.normal);

    float3 L =
        normalize(-lightDirection);

    float3 V =
        normalize(
            cameraPosition.xyz -
            input.worldPos
        );

    float3 H =
        normalize(L + V);

    float3 I =
        normalize(
            input.worldPos -
            cameraPosition.xyz
        );

    float3 R =
        reflect(I, N);

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

    // ---------------------------------------------------------
    // Emissive materials
    // ---------------------------------------------------------

    if (matType == MATERIAL_BRAKE_LIGHT)
    {
        return ShadeBrakeLight(
            texColor
        );
    }

    if (matType == MATERIAL_HEAD_LIGHT)
    {
        return ShadeHeadLight(
            texColor
        );
    }

    if (matType == MATERIAL_PACE_LIGHT)
    {
        return ShadePaceLight(
            texColor
        );
    }

    if (matType == MATERIAL_LAMP)
    {
        return ShadeLampGlow(
            texColor,
            input.texCoord
        );
    }

    // ---------------------------------------------------------
    // Specialized sunlight-driven materials
    // ---------------------------------------------------------

    if (matType == MATERIAL_SOLID_PAINT)
    {
        return ShadeCarPaint(
            texColor,
            N,
            L,
            V,
            H,
            R,
            input.worldPos,
            shadowFactor
        );
    }

    if (matType == MATERIAL_GLASS)
    {
        return ShadeGlass(
            texColor,
            N,
            L,
            V,
            H,
            R,
            shadowFactor
        );
    }

    if (matType == MATERIAL_RUBBER)
    {
        return ShadeRubber(
            texColor,
            N,
            L,
            shadowFactor
        );
    }

    if (matType == MATERIAL_ASPHALT)
    {
        return ShadeAsphalt(
            texColor,
            N,
            L,
            input.worldPos,
            ambientIntensity,
            headlightIntensity,
            H,
            V,
            shadowFactor
        );
    }

    if (matType == MATERIAL_LIVERY)
    {
        return ShadeCarLivery(
            texColor,
            N,
            L,
            V,
            H,
            R,
            localLighting,
            shadowFactor
        );
    }

    if (matType == MATERIAL_ALCANTARA)
    {
        return ShadeAlcantara(
            texColor,
            N,
            L,
            shadowFactor
        );
    }

    if (matType == MATERIAL_DECAL_TEXT)
    {
        return ShadeDecalText(
            texColor,
            N,
            L,
            shadowFactor
        );
    }

    if (matType == MATERIAL_SAFETYCAR_PAINT)
    {
        return ShadeSafetyCarPaint(
            texColor,
            N,
            L,
            V,
            H,
            R,
            input.localPos,
            shadowFactor
        );
    }

    // ---------------------------------------------------------
    // Alpha-tested trees
    // ---------------------------------------------------------

      if (matType == MATERIAL_TREE)
    {
        clip(texColor.a - 0.01f);
    }

    // ---------------------------------------------------------
    // Generic fallback material
    // ---------------------------------------------------------

    float ndotl =
        saturate(
            dot(N, L)
        );

    float3 baseColor =
        texColor.rgb;

    if (length(material.diffuseColor) >= 0.01f)
    {
        baseColor *=
            material.diffuseColor;
    }

    // Ambient remains visible in shadow.
    float3 ambient =
        baseColor *
        ambientIntensity;

    // Direct sunlight is blocked by Hannah.
    float3 diffuse =
        baseColor *
        lightColor.rgb *
        ndotl *
        0.9f *
        shadowFactor;

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
            saturate(dot(N, V)),
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

    // Environment reflections remain visible in shadow.
    float3 finalMirror =
        mirrorColor *
        material.specularColor *
        fresnel;

    // ---------------------------------------------------------
    // Direct sunlight specular
    // ---------------------------------------------------------

    float specClearCoat =
        pow(
            saturate(dot(N, H)),
            512.0f
        ) *
        2.5f;

    float specFlakes =
        pow(
            saturate(dot(N, H)),
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
        ambient +
        diffuse +
        reflection +
        finalMirror +
        localLighting;

    return float4(
        finalColor,
        1.0f
    );
}




PS_INPUT mainVS(VS_INPUT input) { return VS(input); }
float4 mainPS(PS_INPUT input) : SV_Target{ return PS(input); }
PS_INPUT main(VS_INPUT input) { return VS(input); }