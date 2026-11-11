#include <iostream>
#include <array>
#include <cmath>
#include "framebuffer.h"
#include "raster.h"
#include "math3d.h"

constexpr size_t NUM_TRIS = 12;
constexpr size_t NUM_VERTS = 8;
constexpr bool CULLING = true;

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

    draw_line(fb, x0, y0, x1, y1, c);
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

    for(size_t i = 0; i < verts.size(); i++) {
        Vec4 clip = mvp * Vec4::point(verts[i]);

        float x_ndc = clip.x / clip.w;
        float y_ndc = clip.y / clip.w;
        float z_ndc = clip.z / clip.w;

        float x_screen = (x_ndc + 1) * (static_cast<float> (fb.width) * 0.5f);
        float y_screen = (1 - y_ndc) * (static_cast<float> (fb.height) * 0.5f);
        float z_screen = z_ndc;

        screen[i] = {x_screen, y_screen, z_screen};

        std::cout << "clip.w = " << clip.w << ", screen " << i << " = ";
        screen[i].print();
    }

    std::array<Color, 6> face_colors = {{
        {255, 255, 0}, {255, 255, 255}, {0, 255, 0},    // -x, -y, -z
        {0, 0, 255}, {0, 255, 255}, {255, 0, 0}    // x, y, z
    }};

    std::array<bool, NUM_TRIS> back_facing;
    for(size_t i = 0; i < tris.size(); i++) {
        const Vec3& a = screen[tris[i][0]];
        const Vec3& b = screen[tris[i][1]];
        const Vec3& c = screen[tris[i][2]];

        back_facing[i] = is_back_facing(a, b, c);

        if(back_facing[i]) {
            std::cout << "tri" << i << " is back facing: " << edge_function(a, b, c) << "\n";
        }
        else{
            std::cout << "tri" << i << " is not back facing: " << edge_function(a, b, c) << "\n";
        }
    }

    std::array<size_t, 6> face_order = {0, 1, 2, 3, 4, 5};
//    std::array<size_t, 6> face_order = {5, 2, 3, 0, 4, 1};

    for(size_t f : face_order) {
        for(size_t k = 0; k < 2; k++) {
            size_t t = 2 * f + k;
            const Vec3& a = screen[tris[t][0]];
            const Vec3& b = screen[tris[t][1]];
            const Vec3& c = screen[tris[t][2]];

            if(CULLING && back_facing[t]) {
                continue;
            }

            draw_triangle(fb, a, b, c, face_colors[f]);
        }
    }
/*
    for(size_t i = 0; i < tris.size(); i++) {
        const Vec3& a = screen[tris[i][0]];
        const Vec3& b = screen[tris[i][1]];
        const Vec3& c = screen[tris[i][2]];

        if(CULLING && back_facing[i]) {
            continue;
        }

        draw_triangle(fb, a, b, c, face_colors[i / 2]);
    }
*/
    for(size_t i = 0; i < tris.size(); i++) {
        const Vec3& a = screen[tris[i][0]];
        const Vec3& b = screen[tris[i][1]];
        const Vec3& c = screen[tris[i][2]];

        if(back_facing[i]) {
            continue;
        }

        draw_edge(fb, a, b, Color{0, 0, 0});
        draw_edge(fb, b, c, Color{0, 0, 0});
        draw_edge(fb, c, a, Color{0, 0, 0});
    }

    fb.write_ppm("cube.ppm");
    fb.write_depth_pgm("cube.pgm");

    return 0;
}
