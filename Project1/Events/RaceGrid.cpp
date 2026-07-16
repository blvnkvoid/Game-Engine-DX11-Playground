#include "RaceGrid.h"

#include "../Graphics/GraphicsEngine.h"
#include "../Scene/Camera.h"
#include "../Scene/GameObject.h"
#include "../Cars/VehicleAsset.h"
#include "../Physics/PhysicsEngine.h"

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
    const std::vector<MapMarker>& markers,
    VehicleRegistry& vehicleRegistry,
    PhysicsEngine& physics,
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


        const MapMarker& marker = markers[carEntry.gridPosition];

        btVector3 groundedPosition =
            physics.ProjectToGround(
                btVector3(marker.x, marker.y, marker.z),
                0.6f);

        if (carEntry.isPlayer)
        {
            playerSelection = carEntry.vehicle;

            const MapMarker& marker =
                markers[carEntry.gridPosition];

            m_playerSpawn.setIdentity();
            m_playerSpawn.setOrigin(groundedPosition);  
            m_playerSpawn.setRotation(
                btQuaternion(
                    marker.rotX,
                    marker.rotY,
                    marker.rotZ,
                    marker.rotW));


            continue;
        }

        RaceGridCar gridCar;
        gridCar.vehicle = carEntry.vehicle;
        gridCar.object = vehicle.object.get();
            
        if (markers.empty())
        {
            OutputDebugStringA("ERROR: No grid markers found!\n");
            continue;
        }

        if (carEntry.gridPosition < 0 ||
            carEntry.gridPosition >= markers.size())
        {

            OutputDebugStringA(
                ("ERROR: gridPosition out of range: " +
                    std::to_string(carEntry.gridPosition) +
                    " marker count=" +
                    std::to_string(markers.size()) +
                    "\n").c_str());

            continue;
        }


        XMMATRIX visualOffset =
            XMMatrixTranslation(0.0f, -0.6f, 0.0f);

        XMVECTOR rotationQuaternion =
            XMVectorSet(
                marker.rotX,
                marker.rotY,
                marker.rotZ,
                marker.rotW);

        OutputDebugStringA(
            ("AI car marker: " + marker.name +
                " X=" + std::to_string(marker.x) +
                " Y=" + std::to_string(marker.y) +
                " Z=" + std::to_string(marker.z) +
                "\n").c_str());

        gridCar.world =
           visualOffset * XMMatrixRotationQuaternion(rotationQuaternion) *
            XMMatrixTranslation(
                groundedPosition.x(),
                groundedPosition.y(),
                groundedPosition.z());

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