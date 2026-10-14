# CSC4140 Assignment 02: Transformation

This project renders a wireframe triangle after applying model, view, and perspective
transformations. It implements arbitrary-axis model rotation with Rodrigues' formula and
loads scene parameters from JSON configuration files.

## Requirements

- A C++17 compiler
- CMake 3.16 or later
- OpenCV 4
- Eigen3

CLI11 and nlohmann/json are provided as single-header libraries in `include/`, so no separate
installation is needed for them.

## Build and Generate the Results

Run the supplied script from any working directory:

```bash
./compile_run.bash
```

The script configures and builds the `ass2` executable in `build/`, then creates these images
in the project root:

| Output | Configuration | Rotation angle |
|---|---|---:|
| `result.png` | `config/config.json` | 0 degrees |
| `result1.png` | `config/config1.json` | 0 degrees |
| `result2.png` | `config/config2.json` | 0 degrees |
| `result3.png` | `config/config3.json` | 0 degrees |

These images are written into the project root by the script on each run.

## Manual Build

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
```

## Batch Rendering

The batch interface accepts an angle, output filename, and JSON configuration:

```bash
./build/ass2 --rotation 45 --output result.png --config config/config1.json
```

The original positional output format is also supported:

```bash
./build/ass2 -r 45 result.png config1.json
```

Use `./build/ass2 --help` for the complete CLI11-generated option reference.

## Interactive Mode

Start the windowed renderer with an optional initial rotation and configuration:

```bash
./build/ass2 --interactive --rotation 0 --config config/config.json
```

| Key | Action |
|---|---|
| `a` / `d` | Increase/decrease the rotation angle |
| `j` / `l` | Translate along the negative/positive x-axis |
| `w` / `s` | Translate along the positive/negative y-axis |
| `q` / `e` | Translate along the positive/negative z-axis |
| `+` / `-` | Uniformly scale up/down |
| `[` / `]` | Decrease/increase the vertical field of view |
| `r` | Restore the initial angle, translation, scale, and FOV |
| `p` | Save a numbered screenshot without overwriting an existing one |
| `h` | Print the control reference |
| Esc | Exit |

## Configuration Files

Each JSON file defines:

- Camera position: `eye_pos`
- Triangle data: `vertices`, `colors`, `tex_coords`, and `normals`
- Projection parameters: `eye_fov`, `aspect_ratio`, `zNear`, and `zFar`
- Model translation and scale: `T` and `S`
- Rotation axis: the line through `P0` in direction `P1 - P0`

The included presets isolate different transformation effects: the default scene provides a
baseline, `config1.json` applies scaling and translation, `config2.json` changes the field of
view, and `config3.json` demonstrates perspective with different vertex depths.

## Additional Work

- CLI11-based option parsing with help text, conversion errors, conflict detection, and
  backward-compatible positional arguments.
- External JSON scene presets that can be changed without recompiling.
- Interactive rotation, translation, scaling, FOV adjustment, reset, and collision-free
  screenshot numbering.
