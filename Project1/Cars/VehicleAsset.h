#pragma once
#include <memory>
#include "../Scene/GameObject.h"
#include "../Graphics/CarLoader.h"

struct VehicleAsset
{
    std::unique_ptr<CarLoader> model;
    std::unique_ptr<GameObject> object;
};