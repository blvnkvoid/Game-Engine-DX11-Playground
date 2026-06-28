#pragma once

#include <directxmath.h>
#include <vector>
#include <windows.h>
#include <string>

struct SharedVertex {
    DirectX::XMFLOAT3 pos;       // 12 bytes
    DirectX::XMFLOAT4 color;     // 16 bytes
    DirectX::XMFLOAT2 texCoord;  // 8 bytes
    DirectX::XMFLOAT3 normal;    // 12 bytes (Crucial for the Bugatti's curves)
};
static_assert(sizeof(SharedVertex) % 16 == 0, "DANGER!");

    struct SharedMaterial {
        DirectX::XMFLOAT3 diffuseColor;     // 12 bytes
        float materialType;                     // 4 bytes (Alignment!)

        DirectX::XMFLOAT3 specularColor;    // 12 bytes
        float specularPower;                // 4 bytes (The 'Ns' from your .mtl)

        DirectX::XMFLOAT3 ambientColor;     // 12 bytes
        float d; // Change whatever was here to 'd' [cite: 2026-01-03]// 4 bytes (Final Alignment!)

    };

    static_assert(sizeof(SharedMaterial) % 16 == 0, "DANGER!");

struct MaterialData
{
    SharedMaterial gpuMaterial;
    std::string diffuseTextureName;
};

struct SharedSceneData
{
    DirectX::XMMATRIX world;
    DirectX::XMMATRIX view;
    DirectX::XMMATRIX projection;

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
};

static_assert(sizeof(SharedSceneData) % 16 == 0, "DANGER!");



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

};

enum class EngineState {
    MAIN_MENU,
    GAMEPLAY
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
    MATERIAL_DECAL_TEXT
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
        RoadAtlanta,
        RouteX,
        Nordschleife,
        BeginnerCourse,
        Motorland,
        Tsukuba,
        LeMans,
        DeepForest
    };

    struct GameConfig {
        static TrackSelection activeTrack;
    };






