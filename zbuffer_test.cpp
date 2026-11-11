#include "framebuffer.h"
#include "raster.h"
#include "math3d.h"

int main() {
    Framebuffer fb(800, 600);

    fb.clear(Color{40, 40, 40});

    Vec3 a0 = {200, 100, 0};
    Vec3 a1 = {50, 500, -1};
    Vec3 a2 = {400, 500, 0.9f};
    Vec3 b0 = {300, 150, 0};
    Vec3 b1 = {100, 550, 0.9f};
    Vec3 b2 = {500, 550, -1};

    draw_triangle(fb, a0, a1, a2, {255, 0, 0});

    fb.clear_depth();

    draw_triangle(fb, b0, b1, b2, {0, 255, 0});
//    draw_triangle(fb, a0, a1, a2, {255, 0, 0});

    fb.write_ppm("zbuffer_test_painter.ppm");

    return 0;
}
