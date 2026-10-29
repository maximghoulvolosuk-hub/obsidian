#pragma once
#include <cstdint>

struct Vector3 {
    float x, y, z;
    Vector3 operator+(const Vector3& o) const { return {x + o.x, y + o.y, z + o.z}; }
    Vector3 operator-(const Vector3& o) const { return {x - o.x, y - o.y, z - o.z}; }
};

struct Vector2 {
    float x, y;
};

struct Matrix4x4 {
    float m[16];
};

namespace Offsets {
    constexpr uintptr_t PlayerManager_TypeInfo = 0x04A8B120;
    constexpr uintptr_t StaticFields           = 0xB8;
    constexpr uintptr_t Instance               = 0x0;
    constexpr uintptr_t LocalPlayer            = 0x48;
    constexpr uintptr_t PlayerList             = 0x50;
    constexpr uintptr_t ListItems              = 0x10;
    constexpr uintptr_t ListSize               = 0x18;
    constexpr uintptr_t Team                   = 0x78;
    constexpr uintptr_t Health                 = 0x84;
    constexpr uintptr_t Position               = 0x110;
}
