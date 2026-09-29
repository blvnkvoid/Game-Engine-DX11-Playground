#pragma once

#include <vector>
#include "GameObject.h"
#include "../Graphics/CarLoader.h"

class Scene {
public:
    std::vector<GameObject*> gameObjects;
    CarLoader* chaseTarget = nullptr; // The "Star" of the show

    void AddObject(GameObject* obj) { gameObjects.push_back(obj); }

    void SetChaseTarget(CarLoader* m) { chaseTarget = m; }
    CarLoader* GetChaseTarget() { return chaseTarget; }

    void Clear() {
        // We don't 'delete' the objects here because WinMain is handling that.
        // We just clear the pointers so the Scene becomes an empty container.
        gameObjects.clear();

        // Crucial: Reset the chase target so the camera doesn't follow a ghost
        chaseTarget = nullptr;

    }
};
