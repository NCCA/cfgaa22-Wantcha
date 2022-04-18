#include "Assets/AssetManager.h"
#include "ObjMesh.h"

std::unordered_map<AssetType, std::unique_ptr<IAssetCache>> AssetManager::s_Caches;

void AssetManager::RegisterCache(std::unique_ptr<IAssetCache> cache)
{
    s_Caches[cache->GetCachedType()] = std::move(cache);
}

/*void AssetManager::RegisterAsset(const std::string& id)
{

}*/

void AssetManager::CollectGarbage()
{
    for (auto& kv : s_Caches)
    {
        kv.second->CollectGarbage();
    }
}


template<>
std::shared_ptr<ObjMesh> AssetManager::LoadAsset(const std::string& path)
{
    //CM_CORE_ERROR(path);
    std::shared_ptr<ObjMesh> mesh = std::make_shared<ObjMesh>(path);
    
    return mesh;
}