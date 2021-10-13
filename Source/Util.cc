#include "Zion/Util.hh"
#include <math.h>
using namespace Zion;

float Util::RadiansToDegrees(float radians) {
    return radians * 57.2958F;
}

float Util::GetDistance(Vector3 a, Vector3 b) {
    return sqrtf(
        (b.x - a.x) * (b.x - a.x) +
        (b.y - a.y) * (b.y - a.y) +
        (b.z - a.z) * (b.z - a.z));
}

Vector3 Util::GetAngles(Vector4 quaternion) {
    Vector3 angles;

    angles.x = atan2f(
        2 * (quaternion.w * quaternion.x + quaternion.y * quaternion.z),
        1 - 2 * (quaternion.x * quaternion.x + quaternion.y * quaternion.y));

    float sinp = 2 * (quaternion.w * quaternion.y - quaternion.z * quaternion.x);
    if (fabs(sinp) >= 1)
        angles.y = copysignf(M_PI / 2, sinp);
    else
        angles.y = asinf(sinp);

    angles.z = atan2f(
        2 * (quaternion.w * quaternion.z + quaternion.x * quaternion.y),
        1 - 2 * (quaternion.y * quaternion.y + quaternion.z * quaternion.z));

    return angles;
}

Vector4 Util::ToQuaternion(Vector3 angles) {
    float cy = cos(angles.z * 0.5);
    float sy = sin(angles.z * 0.5);
    float cp = cos(angles.y * 0.5);
    float sp = sin(angles.y * 0.5);
    float cr = cos(angles.x * 0.5);
    float sr = sin(angles.x * 0.5);

    Vector4 quaternion;
    quaternion.w = cr * cp * cy + sr * sp * sy;
    quaternion.x = sr * cp * cy - cr * sp * sy;
    quaternion.y = cr * sp * cy + sr * cp * sy;
    quaternion.z = cr * cp * sy - sr * sp * cy;

    return quaternion;
}

uint32_t Util::RGBAToARGB(uint32_t rgba) {
    uint32_t argb = 0;

    argb |= (rgba >> 24) & 0x000000FF;
    argb |= (rgba >> 8) & 0x0000FF00;
    argb |= (rgba << 8) & 0x00FF0000;
    argb |= (rgba << 24) & 0xFF000000;

    return argb;
}