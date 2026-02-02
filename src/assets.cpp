#include "assets.h"

#include "olcPixelGameEngine.h"
#include "olcPGEX_TransformedView.h"

Assets::Assets(const std::string &path) : basePath(path) {
}

Assets::~Assets() {
    for (auto& pair : decalCache) {
        delete pair.second;
    }
    for (auto& pair : spriteCache) {
        delete pair.second;
    }
}

olc::Sprite *Assets::getSprite(const std::string &name)
{
    if (spriteCache.find(name) != spriteCache.end()) {
        return spriteCache[name];
    }

    std::string fullPath = basePath + "/" + name;
    olc::Sprite* sprite = new olc::Sprite(fullPath);
    
    spriteCache[name] = sprite;
    return sprite;
}

olc::Decal *Assets::getDecal(const std::string &name)
{
    if (decalCache.find(name) != decalCache.end()) {
        return decalCache[name];
    }

    olc::Sprite* sprite = getSprite(name);
    olc::Decal* decal = new olc::Decal(sprite);

    decalCache[name] = decal;
    return decal;
}