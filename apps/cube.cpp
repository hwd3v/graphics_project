#include <iostream>
#include <array>
#include <cmath>
#include <vector>
#include "framebuffer.h"
#include "raster.h"
#include "math3d.h"

constexpr size_t NUM_TRIS = 12;
constexpr size_t NUM_VERTS = 8;
constexpr bool CULLING = true;
constexpr float LINE_DEPTH_BIAS = 0.01f;    // 太小會 z-fighting，太大背面的線會從角落透出來

using Tri = std::array<int, 3>;

bool check_winding(const std::array<Vec3, NUM_VERTS>& verts, const std::array<Tri, NUM_TRIS>& tris) {
    bool ok = true;
    for(size_t i = 0; i < tris.size(); i++) {
        const Vec3& a = verts[tris[i][0]];
        const Vec3& b = verts[tris[i][1]];
        const Vec3& c = verts[tris[i][2]];

        Vec3 n = (b - a).cross(c - a);
        Vec3 centroid = (a + b + c) * (1.0f / 3.0f);
        float d = n.dot(centroid);
        std::cout <<"i = " << i << ", d = " << d << (d > 0 ? ", PASS" : ", FAIL") << "\n";
        if(d <= 0) {
            ok = false;
        }
    }
    return ok;
}

void draw_edge(Framebuffer& fb, const Vec3& p, const Vec3& q, Color c) {
    int x0 = static_cast<int> (std::floor(p.x));
    int y0 = static_cast<int> (std::floor(p.y));
    int x1 = static_cast<int> (std::floor(q.x));
    int y1 = static_cast<int> (std::floor(q.y));

    draw_line(fb, x0, y0, x1, y1, p.z - LINE_DEPTH_BIAS, q.z - LINE_DEPTH_BIAS, c);
}

Vec3 to_screen(const Vec4& clip, const Framebuffer& fb) {
    float x_ndc = clip.x / clip.w;
    float y_ndc = clip.y / clip.w;
    float z_ndc = clip.z / clip.w;

    float x_screen = (x_ndc + 1) * (static_cast<float> (fb.width) * 0.5f);
    float y_screen = (1 - y_ndc) * (static_cast<float> (fb.height) * 0.5f);
    float z_screen = z_ndc;

    return Vec3(x_screen, y_screen, z_screen);
}

Vec4 intersect_near(const Vec4& a, const Vec4& b) {
    float d_a = a.z + a.w;
    float d_b = b.z + b.w;

    float t = d_a / (d_a - d_b);    // 一個在外一個在內 (d_a - d_b) 就不會為0

    float x = a.x + (b.x - a.x) * t;
    float y = a.y + (b.y - a.y) * t;
    float z = a.z + (b.z - a.z) * t;
    float w = a.w + (b.w - a.w) * t;

    return Vec4(x, y, z, w);
}

bool inside(const Vec4& a) {
    return a.z + a.w >= 0;
}

std::vector<Vec4> clip_near(const Vec4& a, const Vec4& b, const Vec4& c) {
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

int main() {
    Framebuffer fb(800, 600);

    fb.clear(Color{40, 40, 40});

    Mat4 M = Mat4::identity();
    Mat4 V = Mat4::lookAt(Vec3(3, 2, 4), Vec3(0, 0, 0), Vec3(0, 1, 0));
    Mat4 P = Mat4::perspective(Mat4::radians(60.0f), static_cast<float> (fb.width) / static_cast<float> (fb.height), 1.0f, 10.0f);

    Mat4 mvp = P * V * M;

    std::array<Vec3, NUM_VERTS> verts = {{
        {-1, -1, -1}, {1, -1, -1},
        {-1, 1, -1}, {1, 1, -1},
        {-1, -1, 1}, {1, -1, 1},
        {-1, 1, 1}, {1, 1, 1}
    }};

    std::array<Tri, NUM_TRIS> tris = {{
        {0, 4, 6}, {0, 6, 2},    // -x
        {4, 1, 5}, {4, 0, 1},    // -y
        {0, 2, 3}, {0, 3, 1},    // -z
        {1, 3, 7}, {1, 7, 5},    // +x
        {6, 7, 3}, {6, 3, 2},    // +y
        {4, 5, 7}, {4, 7, 6}     // +z
    }};

    if(!check_winding(verts, tris)) {
        return 1;
    }

    std::array<Vec3, NUM_VERTS> screen;
    std::array<Vec4, NUM_VERTS> clip_coords;

    for(size_t i = 0; i < verts.size(); i++) {
        Vec4 clip = mvp * Vec4::point(verts[i]);

        clip_coords[i] = clip;

        screen[i] = to_screen(clip, fb);

        std::cout << "clip.w = " << clip.w << ", screen " << i << " = ";
        screen[i].print();
    }

    std::array<Color, 6> face_colors = {{
        {255, 255, 0}, {255, 255, 255}, {0, 255, 0},    // -x, -y, -z
        {0, 0, 255}, {0, 255, 255}, {255, 0, 0}    // x, y, z
    }};

    std::array<size_t, 6> face_order = {0, 1, 2, 3, 4, 5};
//    std::array<size_t, 6> face_order = {5, 2, 3, 0, 4, 1};

    for(size_t f : face_order) {
        for(size_t k = 0; k < 2; k++) {
            size_t t = 2 * f + k;
            const Vec4& ca = clip_coords[tris[t][0]];
            const Vec4& cb = clip_coords[tris[t][1]];
            const Vec4& cc = clip_coords[tris[t][2]];

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

                draw_triangle(fb, s[0], s[j], s[j + 1], face_colors[f]);
            }
        }
    }

    for(size_t i = 0; i < tris.size(); i++) {
        const Vec4& ca = clip_coords[tris[i][0]];
        const Vec4& cb = clip_coords[tris[i][1]];
        const Vec4& cc = clip_coords[tris[i][2]];

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

    fb.write_ppm("cube.ppm");
//    fb.write_depth_pgm("cube.pgm");

    return 0;
}
