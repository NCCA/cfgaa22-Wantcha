#ifndef LIGHTS_H
#define LIGHTS_H

#include <ngl/Vec3.h>
#include "Transform.h"
#include "SceneObject.h"

enum class LightType { Directional, Point };

class Light : public SceneObject
{
public:
    Light() = default;
    Light(LightType type, const Transform& transform,
        const ngl::Vec3& color = {1, 1, 1}, float intensity = 1.0f);

    Light(LightType type, const ngl::Vec3& position, const ngl::Vec3& rotation,
        const ngl::Vec3& color = {1, 1, 1}, float intensity = 1.0f);

    float GetIntensity() const { return m_intensity; }
    ngl::Vec3 GetColor() const { return m_color; }
    ngl::Vec3 GetForward() const;
    LightType GetType() const { return m_lightType; }
    virtual QGridLayout* GetLayout() override;

    //virtual void Draw() const override {}
    //virtual void DrawHighlighted() const override {}
private:
    LightType m_lightType;
    ngl::Vec3 m_color = {1, 1, 1};
    float m_intensity = 1.0f;
};

/*class DirectionalLight
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
};*/

#endif