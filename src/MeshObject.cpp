#include "MeshObject.h"

#include "Assets/AssetManager.h"
#include "ObjMesh.h"

//QGridLayout* MeshObject::s_layout = new QGridLayout();

MeshObject::MeshObject(const std::string& filepath)
{
    m_mesh = AssetManager::GetAsset<ObjMesh>(filepath);
}

/*
GetLayout()

s_layout = new QGridLayout();
//m_sceneObjects[1]->GetMesh()->GetMaterial().SetTexture(TextureType::ALBEDO, "textures/Cerberus/Cerberus_A.png");

s_layout->AddWidget( new QLabel("Albedo"), 0, 0 );

*/