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

    draw_line_naive(&fb, 0, 0, 199, 49, 0, 255, 0);

    write_ppm(&fb, "draw_line_naive.ppm");
    free(fb.pixels);
    return 0;
}
