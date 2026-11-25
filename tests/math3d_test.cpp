#include "math3d.h"

constexpr int WIDTH = 800;
constexpr int HEIGHT = 600;

int main() {
    Mat4 view3 = Mat4::lookAt(Vec3(5, 0, 0), Vec3(0, 0, 0), Vec3(0, 1, 0));
    view3.print();
    (view3 * Vec4::point(Vec3(0, 0, 0))).print();
    (view3 * Vec4::point(Vec3(0, 0, 1))).print();

    Mat4 V = Mat4::lookAt(Vec3(0, 0, 5), Vec3(0, 0, 0), Vec3(0, 1, 0));
    Mat4 P = Mat4::perspective(Mat4::radians(90.0f), static_cast<float>(WIDTH) / HEIGHT, 1.0f, 10.0f);

    Vec4 clip = P * V * Vec4::point(Vec3(1, 1, 0));

    float x_ndc = clip.x / clip.w;
    float y_ndc = clip.y / clip.w;

    float x_pixel = (x_ndc + 1) * (WIDTH * 0.5f);
    float y_pixel = (1 - y_ndc) * (HEIGHT * 0.5f);
    printf("w = %.2f\n", clip.w);
    printf("pixel = (%.2f, %.2f)\n", x_pixel, y_pixel);
    return 0;
}

