#pragma once
#include "Base.hh"
#include <stdint.h>

namespace Zion {
    class Util {
        public:
            static float RadiansToDegrees(float radians);
            static float GetDistance(Vector3 a, Vector3 b);
            static Vector3 GetAngles(Vector4 quaternion);
            static Vector4 ToQuaternion(Vector3 angles);
            static uint32_t RGBAToARGB(uint32_t rgba);
    };
};