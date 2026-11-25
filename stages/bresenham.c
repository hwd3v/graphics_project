#include <stdio.h>
#include <stdlib.h>

typedef struct{
    int width;
    int height;
    unsigned char *pixels;
} Framebuffer;

Framebuffer create_framebuffer(int width, int height) {
    Framebuffer fb;
    fb.width = width;
    fb.height = height;
    fb.pixels = malloc((size_t)width * height * 3);
    return fb;
}

void set_pixel(Framebuffer *fb, int x, int y, unsigned char r, unsigned char g, unsigned char b) {
    if(x < 0 || y < 0 || x > fb -> width - 1 || y > fb -> height - 1) {
        return;
    }
    int index = (y * (fb -> width) + x) * 3;
    fb -> pixels[index] = r;
    fb -> pixels[index + 1] = g;
    fb -> pixels[index + 2] = b;
}

void write_ppm(Framebuffer *fb, const char *filename) {
    FILE *fp = fopen(filename, "wb");
    if(fp == NULL) {
        return;
    }

    fprintf(fp, "P6\n%d %d\n255\n", fb -> width, fb -> height);

    fwrite(fb -> pixels, 1, (fb -> width) * (fb -> height) * 3, fp);

    fclose(fp);
}

void draw_line_naive(Framebuffer *fb, int x0, int y0, int x1, int y1,
                     unsigned char r, unsigned char g, unsigned char b) {
    float slope = (float)(y1 - y0)/(x1 - x0);

    for(int x = x0; x <= x1; x++) {
        float y = slope * (x - x0) + y0;
        set_pixel(fb, x, (int)(y + 0.5f), r, g, b);
    }
}

void draw_line_bresenham(Framebuffer *fb, int x0, int y0, int x1, int y1,
                         unsigned char r, unsigned char g, unsigned char b) {
    int steep = 0;

    // 判斷陡不陡：用絕對值比較 dy 和 dx
    if(abs(y1 - y0) > abs(x1 - x0)) {
        steep = 1;
        // 把 x0 跟 y0 對調、把 x1 跟 y1 對調
        int tmp;
        tmp = x0; x0 = y0; y0 = tmp;
        tmp = x1; x1 = y1; y1 = tmp;
    }

    if(x0 > x1) {
        int tmp;
        tmp = x0; x0 = x1; x1 = tmp;
        tmp = y0; y0 = y1; y1 = tmp;
    }

    int dx = x1 - x0;
    int dy = y1 - y0;
    int step = (dy < 0) ? -1 : 1;
    if(dy < 0) {
        dy = -dy;
    }
    int error = 0;
    int y = y0;

    for(int x = x0; x <= x1; x++) {
        if(steep) {
            set_pixel(fb, y, x, r, g, b);
        }
        else{
            set_pixel(fb, x, y, r, g, b);
        }

        error += 2 * dy;
        if(error > dx) {
            y += step;
            error -= 2 * dx;
        }
    }
}

int main(void) {
    Framebuffer fb = create_framebuffer(200, 100);
    if(fb.pixels == NULL) {
        return 1;
    }

    for(int y = 0; y < fb.height; y++) {
        for(int x = 0; x < fb.width; x++) {
             set_pixel(&fb, x, y, 40, 40, 40);
        }
    }

    draw_line_bresenham(&fb, 0, 0, 199, 49, 0, 255, 0);
    draw_line_bresenham(&fb, 199, 49, 0, 0, 255, 0, 0);
    draw_line_bresenham(&fb, 0, 0, 49, 99, 0, 0, 255);
    draw_line_bresenham(&fb, 0, 99, 199, 0, 255, 255, 255);
    draw_line_bresenham(&fb, 0, -1, 300, 300, 50, 50, 50);

    write_ppm(&fb, "draw_line_bresenham.ppm");
    free(fb.pixels);
    return 0;
}
