#include "RaceGrid.h"

#include "../Graphics/GraphicsEngine.h"
#include "../Scene/Camera.h"
#include "../Scene/GameObject.h"
#include "../Cars/VehicleAsset.h"

using namespace DirectX;

VehicleSelection RaceGrid::GetPlayerSelection(
    const EventDefinition& event,
    VehicleSelection fallbackPlayer)
{
    for (const auto& carEntry : event.cars)
    {
        if (carEntry.isPlayer)
            return carEntry.vehicle;
    }

    return fallbackPlayer;
}


VehicleSelection RaceGrid::Build(
    const EventDefinition& event,
    const std::vector<GridMarker>& gridMarkers,
    VehicleRegistry& vehicleRegistry,
    ID3D11Device* device,
    ID3D11DeviceContext* context,
    TextureManager* textureManager,
    VehicleSelection fallbackPlayer)
{
    Clear();

    VehicleSelection playerSelection = fallbackPlayer;

    for (const auto& carEntry : event.cars)
    {
        VehicleAsset& vehicle =
            vehicleRegistry.GetOrLoadVehicle(
                carEntry.vehicle,
                device,
                context,
                textureManager);

        if (carEntry.isPlayer)
        {
            playerSelection = carEntry.vehicle;

            const GridMarker& marker =
                gridMarkers[carEntry.gridPosition];

            m_playerSpawn.setIdentity();
            m_playerSpawn.setOrigin(
                btVector3(marker.x, marker.y, marker.z));

            continue;
        }

        RaceGridCar gridCar;
        gridCar.vehicle = carEntry.vehicle;
        gridCar.object = vehicle.object.get();

        if (gridMarkers.empty())
        {
            OutputDebugStringA("ERROR: No grid markers found!\n");
            continue;
        }

        if (carEntry.gridPosition < 0 ||
            carEntry.gridPosition >= gridMarkers.size())
        {

            OutputDebugStringA(
                ("ERROR: gridPosition out of range: " +
                    std::to_string(carEntry.gridPosition) +
                    " marker count=" +
                    std::to_string(gridMarkers.size()) +
                    "\n").c_str());

            continue;
        }

        const GridMarker& marker = gridMarkers[carEntry.gridPosition];

        OutputDebugStringA(
            ("AI car marker: " + marker.name +
                " X=" + std::to_string(marker.x) +
                " Y=" + std::to_string(marker.y) +
                " Z=" + std::to_string(marker.z) +
                "\n").c_str());

        gridCar.world =
            XMMatrixRotationY(DirectX::XM_PIDIV2) * XMMatrixTranslation(marker.x, marker.y, marker.z);
        gridCar.isPlayer = false;

        m_cars.push_back(gridCar);

    }

    return playerSelection;
}

void RaceGrid::Render(GraphicsEngine& engine, Camera& camera)
{
    for (auto& car : m_cars)
    {
        if (!car.object)
            continue;

        car.object->SetWorldMatrix(car.world);
        engine.RenderObject(car.object, &camera);
    }
}

void RaceGrid::Clear()
{
    m_cars.clear();
}

XMMATRIX RaceGrid::GetGridTransform(int gridPosition)
{
    float spacingZ = 8.0f;
    float spacingX = 4.0f;

    int row = gridPosition / 2;
    int side = gridPosition % 2;

    float x = side == 0 ? -spacingX : spacingX;
    float z = -(row * spacingZ);

    return XMMatrixTranslation(
        -204.0f + x,
        15.0f,
        170.0f + z);
}