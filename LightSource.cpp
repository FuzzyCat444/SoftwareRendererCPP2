#include "LightSource.hpp"


PointLight::PointLight()
{
}

PointLight::PointLight(Vector3 color, double attenuation, Vector3 position)
	: color{ color }, attenuation{ attenuation }, position{ position }
{
}

DirectionalLight::DirectionalLight()
{
}

DirectionalLight::DirectionalLight(Vector3 color, Vector3 direction)
	: color{ color }, direction{ direction }
{
}

AmbientLight::AmbientLight()
{
}

AmbientLight::AmbientLight(Vector3 color)
    : color{ color }
{
}

LightSource::LightSource()
    : type{ LightType::POINT }
{
}

LightSource::LightSource(PointLight point)
	: type{ LightType::POINT }, point{ point }
{
}

LightSource::LightSource(DirectionalLight directional)
	: type{ LightType::DIRECTIONAL }, directional{ directional }
{
}

LightSource::LightSource(AmbientLight ambient)
	: type{ LightType::AMBIENT }, ambient{ ambient }
{
}
