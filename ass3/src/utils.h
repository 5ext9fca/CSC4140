#pragma once

#include <tuple>

namespace CGL {

  // Axis-aligned screen-space bounding box of a triangle, clipped to
  // [0, width - 1] x [0, height - 1].
  // Returns { minX, maxX, minY, maxY }.
  std::tuple<int, int, int, int> bounding_box(
    float x0, float y0,
    float x1, float y1,
    float x2, float y2,
    int width, int height);

  // Signed area of the triangle.
  float area_triangle(float x0, float y0, float x1, float y1, float x2, float y2);

  // Barycentric coordinates (alpha, beta, gamma) of (x, y) w.r.t. the triangle.
  std::tuple<float, float, float> barycentric_coordinates(
    float x, float y,
    float x0, float y0,
    float x1, float y1,
    float x2, float y2);

  bool point_in_triangle(
    float x, float y,
    float x0, float y0,
    float x1, float y1,
    float x2, float y2);

  // Overload that also outputs the barycentric coordinates of the point.
  bool point_in_triangle(
    float x, float y,
    float x0, float y0,
    float x1, float y1,
    float x2, float y2,
    float& alpha, float& beta, float& gamma);

}
