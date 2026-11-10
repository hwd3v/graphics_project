#pragma once

#include <algorithm>
#include <cstdlib>
#include <utility>
#include "framebuffer.h"
#include "math3d.h"

inline void draw_line(Framebuffer& fb, int x0, int y0, int x1, int y1, Color c) {
    bool steep = false;
    if(std::abs(y1 - y0) > std::abs(x1 - x0)) {
        steep = true;

        std::swap(x0, y0);
        std::swap(x1, y1);
    }

    if(x0 > x1) {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }

    int dx = x1 - x0;
    int dy = y1 - y0;
    int step = (dy < 0 ? -1 : 1);
    if(dy < 0) {
        dy = -dy;
    }
    int error = 0;
    int y = y0;

    for(int x = x0; x <= x1; x++) {
        if(steep) {
            fb.set_pixel(y, x, c);
        }
        else {
            fb.set_pixel(x, y, c);
        }

        error += 2 * dy;
        if(error > dx) {
            y += step;
            error -= 2 * dx;
        }
    }
}

inline float edge_function(const Vec3& a, const Vec3& b, const Vec3& p) {
    return (b.x - a.x) * (p.y - a.y) - (b.y - a.y) * (p.x - a.x);
}

inline bool is_back_facing(const Vec3& a, const Vec3& b, const Vec3& c) {
    return (edge_function(a, b, c) >= 0);    // 螢幕座標 y 朝下，從外面看 CCW 的正面在這裡會是負值，所以 >= 0 是背面
}

inline void draw_triangle(Framebuffer& fb, const Vec3& v0, const Vec3& v1, const Vec3& v2, Color c) {
    int max_x = static_cast<int> (std::max({v0.x, v1.x, v2.x}));
    int max_y = static_cast<int> (std::max({v0.y, v1.y, v2.y}));
    int min_x = static_cast<int> (std::min({v0.x, v1.x, v2.x}));
    int min_y = static_cast<int> (std::min({v0.y, v1.y, v2.y}));

    if(min_x < 0) {
        min_x = 0;
    }
    if(min_y < 0) {
        min_y = 0;
    }
    if(max_x >= fb.width) {
        max_x = fb.width - 1;
    }
    if(max_y >= fb.height) {
        max_y = fb.height - 1;
    }

    for(int y = min_y; y <= max_y; y++) {
        for(int x = min_x; x <= max_x; x++) {
            Vec3 p = Vec3{static_cast<float> (x) + 0.5f, static_cast<float> (y) + 0.5f, 0.0f};

            float e0 = edge_function(v1, v2, p);
            float e1 = edge_function(v2, v0, p);
            float e2 = edge_function(v0, v1, p);

            if((e0 >= 0 && e1 >= 0 && e2 >= 0) || (e0 <= 0 && e1 <= 0&& e2 <= 0)) {
                fb.set_pixel(x, y, c);
            }
        }
    }
}
