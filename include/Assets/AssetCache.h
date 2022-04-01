#ifndef ASSET_CACHE_H
#define ASSET_CACHE_H

/// Code adapted from my older game engine project
/// https://github.com/Wantcha/Chimera/blob/main/Chimera/src/Chimera/Assets/AssetCache.h

#include <unordered_map>
#include "Asset.h"
#include <string>
#include <memory>

class IAssetCache
{
public:
    virtual AssetType GetCachedType() = 0;
    virtual void CollectGarbage() = 0;
};

template<typename T>
class AssetCache : public IAssetCache
{
public:
    AssetType GetCachedType() override
    {
        return T::GetAssetType();
    }

    void CacheAsset(const std::string& id, std::shared_ptr<T> asset)
    {
        if (!IsAssetCached(id))
            m_Cache[id] = std::move(asset);
    }

    std::shared_ptr<T> GetAsset(const std::string& id) const
    {
        const auto& found = m_Cache.find(id);
        if (found != m_Cache.end())
        {
            return (*found).second;
        }
        return nullptr;
    }

    bool IsAssetCached(const std::string& id) const
    {
        const auto& found = m_Cache.find(id);
        return (found != m_Cache.end()/* && found->second*/);
    }

    void CollectGarbage() override
    {
        for (auto iter = m_Cache.begin(); iter != m_Cache.end();)
        {
            if (iter->second.use_count() <= 1/* || iter->second == nullptr*/)
            {
                // in this case the only reference left is the cache reference
                // we can free the asset from the cache
                iter = m_Cache.erase(iter);
            }
            else
            {
                ++iter;
            }
        }
    }

private:
    std::unordered_map<std::string, std::shared_ptr<T> > m_Cache;
};

#endif