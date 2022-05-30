#ifndef ASSET_MANAGER_H
#define ASSET_MANAGER_H

/// Code adapted from my older game engine project
/// https://github.com/Wantcha/Chimera/blob/main/Chimera/src/Chimera/Assets/AssetManager.h

#include "AssetCache.h"

class AssetManager
{
public:
    template<typename T>
    static void RegisterCache(T cache);

    template<typename T>
    static std::shared_ptr<T> GetAsset(const std::string& id);

    template<typename T>
    static std::string GetPath(T asset);
    /*{
        AssetType type = T::GetAssetType();
        if (s_Caches.find(type) != s_Caches.end())
        {
            AssetCache<T>* cache = static_cast<AssetCache<T>*>(s_Caches[type].get());
            std::shared_ptr<T> asset = cache->GetAsset(id);

            if (asset) return asset;
            asset = LoadAsset<T>(id);
            cache->CacheAsset(id, asset);

            return asset;
        }
    }*/

    static void CollectGarbage();

private:

    template<typename T>
    static std::shared_ptr<T> LoadAsset(const std::string& path);

    static std::unordered_map<AssetType, std::unique_ptr<IAssetCache>> s_Caches;
};

#endif