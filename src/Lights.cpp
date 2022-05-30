#include "Lights.h"
#include "Assets/AssetManager.h"
#include "ObjMesh.h"
#include <QColorDialog>
#include <QDoubleSpinBox>
#include "ColorPicker.h"
#include <ngl/Quaternion.h>
#include <ngl/Util.h>
#include <ngl/Transformation.h>
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
        m_shadowBuffer = std::make_unique<FrameBuffer>(PBRShaderManager::s_directionalShadowMap, PBRShaderManager::s_curDirShadowIndex);
        ++PBRShaderManager::s_curDirShadowIndex;

        float orthoSize = 10;
        m_projection = ngl::ortho(-orthoSize, orthoSize, -orthoSize, orthoSize, -10, 10);
    }
    else
    {
        m_mesh = AssetManager::GetAsset<ObjMesh>("meshes/sphere.obj");
        m_name = "PointLight";
        m_shadowBuffer = std::make_unique<FrameBuffer>(PBRShaderManager::s_pointShadowMap, PBRShaderManager::s_curPointShadowIndex);
        ++PBRShaderManager::s_curPointShadowIndex;

        m_projection = ngl::perspective( 90.0f, 1.0f, 0.1f, 25.0f );
    }
	
}

void Light::RecalculateShadowBuffer(uint32_t texture3DIndex)
{
    if(m_lightType == LightType::Directional)
    {
        m_shadowBuffer.reset(new FrameBuffer(PBRShaderManager::s_directionalShadowMap, PBRShaderManager::s_curDirShadowIndex));
        ++PBRShaderManager::s_curDirShadowIndex;
    }
    else
    {
        m_shadowBuffer.reset(new FrameBuffer(PBRShaderManager::s_pointShadowMap, PBRShaderManager::s_curPointShadowIndex));
        ++PBRShaderManager::s_curPointShadowIndex;
    }
}

/*QGridLayout* Light::GetLayout()
{
    QGridLayout* layout = new QGridLayout();
    layout->addWidget( new QLabel("LIGHT PROPERTIES"), 0, 0 );
    layout->addWidget( new QLabel("Color"), 1, 0 );
    ColorPicker* picker = new ColorPicker(m_color);
    
    QObject::connect(picker, qOverload<const ngl::Vec3&>(&ColorPicker::PickedColor),
       [this, picker](const ngl::Vec3& arg) { m_color = arg; PBRShaderManager::RefreshCurrentLights(); picker->update(); });

    layout->addWidget( picker, 1, 1, Qt::AlignLeft );

    layout->addWidget( new QLabel("Intensity"), 2, 0 );

    QDoubleSpinBox* intensity = new QDoubleSpinBox();
    intensity->setMaximum(100); intensity->setMinimum(0);
    intensity->setValue(m_intensity);

    QObject::connect(intensity, qOverload<double>(&QDoubleSpinBox::valueChanged),
       [this](double arg) { m_intensity = arg; PBRShaderManager::RefreshCurrentLights(); });

    layout->addWidget( intensity, 2, 1, Qt::AlignLeft );
    return layout;
}*/

ngl::Vec3 Light::GetForward()
{
    return m_transform.getMatrix().getForwardVector();
}

ngl::Mat4 Light::GetView()
{
    //if(m_lightType == LightType::Directional)
    //{
        Transform initialTransform = m_transform;
        initialTransform.addRotation(initialTransform.getMatrix().getUpVector(), 180);
        ngl::Vec3 angles = initialTransform.getRotation();
        ngl::Mat4 trans, rotX, rotY, rotZ, viewMat;
        rotX.rotateX(angles.m_x);
        rotY.rotateY(angles.m_y);
        rotZ.rotateZ(angles.m_z);
        ngl::Vec3 pos = m_transform.getPosition();
        trans.translate(pos.m_x, pos.m_y, pos.m_z);
        viewMat = trans * rotZ * rotY * rotX;
        return viewMat.inverse();
    //}
    //else
    //{
    //    return ngl::Mat4();
    //}
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

