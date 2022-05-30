#ifndef ASSET_CACHE_H
#define ASSET_CACHE_H

/// Code adapted from my older game engine project
/// https://github.com/Wantcha/Chimera/blob/main/Chimera/src/Chimera/Assets/AssetCache.h

#include <unordered_map>
#include "Asset.h"
#include "ngl/Texture.h"
#include <string>
#include <iostream>
#include <memory>

class IAssetCache
{
public:
    //virtual AssetType GetCachedType() = 0;
    virtual void CollectGarbage() = 0;
};

template<typename T>
class AssetCache : public IAssetCache
{
public:
    /*AssetType GetCachedType() override
    {
        if(!std::is_same<T, ngl::Texture>())
        {
            return T::GetAssetType();
        }
        return AssetType::Texture2D;
        
    }*/

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

    std::string FindAsset(std::shared_ptr<T> asset) const
    {
        for (auto it = m_Cache.begin(); it != m_Cache.end(); ++it)
        {
            if (it->second == asset) return it->first;
        }
        //std::cout<<"CANNOT FIND TEXTURE\n";
        return "";
    }

    void CollectGarbage() override
    {
        for (auto iter = m_Cache.begin(); iter != m_Cache.end();)
        {
            //std::cout<<iter->second.use_count()<<"\n";
            if (iter->second.use_count() <= 1/* || iter->second == nullptr*/)
            {
                // in this case the only reference left is the cache reference
                // we can free the asset from the cache
                iter = m_Cache.erase(iter);
                //std::cout<<"DELETING RESOURCE!\n";
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