#include "Lights.h"
#include "Assets/AssetManager.h"
#include "ObjMesh.h"
#include <QColorDialog>
#include "ColorPicker.h"
#include <ngl/Quaternion.h>
#include <ngl/Util.h>
#include <iostream>

Light::Light(LightType type, const Transform& transform, const ngl::Vec3& color, float intensity)
    :m_color(color), m_intensity(intensity), m_lightType(type)
{
    m_transform = transform;
    m_isLight = true;

    Initialize();
}

Light::Light(LightType type, const ngl::Vec3& position, const ngl::Vec3& rotation, const ngl::Vec3& color, float intensity)
    :m_color(color), m_intensity(intensity), m_lightType(type) 
{
    m_transform.addPosition(position);
    m_transform.addRotation(rotation);
    m_isLight = true;

    Initialize();
}

void Light::Initialize()
{
    if(m_lightType == LightType::Directional)
    {
        m_mesh = AssetManager::GetAsset<ObjMesh>("meshes/arrow.obj");
        m_name = "DirectionalLight";
    }
    else
    {
        m_mesh = AssetManager::GetAsset<ObjMesh>("meshes/sphere.obj");
        m_name = "PointLight";
    }
    float orthoSize = 10;
    m_projection = ngl::ortho(-orthoSize, orthoSize, -orthoSize, orthoSize, -10, 10);

	m_shadowBuffer = std::make_unique<FrameBuffer>(PBRShaderManager::s_directionalShadowMap, 0);
}

QGridLayout* Light::GetLayout()
{
    QGridLayout* layout = new QGridLayout();
    layout->addWidget( new QLabel("LIGHT PROPERTIES"), 0, 0 );
    layout->addWidget( new QLabel("Color"), 1, 0 );
    ColorPicker* picker = new ColorPicker(m_color);
    
    QObject::connect(picker, qOverload<const ngl::Vec3&>(&ColorPicker::PickedColor),
       [this](const ngl::Vec3& arg) { m_color = arg; PBRShaderManager::RefreshCurrentLights(); });

    layout->addWidget( picker, 1, 1, Qt::AlignLeft );

    return layout;
}

ngl::Vec3 Light::GetForward()
{
    //float pitch = m_transform.getRotation().m_x;
    //float yaw = m_transform.getRotation().m_y;
    //return ngl::Quaternion(ngl::Vec3(pitch, yaw, 0.0f)).toMat4().getForwardVector();
    return m_transform.getMatrix().getForwardVector();
    //std::cout<<m_transform.getRotation().m_x<<" "<<m_transform.getRotation().m_y<<" "<<m_transform.getRotation().m_z<<"\n";
    //return m_transform.getMatrix().getForwardVector();
}

ngl::Mat4 Light::GetView()
{
    // make sure upVector is different that light direction
    ngl::Vec3 forward = GetForward();
    std::cout<<forward.m_x<<" "<<forward.m_y<<" "<<forward.m_z<<"\n";
    ngl::Vec3 upVector = (0, 1, 0);
    if(std::abs( forward.m_y ) > 0.99f)
    {
        upVector = (0, 0, 1);
    }
    
    return ngl::lookAt( (0,0,0),  forward * 10, upVector);
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

