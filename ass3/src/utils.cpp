#include "utils.h"

#include <algorithm>
#include <cmath>
#include <tuple>

using namespace std;

namespace CGL {

  tuple<int, int, int, int> bounding_box(
    float x0, float y0,
    float x1, float y1,
    float x2, float y2,
    int width, int height
  ) {
    int minX = max(0.0f, floor(min(x0, min(x1, x2))));
    int maxX = min((float)(width - 1), floor(max(x0, max(x1, x2))));
    int minY = max(0.0f, floor(min(y0, min(y1, y2))));
    int maxY = min((float)(height - 1), floor(max(y0, max(y1, y2))));
    return { minX,maxX,minY,maxY };
  }

  float area_triangle(float x0, float y0, float x1, float y1, float x2, float y2) {
    return 0.5 * ((x1 - x0) * (y2 - y0) - (x2 - x0) * (y1 - y0));
  }

  tuple<float, float, float> barycentric_coordinates(
    float x, float y,
    float x0, float y0,
    float x1, float y1,
    float x2, float y2
  ) {
    float areaABC = area_triangle(x0, y0, x1, y1, x2, y2);
    float areaPAB = area_triangle(x, y, x0, y0, x1, y1);
    float areaPBC = area_triangle(x, y, x1, y1, x2, y2);
    float areaPCA = area_triangle(x, y, x2, y2, x0, y0);

    if (areaABC == 0) return { -1.0f,-1.0f,-1.0f };

    float alpha = areaPBC / areaABC;
    float beta = areaPCA / areaABC;
    float gamma = areaPAB / areaABC;

    return { alpha,beta,gamma };
  }

  bool point_in_triangle(float x, float y, float x0, float y0, float x1, float y1, float x2, float y2) {
    float alpha, beta, gamma;
    return point_in_triangle(x, y, x0, y0, x1, y1, x2, y2, alpha, beta, gamma);
  }

  bool point_in_triangle(float x, float y, float x0, float y0, float x1, float y1, float x2, float y2,
                         float& alpha, float& beta, float& gamma) {
    tie(alpha, beta, gamma) = barycentric_coordinates(x, y, x0, y0, x1, y1, x2, y2);
    return (alpha >= 0 && beta >= 0 && gamma >= 0);
  }

}
