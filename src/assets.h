#pragma once

#include <string>
#include <map>

#include "olcPixelGameEngine.h"
#include "olcPGEX_TransformedView.h"

class Assets {
private:
    std::string basePath;
    std::map<std::string, olc::Decal*> decalCache;
    std::map<std::string, olc::Sprite*> spriteCache;
public:
    Assets(const std::string &path);
    Assets(const Assets&) = delete;
    Assets& operator=(const Assets&) = delete;
    ~Assets();

    olc::Sprite* getSprite(const std::string &name);
    olc::Decal* getDecal(const std::string &name);
};