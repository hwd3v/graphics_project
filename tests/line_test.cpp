#include "raster.h"
#include "framebuffer.h"

int main() {
    Framebuffer fb(200, 100);
    fb.clear(Color{40, 40, 40});

    fb.clear_depth();
    draw_line(fb, 0, 0, 199, 49, 0.0f, 0.0f, Color{0, 255, 0});
    fb.clear_depth();
    draw_line(fb, 199, 49, 0, 0, 0.0f, 0.0f, Color{255, 0, 0});
    fb.clear_depth();
    draw_line(fb, 0, 0, 49, 99, 0.0f, 0.0f, Color{0, 0, 255});
    fb.clear_depth();
    draw_line(fb, 0, 99, 199, 0, 0.0f, 0.0f, Color{255, 255, 255});
    fb.clear_depth();
    draw_line(fb, 0, -1, 300, 300, 0.0f, 0.0f, Color{50, 50, 50});

    fb.write_ppm("line_test.ppm");

    return 0;
}
