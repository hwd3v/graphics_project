#include <stdio.h>
#include <stdlib.h>

#define MIN3(a, b, c) ((a) < (b) ? ((a) < (c) ? (a) : (c)) : ((b) < (c) ? (b) : (c)))
#define MAX3(a, b, c) ((a) > (b) ? ((a) > (c) ? (a) : (c)) : ((b) > (c) ? (b) : (c)))

typedef struct{
    int width;
    int height;
    unsigned char *pixels;
} Framebuffer;

typedef struct{
    int x, y;
    unsigned char r, g, b;
} Vertex;

Framebuffer create_framebuffer(int width, int height) {
    Framebuffer fb;
    fb.width = width;
    fb.height = height;
    fb.pixels = malloc((size_t)width * height * 3);
    return fb;
}

void set_pixel(Framebuffer *fb, int x, int y, unsigned char r, unsigned char g, unsigned char b) {
    if(x < 0 || y < 0 || x >= fb -> width || y >= fb -> height) {
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

    fwrite(fb -> pixels, 1, (size_t)(fb -> width) * (fb -> height) * 3, fp);

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

int edge(int ax, int ay, int bx, int by, int px, int py) {
    return (bx - ax) * (py - ay) - (by - ay) * (px - ax);
}

void draw_triangle(Framebuffer *fb, int x0, int y0, int x1, int y1, int x2, int y2,
                   unsigned char r, unsigned char g, unsigned char b) {
    int max_x = MAX3(x0, x1, x2);
    int max_y = MAX3(y0, y1, y2);
    int min_x = MIN3(x0, x1, x2);
    int min_y = MIN3(y0, y1, y2);

    if(min_x < 0) {
        min_x = 0;
    }
    if(min_y < 0) {
        min_y = 0;
    }
    if(max_x > fb -> width -1) {
        max_x = fb -> width - 1;
    }
    if(max_y > fb -> height - 1) {
        max_y = fb -> height - 1;
    }

    for(int y = min_y; y <= max_y; y++) {
        for(int x = min_x; x <= max_x; x++) {
            int e0 = edge(x1, y1, x2, y2, x, y);
            int e1 = edge(x2, y2, x0, y0, x, y);
            int e2 = edge(x0, y0, x1, y1, x, y);
            if((e0 >= 0 && e1 >= 0 && e2 >= 0) || (e0 <= 0 && e1 <= 0 && e2 <= 0)) {
                set_pixel(fb, x, y, r, g, b);
            }
        }
    }
}

void draw_triangle_shaded(Framebuffer *fb, Vertex v0, Vertex v1, Vertex v2) {
    int area = edge(v0.x, v0.y, v1.x, v1.y, v2.x, v2.y);
    if(area == 0) {
        return;
    }

    int max_x = MAX3(v0.x, v1.x, v2.x);
    int max_y = MAX3(v0.y, v1.y, v2.y);
    int min_x = MIN3(v0.x, v1.x, v2.x);
    int min_y = MIN3(v0.y, v1.y, v2.y);

    if(min_x < 0) {
        min_x = 0;
    }
    if(min_y < 0) {
        min_y = 0;
    }
    if(max_x > fb -> width - 1) {
        max_x = fb -> width - 1;
    }
    if(max_y > fb -> height - 1) {
        max_y = fb -> height - 1;
    }

    for(int y = min_y; y <= max_y; y++) {
        for(int x = min_x; x <= max_x; x++) {
            int e0 = edge(v1.x, v1.y, v2.x, v2.y, x, y);
            int e1 = edge(v2.x, v2.y, v0.x, v0.y, x, y);
            int e2 = edge(v0.x, v0.y, v1.x, v1.y, x, y);

            if((e0 >= 0 && e1 >= 0 && e2 >= 0) || (e0 <= 0 && e1 <= 0 && e2 <= 0)) {
                unsigned char cr = (e0 * v0.r + e1 * v1.r + e2 * v2.r) / area;
                unsigned char cg = (e0 * v0.g + e1 * v1.g + e2 * v2.g) / area;
                unsigned char cb = (e0 * v0.b + e1 * v1.b + e2 * v2.b) / area;
                set_pixel(fb, x, y, cr, cg, cb);
            }
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
    draw_triangle(&fb, 0, 0, 80, 80, 100, 40, 100, 100, 100);
    draw_triangle(&fb, 120, 20, 190, 30, 150, 90, 200, 80, 0);
    draw_triangle(&fb, -30, 60, 40, 110, 60, 50, 0, 120, 200);
    draw_triangle_shaded(&fb, (Vertex){10, 10, 255, 0, 0}, (Vertex){100, 20, 0, 255, 0}, (Vertex){40, 90, 0, 0, 255});

    write_ppm(&fb, "edgefunction.ppm");
    free(fb.pixels);
    return 0;
}
