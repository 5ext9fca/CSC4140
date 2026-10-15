# CSC4140 Assignment 01: Vectors, Matrices, and Image Reconstruction

This project is the linear-algebra warm-up: basic vector and matrix operations, image
reconstruction with the singular value decomposition (SVD), and a rotation about an
arbitrary axis. It is written in C++17 and uses Eigen3 for linear algebra and OpenCV 4 for
image I/O.

## Requirements

- A C++17 compiler
- CMake 3.16 or later
- Eigen3
- OpenCV 4

## Build and Run

Run the supplied script from any working directory:

```bash
./compile_run.sh
```

The script configures and builds the `demo` executable in `build/`, then runs it from the
project root. The program prints its results to standard output and writes these images
into the project root:

| Output | Description |
|---|---|
| `reconstructed_1.png` | `lenna.png` reconstructed from the first 1 singular value |
| `reconstructed_10.png` | ... from the first 10 singular values |
| `reconstructed_50.png` | ... from the first 50 singular values |

`output.txt` captures a reference run of the program.

## Manual Build

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
./build/demo
```

`CMakePresets.json` provides an equivalent `cmake` preset (Ninja generator, output under
`out/`); `ninja-build` is provided by the dev container.

## What It Computes

- **2.1 Basic vector operations** — `v + w` and the dot product `v · w`.
- **2.2 Basic matrix operations** — `i + j`, `i * j`, and `i * v` for 4x4 integer matrices.
- **2.3 SVD decomposition** — thin SVD of the grayscale `lenna` image and reconstruction
  from the first 1, 10, and 50 singular values.
- **2.4 Transformation** — rotates a point about an arbitrary center by composing
  rotations around the X, Y, and Z axes.

## Files

- `main.cpp` — all four parts of the assignment.
- `lenna.png` — input image (grayscale).
- `output.txt`, `reconstructed_1.png`, `reconstructed_10.png`, `reconstructed_50.png` —
  reference results.
- `CMakeLists.txt`, `CMakePresets.json`, `compile_run.sh` — build configuration.
