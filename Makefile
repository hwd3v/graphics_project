CXX = g++
CXXFLAGS = -std=c++17 -g -Wall -Wextra -Wpedantic -Wconversion -I include

.PHONY: all clean

all: build/cube build/zbuffer_test build/math3d_test build/line_test

build/cube: apps/cube.cpp include/math3d.h include/framebuffer.h include/raster.h include/mesh.h
	mkdir -p build
	$(CXX) $(CXXFLAGS) apps/cube.cpp -o build/cube

build/zbuffer_test: tests/zbuffer_test.cpp include/math3d.h include/framebuffer.h include/raster.h
	mkdir -p build
	$(CXX) $(CXXFLAGS) tests/zbuffer_test.cpp -o build/zbuffer_test

build/math3d_test: tests/math3d_test.cpp include/math3d.h
	mkdir -p build
	$(CXX) $(CXXFLAGS) tests/math3d_test.cpp -o build/math3d_test

build/line_test: tests/line_test.cpp include/raster.h include/framebuffer.h
	mkdir -p build
	$(CXX) $(CXXFLAGS) tests/line_test.cpp -o build/line_test

clean:
	rm -rf build
