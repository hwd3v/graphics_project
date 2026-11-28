#pragma once

#include <vector>
#include <cmath>
#include "framebuffer.h"
#include "raster.h"
#include "math3d.h"
#include "mesh.h"

constexpr bool CULLING = true;
constexpr float LINE_DEPTH_BIAS = 0.01f;    // 太小會 z-fighting，太大背面的線會從角落透出來

inline void draw_edge(Framebuffer& fb, const Vec3& p, const Vec3& q, Color c) {
    int x0 = static_cast<int> (std::floor(p.x));
    int y0 = static_cast<int> (std::floor(p.y));
    int x1 = static_cast<int> (std::floor(q.x));
    int y1 = static_cast<int> (std::floor(q.y));

    draw_line(fb, x0, y0, x1, y1, p.z - LINE_DEPTH_BIAS, q.z - LINE_DEPTH_BIAS, c);
}

inline Vec3 to_screen(const Vec4& clip, const Framebuffer& fb) {
    float x_ndc = clip.x / clip.w;
    float y_ndc = clip.y / clip.w;
    float z_ndc = clip.z / clip.w;

    float x_screen = (x_ndc + 1) * (static_cast<float> (fb.width) * 0.5f);
    float y_screen = (1 - y_ndc) * (static_cast<float> (fb.height) * 0.5f);
    float z_screen = z_ndc;

    return Vec3(x_screen, y_screen, z_screen);
}

inline Vec4 intersect_near(const Vec4& a, const Vec4& b) {
    float d_a = a.z + a.w;
    float d_b = b.z + b.w;

    float t = d_a / (d_a - d_b);    // 一個在外一個在內 (d_a - d_b) 就不會為0

    float x = a.x + (b.x - a.x) * t;
    float y = a.y + (b.y - a.y) * t;
    float z = a.z + (b.z - a.z) * t;
    float w = a.w + (b.w - a.w) * t;

    return Vec4(x, y, z, w);
}

inline bool inside(const Vec4& a) {
    return a.z + a.w >= 0;
}

inline std::vector<Vec4> clip_near(const Vec4& a, const Vec4& b, const Vec4& c) {
    std::vector<Vec4> result;
    Vec4 v[3] = {a, b, c};

    for(int i = 0; i < 3; i++) {
        bool in_cur = inside(v[i]);
        bool in_next = inside(v[(i + 1) % 3]);

        if(in_cur) {
            result.push_back(v[i]);
        }
        if(in_cur != in_next) {
            const Vec4& in_pt = in_cur ? v[i] : v[(i + 1) % 3];    // 同一條邊會被兩個三角形共用，兩邊算出來焦點需完全一樣，永遠從裡面的點出發
            const Vec4& out_pt = in_cur ? v[(i + 1) % 3] : v[i];
            Vec4 intersect_point = intersect_near(in_pt, out_pt);
            result.push_back(intersect_point);
        }
    }

    return result;
}

inline void draw_mesh(Framebuffer& fb, const Mesh& mesh, const Mat4& mvp) {
    std::vector<Vec4> clip_coords (mesh.vertices.size());

    for(size_t i = 0; i < mesh.vertices.size(); i++) {
        Vec4 clip = mvp * Vec4::point(mesh.vertices[i]);

        clip_coords[i] = clip;
    }

    for(size_t t = 0; t < mesh.triangles.size(); t++) {
        const Vec4& ca = clip_coords[mesh.triangles[t][0]];
        const Vec4& cb = clip_coords[mesh.triangles[t][1]];
        const Vec4& cc = clip_coords[mesh.triangles[t][2]];

        std::vector<Vec4> poly = clip_near(ca, cb, cc);
        if(poly.size() < 3) {
            continue;
        }

        std::vector<Vec3> s;
        for(const Vec4& p : poly) {
            s.push_back(to_screen(p, fb));
        }

        for(size_t j = 1; j + 1 < s.size(); j++) {
            if(CULLING && is_back_facing(s[0], s[j], s[j + 1])) {
                continue;
            }

            draw_triangle(fb, s[0], s[j], s[j + 1], mesh.colors[t]);
        }
    }

    for(size_t i = 0; i < mesh.triangles.size(); i++) {
        const Vec4& ca = clip_coords[mesh.triangles[i][0]];
        const Vec4& cb = clip_coords[mesh.triangles[i][1]];
        const Vec4& cc = clip_coords[mesh.triangles[i][2]];

        std::vector<Vec4> poly = clip_near(ca, cb, cc);
        if(poly.size() < 3) {
            continue;
        }

        std::vector<Vec3> s;
        for(const Vec4& p : poly) {
            s.push_back(to_screen(p, fb));
        }

        for(size_t j = 1; j + 1 < s.size(); j++) {
            if(CULLING && is_back_facing(s[0], s[j], s[j + 1])) {
                continue;
            }

            draw_edge(fb, s[0], s[j], Color{0, 0, 0});
            draw_edge(fb, s[j], s[j + 1], Color{0, 0, 0});
            draw_edge(fb, s[j + 1], s[0], Color{0, 0, 0});
        }
    }
}
