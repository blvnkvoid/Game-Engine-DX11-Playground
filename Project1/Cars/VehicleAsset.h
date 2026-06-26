#pragma once
#include <memory>
#include "../Scene/GameObject.h"
#include "../Graphics/Model.h"

struct VehicleAsset
{
    std::unique_ptr<Model> model;
    std::unique_ptr<GameObject> object;
};