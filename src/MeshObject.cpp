#include "MeshObject.h"

#include "Assets/AssetManager.h"
#include "ObjMesh.h"

MeshObject::MeshObject(const std::string& filepath)
{
    m_mesh = AssetManager::GetAsset<ObjMesh>(filepath);
}