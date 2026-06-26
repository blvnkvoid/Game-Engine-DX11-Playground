#include "VehicleRegistry.h"
#include "../SharedVehicleTypes.h"
#include "../Cars/Porsche_911_Carrera_S/Porsche911.h"
#include "../Cars/Audi_R8_LMS/AudiR8.h"
#include "../Cars/Bugatti_Chiron/BugattiChiron.h"
#include "../Cars/Honda_Civic_EG6/Civic.h"
#include "../Cars/Toyota_Minolta/Minolta.h"
#include "../Cars/Honda_NSX_Concept-GT_GT-500_2016/Honda_NSX_Concept-GT_GT-500_2016.h"
#include "../Cars/Mazda_Furai/Mazda_Furai.h"
#include "../Cars/Mazda_MX5/Mazda_MX5.h"
#include "../Cars/Audi_R10/AudiR10.h"
#include "../Cars/Citroen_Xsara_KitCar/Xsara_KitCar.h"
#include "../Cars/Daihatsu_Copen/Copen.h"
#include "../Cars/Toyota_Supra_JGTC_2000/Toyota_Supra_JGTC_2000.h"
#include "../Cars/Honda_NSX_JGTC_2000/Honda_NSX_JGTC_2000.h"
#include "VehicleRegistryEntry.h"


static const std::vector<VehicleRegistryEntry> vehicleTable =
{
    { VehicleSelection::PORSCHE_911,    "Cars/Porsche_911_Carrera_S/", "911_Carrera_S" },
    { VehicleSelection::BUGATTI_CHIRON, "Cars/Bugatti_Chiron/",        "Bugatti" },
    { VehicleSelection::AUDI_R8,        "Cars/Audi_R8_LMS/",           "R8" },
    { VehicleSelection::CIVIC,          "Cars/Honda_Civic_EG6/",       "Civic" },
    { VehicleSelection::MINOLTA,        "Cars/Toyota_Minolta/",        "Minolta" },
    { VehicleSelection::GT500,        "Cars/Honda_NSX_Concept-GT_GT-500_2016/",        "NSX_GT500" },
    { VehicleSelection::FURAI,        "Cars/Mazda_Furai/",        "Furai" },
    { VehicleSelection::MX5,        "Cars/Mazda_MX5/",        "Miata" },
    { VehicleSelection::AUDI_R10,        "Cars/Audi_R10/",        "R10" },
    { VehicleSelection::XSARA,        "Cars/Citroen_Xsara_KitCar/",        "Xsara" },
    { VehicleSelection::COPEN,        "Cars/Daihatsu_Copen/",        "Copen" },
    { VehicleSelection::JGTCSUPRA2000,        "Cars/Toyota_Supra_JGTC_2000/",        "JGTCSUPRA2000" },
    { VehicleSelection::JGTCNSX2000,        "Cars/Honda_NSX_JGTC_2000/",        "JGTCNSX2000" }
};  

VehicleAsset& VehicleRegistry::GetVehicle(VehicleSelection selection)
{
    auto it = vehicles.find(selection);

    if (it == vehicles.end())
    {
        throw std::runtime_error("Vehicle not found in VehicleRegistry");
    }

    return it->second;
}

VehicleDefinition VehicleRegistry::CreateDefinition(VehicleSelection selection)
{
    switch (selection)
    {
    case VehicleSelection::PORSCHE_911:
        return Porsche911::CreateDefinition();

    case VehicleSelection::AUDI_R8:
        return R8::CreateDefinition();

    case VehicleSelection::BUGATTI_CHIRON:
        return Chiron::CreateDefinition();

    case VehicleSelection::CIVIC:
        return Civic::CreateDefinition();

    case VehicleSelection::MINOLTA:
        return Minolta::CreateDefinition();    
    
    case VehicleSelection::GT500:
        return NSX_GT500::CreateDefinition();    
    
    
    case VehicleSelection::FURAI:
        return Furai::CreateDefinition();

    case VehicleSelection::MX5:
        return Mx5::CreateDefinition(); 
    
    case VehicleSelection::AUDI_R10:
        return R10::CreateDefinition();

    case VehicleSelection::XSARA:
        return Xsara::CreateDefinition();    
    
    case VehicleSelection::COPEN:
        return Copen::CreateDefinition();   

    case VehicleSelection::JGTCSUPRA2000:
        return SUPRA_JGTC2000::CreateDefinition();

    case VehicleSelection::JGTCNSX2000:
        return NSX_JGTC2000::CreateDefinition();
    }

    return Civic::CreateDefinition();
}

CameraDefinition VehicleRegistry::CreateCameraDefinition(VehicleSelection selection)
{
    switch (selection)
    {
    case VehicleSelection::PORSCHE_911:
        return Porsche911::CreateCameraDefinition();

    case VehicleSelection::AUDI_R8:
        return R8::CreateCameraDefinition();

    case VehicleSelection::BUGATTI_CHIRON:
        return Chiron::CreateCameraDefinition();

    case VehicleSelection::CIVIC:
        return Civic::CreateCameraDefinition();

    case VehicleSelection::MINOLTA:
        return Minolta::CreateCameraDefinition();

    case VehicleSelection::GT500:
        return NSX_GT500::CreateCameraDefinition();


    case VehicleSelection::FURAI:
        return Furai::CreateCameraDefinition();

    case VehicleSelection::MX5:
        return Mx5::CreateCameraDefinition();

    case VehicleSelection::AUDI_R10:
        return R10::CreateCameraDefinition();

    case VehicleSelection::XSARA:
        return Xsara::CreateCameraDefinition();

    case VehicleSelection::COPEN:
        return Copen::CreateCameraDefinition();

    case VehicleSelection::JGTCSUPRA2000:
        return SUPRA_JGTC2000::CreateCameraDefinition();

    case VehicleSelection::JGTCNSX2000:
        return NSX_JGTC2000::CreateCameraDefinition();
    }

    return Civic::CreateCameraDefinition();
}

void VehicleRegistry::RegisterVehicle(
    VehicleSelection selection,
    const std::string& mtlPath,
    const std::string& objPath,
    const std::wstring& textureFolder,
    ID3D11Device* device,
    ID3D11DeviceContext* context,
    TextureManager* textureManager)
{
    VehicleAsset asset;

    asset.model = std::make_unique<Model>();
    asset.object = std::make_unique<GameObject>(0);

    asset.model->LoadOBJ(mtlPath, objPath, device);
    asset.model->LoadTexture(device, L"white.png");
    asset.model->ResolveMaterialTextures(
        textureManager,
        context,
        textureFolder);

    asset.object->SetModel(asset.model.get());

    vehicles[selection] = std::move(asset);
}

void VehicleRegistry::RegisterVehicle(
    VehicleSelection selection,
    const std::string& carFolder,
    const std::string& fileBaseName,
    ID3D11Device* device,
    ID3D11DeviceContext* context,
    TextureManager* textureManager)
{
    std::string mtlPath =
        carFolder + "material/" + fileBaseName + ".mtl";

    std::string objPath =
        carFolder + "model/" + fileBaseName + ".obj";

    std::wstring wideFolder(
        carFolder.begin(),
        carFolder.end());

    std::wstring textureFolder =
        wideFolder + L"textures/";

    RegisterVehicle(
        selection,
        mtlPath,
        objPath,
        textureFolder,
        device,
        context,
        textureManager);
}


void VehicleRegistry::RegisterAllVehicles(
    ID3D11Device* device,
    ID3D11DeviceContext* context,
    TextureManager* textureManager)
{
    for (const auto& vehicle : vehicleTable)
    {
        RegisterVehicle(
            vehicle.selection,
            vehicle.folder,
            vehicle.fileName,
            device,
            context,
            textureManager);
    }
}