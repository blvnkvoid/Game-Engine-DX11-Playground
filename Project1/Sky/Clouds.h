#pragma once

#include <d3d11.h>

class SkyEngine;
class GraphicsEngine;
class Camera; 

class Clouds
{
public:

    Clouds() = default;
    ~Clouds() = default;

    void Render(
        ID3D11DeviceContext* context,
        SkyEngine& skyEngine,
        GraphicsEngine& graphicsEngine,
        Camera* camera);

private:

};