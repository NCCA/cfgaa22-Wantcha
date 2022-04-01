#ifndef ASSET_H
#define ASSET_H

/// Code adapted from my older game engine project
/// https://github.com/Wantcha/Chimera/blob/main/Chimera/src/Chimera/Assets/Asset.h

#define ASSET_CLASS(type) static AssetType GetAssetType() { return type; }

enum class AssetType : unsigned int
{
    Undefined,
    Texture2D,
    Mesh
};

#endif