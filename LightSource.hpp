#ifndef LIGHTSOURCE_HPP
#define LIGHTSOURCE_HPP

#include "Math.hpp"

struct PointLight
{
    PointLight();
    PointLight(Vector3 color, double attenuation, Vector3 position);

    Vector3 color;
    double attenuation;
    Vector3 position;
};

struct DirectionalLight
{
    DirectionalLight();
    DirectionalLight(Vector3 color, Vector3 direction);

    Vector3 color;
    Vector3 direction;
};

struct AmbientLight
{
    AmbientLight();
    AmbientLight(Vector3 color);

    Vector3 color;
};

enum class LightType
{
    POINT, DIRECTIONAL, AMBIENT
};

struct LightSource
{
    LightSource();
    LightSource(PointLight point);
    LightSource(DirectionalLight directional);
    LightSource(AmbientLight ambient);
    LightType type;
    union
    {
        PointLight point;
        DirectionalLight directional;
        AmbientLight ambient;
    };
};

#endif
