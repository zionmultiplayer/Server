#pragma once

#if defined(__GNUC__)
    #define PACK(def) def __attribute__((__packed__))
#elif defined(_MSC_VER)
    #define PACK(def) __pragma(pack(push, 1)) def __pragma(pack(pop))
#endif

namespace Zion {
    struct Vector2 {
        float x;
        float y;
    };
    
    struct Vector3 {
        float x;
        float y;
        float z;
    };

    struct Vector4 {
        float w;
        float x;
        float y;
        float z;
    };
};

#define ZION__MAX_NAME_LENGTH         24
#define ZION__MAX_VEHICLES            2000
#define ZION__MAX_PLAYERS             1000