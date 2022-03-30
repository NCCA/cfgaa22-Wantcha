#ifndef LIGHTS_H
#define LIGHTS_H

#include <ngl/Vec3.h>

class DirectionalLight
{
public:
    DirectionalLight() = default;
    DirectionalLight(const ngl::Vec3& direction, const ngl::Vec3& color = {1, 1, 1}, float intensity = 1.0f);

    ngl::Vec3 GetColor() const;
    ngl::Vec3 GetDirection() const;
    float GetIntensity() const;
private:
    ngl::Vec3 m_color = {1, 1, 1};
    ngl::Vec3 m_direction = {0, 1, 0};
    float m_intensity = 1.0f;
};

class PointLight
{
public:
    PointLight() = default;
    PointLight(const ngl::Vec3& position, const ngl::Vec3& color = {1, 1, 1}, float intensity = 1.0f);

    ngl::Vec3 GetColor() const;
    ngl::Vec3 GetPosition() const;
    float GetIntensity() const;
private:
    ngl::Vec3 m_color = {1, 1, 1};
    ngl::Vec3 m_pos = {0, 1, 0};
    float m_intensity = 1.0f;
};

#endif