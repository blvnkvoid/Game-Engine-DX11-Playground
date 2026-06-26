    #pragma once

    #include <unordered_map>
    #include <string>
    #include <stdexcept>
    #include <d3d11.h>

    #include "VehicleAsset.h"
    #include "../SharedTypes.h"
    #include "../SharedVehicleTypes.h"
    #include "../Graphics/TextureManager.h"

    struct VehicleSelectionHash
    {
        std::size_t operator()(VehicleSelection selection) const noexcept
        {
            return static_cast<std::size_t>(selection);
        }
    };

    class VehicleRegistry
    {
    public:
        VehicleAsset& GetVehicle(VehicleSelection selection);

        void RegisterAllVehicles(
            ID3D11Device* device,
            ID3D11DeviceContext* context,
            TextureManager* textureManager);

        void RegisterVehicle(
            VehicleSelection selection,
            const std::string& carFolder,
            const std::string& fileBaseName,
            ID3D11Device* device,
            ID3D11DeviceContext* context,
            TextureManager* textureManager);

        void RegisterVehicle(
            VehicleSelection selection,
            const std::string& mtlPath,
            const std::string& objPath,
            const std::wstring& textureFolder,
            ID3D11Device* device,
            ID3D11DeviceContext* context,
            TextureManager* textureManager);

        VehicleDefinition CreateDefinition(VehicleSelection selection);
        CameraDefinition CreateCameraDefinition(VehicleSelection selection);

    private:
        std::unordered_map<VehicleSelection, VehicleAsset, VehicleSelectionHash> vehicles;
    };