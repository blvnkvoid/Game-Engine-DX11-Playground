#pragma once

#include <directxmath.h>
#include <vector>
#include <windows.h>
#include <string>
#include <wrl/client.h>
#include <d3d11.h>

struct SharedVertex {
    DirectX::XMFLOAT3 pos;       // 12 bytes
    DirectX::XMFLOAT4 color;     // 16 bytes
    DirectX::XMFLOAT2 texCoord;  // 8 bytes
    DirectX::XMFLOAT3 normal;    // 12 bytes
    DirectX::XMFLOAT4 tangent;
};
static_assert(sizeof(SharedVertex) % 16 == 0, "DANGER!");

    struct SharedMaterial {
        DirectX::XMFLOAT3 diffuseColor;     // 12 bytes
        float materialType;                     // 4 bytes (Alignment!)

        DirectX::XMFLOAT3 specularColor;    // 12 bytes
        float specularPower;                // 4 bytes (The 'Ns' from your .mtl)

        DirectX::XMFLOAT3 ambientColor;     // 12 bytes
        float d; // Change whatever was here to 'd' [cite: 2026-01-03]// 4 bytes (Final Alignment!)

        float hasDiffuseTexture = 0.0f;
        float hasDetailTexture = 0.0f;
        float hasDiffuseColor = 0.0f;   // NEW: explicit Kd existed in MTL
        float isMapMaterial = 0.0f;
    };

    static_assert(sizeof(SharedMaterial) % 16 == 0, "DANGER!");

    struct MaterialTextures
    {
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> diffuse;
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> normal;
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> detail;
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> normalDetail;
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> maps;
    };

struct MaterialData
{
    SharedMaterial gpuMaterial;
    std::string diffuseTextureName;
    std::string normalTextureName;
    std::string detailTextureName;
    std::string normalDetailTextureName;
    std::string mapsTextureName;


    float hasDiffuseTexture = 0.0f;
    MaterialTextures carTextures;
    MaterialTextures mapTextures;
};


struct SharedSceneData
{
    DirectX::XMMATRIX world;
    DirectX::XMMATRIX view;
    DirectX::XMMATRIX projection;
    
    DirectX::XMMATRIX lightViewProjection[3];
    DirectX::XMFLOAT4 lightDirection;
    DirectX::XMFLOAT4 lightColor;
    DirectX::XMFLOAT4 cameraPosition;
    DirectX::XMFLOAT4 cameraDirection;

    DirectX::XMFLOAT4 carPosition;
    DirectX::XMFLOAT4 carForward;

    SharedMaterial material;

    float brakeAmount;
    float time;
    DirectX::XMFLOAT2 padding;

    float ambientIntensity;
    float headlightIntensity;
    
    DirectX::XMFLOAT2 environmentPadding;
};

static_assert(sizeof(SharedSceneData) % 16 == 0, "DANGER!");

struct LampData
{
    DirectX::XMFLOAT3 lampPosition;
    float lampRadius;
};

static_assert(sizeof(LampData) % 16 == 0, "DANGER!");


struct LampInfo
{
    int lampCount;
    DirectX::XMFLOAT3 lampInfoPadding;

};

static_assert(sizeof(LampInfo) % 16 == 0, "DANGER!");



struct CameraDefinition
{
    float chaseHeight = 0.0f;
    float chaseDistance = 0.0f;
    float chasePitchDeg = 0.0f;

    float roofHeight = 0.0f;
    float roofDistance = 0.0f;
    float roofPitchDeg = 0.0f;


    float bumperHeight = 0.0f;
    float bumperDistance = 0.0f;
    float bumperPitchDeg = 0.0f;

    float cockpitHeight = 0.0f;
    float cockpitDistance = 0.0f;
    float cockpitPitchDeg = 0.0f;

    float cockpitOffsetY = 0.0f;
    float cockpitOffsetX = 0.0f;
    float cockpitOffsetZ = 0.0f;

};

enum class EngineState {
    MAIN_MENU,
    GAMEPLAY
};

enum class GameMode {
    None,
    Arcade,
    GranTurismo
};


enum class VehicleSelection {
    AUDI_R8,
    PORSCHE_911,
    BUGATTI_CHIRON,
    CIVIC,
    GT500,
    MINOLTA,
    FURAI,
    MX5,
    AUDI_R10,
    XSARA,
    COPEN,
    JGTCSUPRA2000,
    JGTCNSX2000,
    SLS_PACECAR
};

enum MaterialType
{
    MATERIAL_DEFAULT = 0,
    MATERIAL_CAR_PAINT,
    MATERIAL_GLASS,
    MATERIAL_RUBBER,
    MATERIAL_ASPHALT,
    MATERIAL_GRASS,
    MATERIAL_KERB,
    MATERIAL_BRAKE_LIGHT,
    MATERIAL_HEAD_LIGHT,
    MATERIAL_PACE_LIGHT,
    MATERIAL_SOLID_PAINT,
    MATERIAL_LIVERY,
    MATERIAL_ALCANTARA,
    MATERIAL_DECAL_TEXT,
    MATERIAL_TREE,
    MATERIAL_LAMP,
    MATERIAL_SAFETYCAR_PAINT,
};


enum class EngineUpgradeSelection
{
    Turbo,
    NATune,
    StockEngine
};

enum class WeightReductionSelection
{
    WeightReductionStage1,
    WeightReductionStage2,
    WeightReductionStage3,
    StockWeight
};

enum class TyresUpgradeSelection
{
    SportTyres,
    SemiSlicks,
    RacingTyres,
    StockTyres
};


    enum class TrackSelection
    {
        AutumnRing,
        ElCapitan,
        Spa,
        GrandValley,
        TrialMountain,
        HighSpeedRing,
        MidfieldRaceway,
        TestCourse,
        RouteX,
        Nordschleife,
        BeginnerCourse,
        Motorland,
        Tsukuba,
        LeMans,
        DeepForest,
        SSR5,
        Suzuka,
        SanAndreas,
        Bayview,
        Bathurst,
        CostaDiAmalfi
    };



    struct GameConfig {
        static TrackSelection activeTrack;
    };






