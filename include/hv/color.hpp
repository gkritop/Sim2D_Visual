// Simple color utility functions
#pragma once
#include <cstdint>
#include <algorithm>
#include <array>

namespace hv {

// Gray
inline std::array<uint8_t,4> gray(float v) {
    v = std::clamp(v, 0.0f, 1.0f);
    uint8_t c = static_cast<uint8_t>(255.0f * v);
    return {c,c,c,255};
}

// Fire
inline std::array<uint8_t,4> fire(float v) {
    v = std::clamp(v, 0.0f, 1.0f);
    float r = std::min(1.5f*v, 1.0f);
    float g = std::min(std::max(1.5f*(v-0.3f), 0.0f), 1.0f);
    float b = std::min(std::max(1.5f*(v-0.6f), 0.0f), 1.0f);
    return { static_cast<uint8_t>(255*r),
             static_cast<uint8_t>(255*g),
             static_cast<uint8_t>(255*b),
             255 };
}

// Blue_Red
inline std::array<uint8_t,4> blue_red(float v) {
    v = std::clamp(v, 0.0f, 1.0f);
    return { static_cast<uint8_t>(255*v),
             0,
             static_cast<uint8_t>(255*(1.0f-v)),
             255 };
}

}
