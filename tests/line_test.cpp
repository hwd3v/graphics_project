#include "raster.h"
#include "framebuffer.h"

int main() {
    Framebuffer fb(200, 100);
    fb.clear(Color{40, 40, 40});
    draw_line(fb, 0, 0, 199, 49, Color{0, 255, 0});
    draw_line(fb, 199, 49, 0, 0, Color{255, 0, 0});
    draw_line(fb, 0, 0, 49, 99, Color{0, 0, 255});
    draw_line(fb, 0, 99, 199, 0, Color{255, 255, 255});
    draw_line(fb, 0, -1, 300, 300, Color{50, 50, 50});

    fb.write_ppm("line_test.ppm");

    return 0;
}
