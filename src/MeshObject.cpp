#include "MeshObject.h"

#include "Assets/AssetManager.h"
#include "ObjMesh.h"
#include <QGLWidget>
#include <QObject>
#include <QSlider>
#include "ColorPicker.h"
#include "TextureWidget.h"

//QGridLayout* MeshObject::s_layout = new QGridLayout();

MeshObject::MeshObject(const std::string& filepath) : SceneObject(filepath)
{

}

QGridLayout* MeshObject::GetLayout()
{
    QGridLayout* layout = new QGridLayout();
    layout->addWidget( new QLabel("MATERIAL PROPERTIES"), 0, 0 );

    layout->addWidget( new QLabel("Albedo"), 1, 0 );
    TextureWidget* albedoWidget = new TextureWidget(&m_material.m_albedoID, 50, 50, m_material.m_albedoTexture);
    layout->addWidget(albedoWidget, 1, 1, Qt::AlignLeft);

    layout->addWidget( new QLabel("Color"), 2, 0 );
    ColorPicker* picker = new ColorPicker( m_material.m_baseColor );
    
    QObject::connect(picker, qOverload<const ngl::Vec3&>(&ColorPicker::PickedColor),
       [this, picker](const ngl::Vec3& arg) { m_material.m_baseColor = arg; });

    layout->addWidget( picker, 2, 1, Qt::AlignLeft );
    
    layout->addWidget( new QLabel("Roughness"), 3, 0 );
    TextureWidget* roughnessWidget = new TextureWidget(&m_material.m_roughnessID, 50, 50, m_material.m_roughnessTexture);
    layout->addWidget(roughnessWidget, 3, 1, Qt::AlignLeft);

    QSlider* roughnessSlider = new QSlider(Qt::Orientation::Horizontal);
    roughnessSlider->setMinimum(0); roughnessSlider->setMaximum(100); roughnessSlider->setValue(m_material.m_roughness * 100);
    QObject::connect(roughnessSlider, qOverload<int>(&QSlider::valueChanged),
       [this](int arg) { m_material.m_roughness = float(arg) / 100.0f; });
    layout->addWidget(roughnessSlider, 3, 2, Qt::AlignLeft);

    layout->addWidget( new QLabel("Normal"), 4, 0 );
    TextureWidget* normalWidget = new TextureWidget(&m_material.m_normalID, 50, 50, m_material.m_normalTexture);
    layout->addWidget(normalWidget, 4, 1, Qt::AlignLeft);

    layout->addWidget( new QLabel("Ambient Occlusion"), 5, 0 );
    TextureWidget* aoWidget = new TextureWidget(&m_material.m_aoID, 50, 50, m_material.m_aoTexture);
    layout->addWidget(aoWidget, 5, 1, Qt::AlignLeft);

    layout->addWidget( new QLabel("Metallic"), 6, 0 );
    TextureWidget* metallicWidget = new TextureWidget(&m_material.m_metallicID, 50, 50, m_material.m_metallicTexture);
    layout->addWidget(metallicWidget, 6, 1, Qt::AlignLeft);
    QSlider* metallicSlider = new QSlider(Qt::Orientation::Horizontal);
    metallicSlider->setMinimum(0); metallicSlider->setMaximum(100); metallicSlider->setValue(m_material.m_metallic * 100);
    QObject::connect(metallicSlider, qOverload<int>(&QSlider::valueChanged),
       [this](int arg) { m_material.m_metallic= float(arg) / 100.0f; });
    layout->addWidget(metallicSlider, 6, 2, Qt::AlignLeft);

    return layout;
}