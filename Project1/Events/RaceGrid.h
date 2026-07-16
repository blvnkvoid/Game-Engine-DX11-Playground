#pragma once

#include <vector>
#include <d3d11.h>
#include <DirectXMath.h>

#include "EventDefinition.h"
#include "../SharedTypes.h"
#include "../Cars/VehicleRegistry.h"
#include "../Graphics/MapLoader.h"
#include "bullet/btBulletDynamicsCommon.h"

class GraphicsEngine;
class Camera;
class GameObject;
class TextureManager;
class PhysicsEngine;

struct RaceGridCar
{
    VehicleSelection vehicle;
    GameObject* object = nullptr;
    DirectX::XMMATRIX world = DirectX::XMMatrixIdentity();
    bool isPlayer = false;
};

class RaceGrid
{
public:
    VehicleSelection Build(
        const EventDefinition& event,
        const std::vector<MapMarker>& markers,
        VehicleRegistry& vehicleRegistry,
        PhysicsEngine& physics,
        ID3D11Device* device,
        ID3D11DeviceContext* context,
        TextureManager* textureManager,
        VehicleSelection fallbackPlayer);

    VehicleSelection GetPlayerSelection(
        const EventDefinition& event,
        VehicleSelection fallbackPlayer);

    const btTransform& GetPlayerSpawn() const
    {
        return m_playerSpawn;
    }

    


    void Render(GraphicsEngine& engine, Camera& camera);

    void Clear();

private:
    DirectX::XMMATRIX GetGridTransform(int gridPosition);

private:
    std::vector<RaceGridCar> m_cars;
    btTransform m_playerSpawn;
};