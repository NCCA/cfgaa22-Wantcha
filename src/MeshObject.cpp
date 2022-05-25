#include "MeshObject.h"

#include "Assets/AssetManager.h"
#include "ObjMesh.h"
#include <QGLWidget>
#include <QObject>
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
    TextureWidget* albedoWidget = new TextureWidget(&m_mesh->GetMaterial().m_albedoID, 50, 50, m_mesh->GetMaterial().m_albedoTexture);
    layout->addWidget(albedoWidget, 1, 1, Qt::AlignLeft);
    //QObject::connect(albedoWidget, &TextureWidget::SelectNewTexture, OnChangeAlbedoTexture(const std::string&) );
    
    layout->addWidget( new QLabel("Roughness"), 2, 0 );
    TextureWidget* roughnessWidget = new TextureWidget(&m_mesh->GetMaterial().m_roughnessID, 50, 50, m_mesh->GetMaterial().m_roughnessTexture);
    layout->addWidget(roughnessWidget, 2, 1, Qt::AlignLeft);

    layout->addWidget( new QLabel("Normal"), 3, 0 );
    TextureWidget* normalWidget = new TextureWidget(&m_mesh->GetMaterial().m_normalID, 50, 50, m_mesh->GetMaterial().m_normalTexture);
    layout->addWidget(normalWidget, 3, 1, Qt::AlignLeft);

    layout->addWidget( new QLabel("Ambient Occlusion"), 4, 0 );
    TextureWidget* aoWidget = new TextureWidget(&m_mesh->GetMaterial().m_aoID, 50, 50, m_mesh->GetMaterial().m_aoTexture);
    layout->addWidget(aoWidget, 4, 1, Qt::AlignLeft);

    layout->addWidget( new QLabel("Metallic"), 5, 0 );
    TextureWidget* metallicWidget = new TextureWidget(&m_mesh->GetMaterial().m_metallicID, 50, 50, m_mesh->GetMaterial().m_metallicTexture);
    layout->addWidget(metallicWidget, 5, 1, Qt::AlignLeft);

    return layout;
}