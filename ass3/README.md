# CSC4140 Assignment 03: SVG Rasterizer

A software rasterizer that renders SVG files, built on top of the course's CGL support
library. It implements point, line, and triangle rasterization (including interpolated
color and texture mapping), supersampling, 2D transforms, and subdivision curves, and it
ships an interactive OpenGL viewer for exploring the scenes.

## Requirements

- A C++17 compiler
- CMake 3.8 or later
- Intel TBB (backend for `std::execution::par`)
- OpenGL, FreeType

GLEW, GLFW, lodepng, and tinyexr are vendored under `CGL/deps/` and `src/`, so they need no
separate installation. All system dependencies are provided by the dev container.

## Build

```bash
cmake -S . -B build
cmake --build build
```

This produces the `draw` executable in `build/`.

## Run

Interactive viewer (requires a display, see the repository README for X11 setup):

```bash
./build/draw svg/basic/test1.svg
```

Headless rendering — rasterize a single SVG to `test.png` without opening a window:

```bash
./build/draw svg/basic/test1.svg nogl 1024 1024
```

The first argument may also be a directory; up to 9 SVG files from it are loaded for the
viewer.

## Tests

`tests/outputs/test_all.py` recursively renders every SVG under `svg/` at 1024x1024 and
binary-compares each result against the expected image in `tests/outputs/reference/`:

```bash
cd tests/outputs
python3 test_all.py
```

## Layout

| Path | Contents |
|---|---|
| `src/` | Rasterizer, SVG parser, draw-rendering backend, transforms, triangulation, textures |
| `include/` | GLEW and GLFW headers |
| `svg/` | Input scenes grouped as `basic`, `extra`, `hardcore`, `illustration`, `subdiv`, `texmap`, `transforms` |
| `img/` | Textures and images referenced by the scenes |
| `tests/` | Regression test script and reference outputs |
| `CGL/` | Vendored CGL support library and its dependencies |
