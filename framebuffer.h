#pragma once
#include <vector>
#include <cstdint>
#include <cstdio>
#include <algorithm>
#include <limits>

struct Color {
    uint8_t r, g, b;
};

struct Framebuffer {
    static constexpr float DEPTH_CLEAR = 1.0f;    // 搭配深度測試 z < depth：far plane（z = 1）以外的東西都不畫
    static constexpr uint8_t BG_GRAY = 0;

    int width, height;

    std::vector<uint8_t> pixels;
    std::vector<float> depth;

    Framebuffer(int w, int h) : width(w), height(h), pixels(w * h * 3, 0), depth(w * h, DEPTH_CLEAR) {}

    void set_pixel(int x, int y, Color c) {
        if(x < 0 || y < 0 || x >= width || y >= height) {
            return;
        }

        int index = (y * width + x) * 3;
        pixels[index] = c.r;
        pixels[index + 1] = c.g;
        pixels[index + 2] = c.b;
    }

    void clear(Color c) {
        for(int y = 0; y < height; y++) {
            for(int x = 0; x < width; x++) {
                set_pixel(x, y, c);
            }
        }
    }

    void clear_depth() {
        for(size_t i = 0; i < depth.size(); i++) {
            depth[i] = DEPTH_CLEAR;
        }
    }

    bool depth_test(int x, int y, float z) {
        if(x < 0 || y < 0 || x >= width || y >= height) {
            return false;
        }

        int index = y * width + x;
        if(z < depth[index]) {
            depth[index] = z;
            return true;
        }
        return false;
    }

    void write_depth_pgm(const char* filename) const {
        std::vector<uint8_t> gray(width * height);

        float z_min = std::numeric_limits<float>::infinity();
        float z_max = -std::numeric_limits<float>::infinity();

        for(size_t i = 0; i < depth.size(); i++) {
            if(depth[i] == DEPTH_CLEAR) {
                continue;
            }
            if(depth[i] < z_min) {
                z_min = depth[i];
            }
            if(depth[i] > z_max) {
                z_max = depth[i];
            }
        }

        float depth_range = (z_max == z_min ? 1 : z_max - z_min);
        for(size_t j = 0; j < depth.size(); j++) {
            if(depth[j] == DEPTH_CLEAR) {
                gray[j] = BG_GRAY;
                continue;
            }
            if(z_max == z_min) {
                gray[j] = 255;
                continue;
            }
            float g = ((z_max - depth[j]) / depth_range) * 255;
            g = std::clamp(g, 0.0f, 255.0f);

            gray[j] = static_cast<uint8_t> (g + 0.5f);
        }

        FILE* fp = fopen(filename, "wb");
        if(fp == nullptr) {
            return;
        }

        fprintf(fp, "P5\n%d %d\n255\n", width, height);

        fwrite(gray.data(), 1, gray.size(), fp);

        fclose(fp);
    }

    void write_ppm(const char* filename) const {
        FILE* fp = fopen(filename, "wb");
        if(fp == nullptr) {
            return;
        }

        fprintf(fp, "P6\n%d %d\n255\n", width, height);

        fwrite(pixels.data(), 1, pixels.size(), fp);

        fclose(fp);
    }
};
