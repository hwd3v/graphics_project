#pragma once

#include <vector>
#include <array>
#include "math3d.h"
#include "framebuffer.h"

using Tri = std::array<int, 3>;

struct Mesh {
    std::vector<Vec3> vertices;
    std::vector<Tri> triangles;
    std::vector<Color> colors;
};

