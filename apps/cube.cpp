#include <iostream>
#include <array>
#include "framebuffer.h"
#include "math3d.h"
#include "mesh.h"
#include "pipeline.h"

bool check_winding(const Mesh& mesh) {    // 適用以原點為中心的凸形物體（三角形到原點的法向量）
    bool ok = true;
    for(size_t i = 0; i < mesh.triangles.size(); i++) {
        const Vec3& a = mesh.vertices[mesh.triangles[i][0]];
        const Vec3& b = mesh.vertices[mesh.triangles[i][1]];
        const Vec3& c = mesh.vertices[mesh.triangles[i][2]];

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

int main() {
    Framebuffer fb(800, 600);

    fb.clear(Color{40, 40, 40});

    Mat4 M = Mat4::identity();
    Mat4 V = Mat4::lookAt(Vec3(3, 2, 4), Vec3(0, 0, 0), Vec3(0, 1, 0));
    Mat4 P = Mat4::perspective(Mat4::radians(60.0f), static_cast<float> (fb.width) / static_cast<float> (fb.height), 1.0f, 10.0f);

    Mat4 mvp = P * V * M;

    Mesh cube;

    cube.vertices = {
        {-1, -1, -1}, {1, -1, -1},
        {-1, 1, -1}, {1, 1, -1},
        {-1, -1, 1}, {1, -1, 1},
        {-1, 1, 1}, {1, 1, 1}
    };

    cube.triangles = {
        {0, 4, 6}, {0, 6, 2},    // -x
        {4, 1, 5}, {4, 0, 1},    // -y
        {0, 2, 3}, {0, 3, 1},    // -z
        {1, 3, 7}, {1, 7, 5},    // +x
        {6, 7, 3}, {6, 3, 2},    // +y
        {4, 5, 7}, {4, 7, 6}     // +z
    };

    if(!check_winding(cube)) {
        return 1;
    }

    std::array<Color, 6> face_colors = {{
        {255, 255, 0}, {255, 255, 255}, {0, 255, 0},    // -x, -y, -z
        {0, 0, 255}, {0, 255, 255}, {255, 0, 0}    // x, y, z
    }};

    for(size_t t = 0; t < cube.triangles.size(); t++) {
        cube.colors.push_back(face_colors[t / 2]);
    }

    draw_mesh(fb, cube, mvp);
    fb.write_ppm("cube.ppm");
    fb.write_depth_pgm("cube.pgm");

    return 0;
}
