#include "Lights.h"
#include <ngl/Quaternion.h>
#include <iostream>

Light::Light(LightType type, const Transform& transform, const ngl::Vec3& color, float intensity)
    :m_color(color), m_intensity(intensity), m_lightType(type) 
{
    m_transform = transform;
}

Light::Light(LightType type, const ngl::Vec3& position, const ngl::Vec3& rotation, const ngl::Vec3& color, float intensity)
    :m_color(color), m_intensity(intensity), m_lightType(type) 
{
    m_transform.addPosition(position);
    m_transform.addRotation(rotation);
}

ngl::Vec3 Light::GetForward() const
{
    float pitch = m_transform.getRotation().m_x;
    float yaw = m_transform.getRotation().m_y;
    return ngl::Quaternion(ngl::Vec3(pitch, yaw, 0.0f)).toMat4().getForwardVector();
    //std::cout<<m_transform.getRotation().m_x<<" "<<m_transform.getRotation().m_y<<" "<<m_transform.getRotation().m_z<<"\n";
    //return m_transform.getMatrix().getForwardVector();
}

/*DirectionalLight::DirectionalLight(const ngl::Vec3& direction, const ngl::Vec3& color, float intensity)
    :m_color(color), m_direction(direction), m_intensity(intensity) 
{

}

ngl::Vec3 DirectionalLight::GetDirection() const
{
    return m_direction;
}*/

/////////////////////////////////////////////////

/*PointLight::PointLight(const ngl::Vec3& position, const ngl::Vec3& color, float intensity)
    :m_color(color), m_pos(position), m_intensity(intensity) 
{

}

ngl::Vec3 PointLight::GetPosition() const
{
    return m_pos;
}*/

