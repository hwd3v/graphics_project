#include <stdlib.h>
#include <stdio.h>

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
    // 若 malloc 失敗，fb.pixels 為 NULL ，呼叫端需自行檢查
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

int main(void) {
    Framebuffer fb = create_framebuffer(200, 100);
    if(fb.pixels == NULL) {
        fprintf(stderr, "Failed to allocate Framebuffer\n");
        return 1;
    }
    // 先把整張圖填成深灰色底
    for(int y = 0; y < fb.height; y++) {
        for(int x = 0; x < fb.width; x++) {
            set_pixel(&fb, x, y, 40, 40, 40);
        }
    }

    // 左上角畫一個 50*50 的紅色方塊
    for(int y = 0; y < 50; y++) {
        for(int x = 0; x < 50; x++) {
            set_pixel(&fb, x, y, 255, 0, 0);
        }
    }

    // 畫一條白色對角線
    for(int i = 0; i < fb.height; i++) {
        set_pixel(&fb, i, i, 255, 255, 255);
    }

    write_ppm(&fb, "output.ppm");
    free(fb.pixels);
    return 0;
}
