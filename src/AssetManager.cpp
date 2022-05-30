#include "Assets/AssetManager.h"
#include "ObjMesh.h"
#include <ngl/Texture.h>

std::unordered_map<AssetType, std::unique_ptr<IAssetCache>> AssetManager::s_Caches;

template<>
void AssetManager::RegisterCache(std::unique_ptr<AssetCache<ObjMesh>> cache)
{
    s_Caches[AssetType::Mesh] = std::move(cache);
}

template<>
void AssetManager::RegisterCache(std::unique_ptr<AssetCache<ngl::Texture>> cache)
{
    s_Caches[AssetType::Texture2D] = std::move(cache);
}

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
    std::shared_ptr<ObjMesh> mesh = std::make_shared<ObjMesh>(path);
    
    return mesh;
}

template<>
std::shared_ptr<ObjMesh> AssetManager::GetAsset(const std::string& id)
{
    AssetType type = AssetType::Mesh;
    if (s_Caches.find(type) != s_Caches.end())
    {
        AssetCache<ObjMesh>* cache = static_cast<AssetCache<ObjMesh>*>(s_Caches[type].get());
        std::shared_ptr<ObjMesh> asset = cache->GetAsset(id);

        if (asset)
        { std::cout<<"Loading existing asset!\n"; return asset;}
        asset = LoadAsset<ObjMesh>(id);
        cache->CacheAsset(id, asset);

        return asset;
    }
    else return nullptr;
}

template<>
std::string AssetManager::GetPath(std::shared_ptr<ObjMesh> asset)
{
    AssetType type = AssetType::Mesh;
    if (s_Caches.find(type) != s_Caches.end())
    {
        AssetCache<ObjMesh>* cache = static_cast<AssetCache<ObjMesh>*>(s_Caches[type].get());
        return cache->FindAsset(asset);
    }
    return "";
}

template<>
std::shared_ptr<ngl::Texture> AssetManager::LoadAsset(const std::string& path)
{
    std::shared_ptr<ngl::Texture> texture = std::make_shared<ngl::Texture>(path);
    
    return texture;
}

template<>
std::shared_ptr<ngl::Texture> AssetManager::GetAsset(const std::string& id)
{
    AssetType type = AssetType::Texture2D;
    if (s_Caches.find(type) != s_Caches.end())
    {
        AssetCache<ngl::Texture>* cache = static_cast<AssetCache<ngl::Texture>*>(s_Caches[type].get());
        std::shared_ptr<ngl::Texture> asset = cache->GetAsset(id);

        if (asset) { std::cout<<"Loading existing asset!\n"; return asset; }
        asset = LoadAsset<ngl::Texture>(id);
        cache->CacheAsset(id, asset);

        return asset;
    }
    else return nullptr;
}

template<>
std::string AssetManager::GetPath(std::shared_ptr<ngl::Texture> asset)
{
    AssetType type = AssetType::Texture2D;
    if (s_Caches.find(type) != s_Caches.end())
    {
        AssetCache<ngl::Texture>* cache = static_cast<AssetCache<ngl::Texture>*>(s_Caches[type].get());
        return cache->FindAsset(asset);
    }
    return "";
}