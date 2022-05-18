#include "SceneObject.h"
#include "Assets/AssetManager.h"
#include "ObjMesh.h"

SceneObject::SceneObject(const std::string& filepath)
{
    m_mesh = AssetManager::GetAsset<ObjMesh>(filepath);
}

void SceneObject::Draw()
{
    m_mesh->Draw();

}

void SceneObject::DrawHighlighted()
{
    m_mesh->DrawWireframe();
}

QGridLayout* SceneObject::GetLayout()
{
    QGridLayout* layout = new QGridLayout();
    //m_sceneObjects[1]->GetMesh()->GetMaterial().SetTexture(TextureType::ALBEDO, "textures/Cerberus/Cerberus_A.png");

    //layout->addWidget( new QLabel("Albedo"), 0, 0 );
    //layout->addWidget( new QGraphicsView() );

    return layout;
}