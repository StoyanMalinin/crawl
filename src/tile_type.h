#pragma once
#include <cstdint>

enum class TileType : uint64_t {    
    Invalid = 0,
    Empty = 1,
    Ground = 2,
    FireBag = 3,
    TreeOrigin = 4,
    Tree = 5,
};