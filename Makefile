CXX = g++
CXXFLAGS = -std=c++17 -g -Wall -Wextra -Wpedantic -Wconversion -I include
.PHONY: all clean
all: build/cube
build/cube: apps/cube.cpp include/math3d.h include/framebuffer.h include/raster.h
	mkdir -p build
	$(CXX) $(CXXFLAGS) apps/cube.cpp -o build/cube
clean: 
	rm -rf build
