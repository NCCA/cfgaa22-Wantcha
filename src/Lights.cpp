#include "Lights.h"

DirectionalLight::DirectionalLight(const ngl::Vec3& direction, const ngl::Vec3& color, float intensity)
    :m_color(color), m_direction(direction), m_intensity(intensity) 
{

}

ngl::Vec3 DirectionalLight::GetColor() const
{
    return m_color;
}

ngl::Vec3 DirectionalLight::GetDirection() const
{
    return m_direction;
}

float DirectionalLight::GetIntensity() const
{
    return m_intensity;
}

/////////////////////////////////////////////////

PointLight::PointLight(const ngl::Vec3& position, const ngl::Vec3& color, float intensity)
    :m_color(color), m_pos(position), m_intensity(intensity) 
{

}

ngl::Vec3 PointLight::GetColor() const
{
    return m_color;
}

ngl::Vec3 PointLight::GetPosition() const
{
    return m_pos;
}

float PointLight::GetIntensity() const
{
    return m_intensity;
}