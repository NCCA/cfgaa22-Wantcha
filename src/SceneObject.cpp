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
    return new QGridLayout();
}

/*void SceneObject::BuildLayout()
{
    m_layout = new QGridLayout();
}*/