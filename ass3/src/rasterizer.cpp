#include "rasterizer.h"
#include <algorithm>
#include <execution>
#include <tuple>
#include <vector>

using namespace std;

namespace CGL {
  
  RasterizerImp::RasterizerImp(PixelSampleMethod psm, LevelSampleMethod lsm,
    size_t width, size_t height,
    unsigned int sample_rate) {
    this->psm = psm;
    this->lsm = lsm;
    this->width = width;
    this->height = height;
    this->sample_rate = sample_rate;

    sample_buffer.resize(width * height, Color::White);
    sample_buffer_depth.resize(width * height, numeric_limits<float>::infinity());
  }

  // Used by rasterize_point and rasterize_line
  void RasterizerImp::fill_pixel(size_t x, size_t y, Color c) {
    // Task 2: You might need to update this function to fix points and lines (such as the black rectangle border in test4.svg)
    // Task 2: Current covarage implementation dont change the buffer
    // NOTE: You are not required to implement proper supersampling for points and lines
    // It is sufficient to use the same color for all supersamples of a pixel for points and lines (not triangles)


    sample_buffer[y * width + x] = c;
    sample_buffer_depth[y * width + x] = 0.0;
  }

  // Rasterize a point: simple example to help you start familiarizing
  // yourself with the starter code.
  //
  void RasterizerImp::rasterize_point(float x, float y, Color color) {
    // fill in the nearest pixel
    int sx = (int)floor(x);
    int sy = (int)floor(y);

    // check bounds
    if (sx < 0 || sx >= width) return;
    if (sy < 0 || sy >= height) return;

    fill_pixel(sx, sy, color);
    return;
  }

  // Rasterize a line.
  void RasterizerImp::rasterize_line(float x0, float y0,
    float x1, float y1,
    Color color) {
    if (x0 > x1) {
      swap(x0, x1); swap(y0, y1);
    }

    float pt[] = { x0,y0 };
    float m = (y1 - y0) / (x1 - x0);
    float dpt[] = { 1,m };
    int steep = abs(m) > 1;
    if (steep) {
      dpt[0] = x1 == x0 ? 0 : 1 / abs(m);
      dpt[1] = x1 == x0 ? (y1 - y0) / abs(y1 - y0) : m / abs(m);
    }

    while (floor(pt[0]) <= floor(x1) && abs(pt[1] - y0) <= abs(y1 - y0)) {
      rasterize_point(pt[0], pt[1], color);
      pt[0] += dpt[0]; pt[1] += dpt[1];
    }
  }

  // Shared supersampled triangle rasterization loop.
  // shade(sx, sy, alpha, beta, gamma) returns the color of one subsample.
  template <typename ShadeFn>
  void RasterizerImp::rasterize_triangle_shaded(
    float x0, float y0, float z0,
    float x1, float y1, float z1,
    float x2, float y2, float z2,
    ShadeFn shade)
  {
    auto [minX, maxX, minY, maxY] = bounding_box(x0, y0, x1, y1, x2, y2,
                                                 (int)width, (int)height);
    if (sample_rate == 0) return;

    unsigned int n = sqrt((double)sample_rate);
    float inverse_sample_rate = 1.0f / (float)sample_rate;

    for (int y = minY; y <= maxY; ++y) {
      for (int x = minX; x <= maxX; ++x) {

        float Asum = 0.0f;
        Color accum(0, 0, 0);
        for (unsigned int i = 0; i < n; ++i) {
          for (unsigned int j = 0; j < n; ++j) {
            float sx = x + (i + 0.5f) / n;
            float sy = y + (j + 0.5f) / n;

            float alpha, beta, gamma;
            if (point_in_triangle(sx, sy, x0, y0, x1, y1, x2, y2,
                                  alpha, beta, gamma)) {
              float sz = alpha * z0 + beta * z1 + gamma * z2;
              if (sz < sample_buffer_depth[y * width + x] || fabs(sample_buffer_depth[y * width + x] - sz) < 1e-6) {
                sample_buffer_depth[y * width + x] = sz;
              } else {
                continue;
              }
              Color c = shade(sx, sy, alpha, beta, gamma);
              float ca = clamp(c.a, 0.0f, 1.0f);
              if (fabs(ca - 1.0f) < 1e-6) { ca = 1.0f; }
              accum += Color(c.r * ca, c.g * ca, c.b * ca);
              Asum += ca;
            }
          }
        }

        if (Asum == 0) continue;

        Color& dst = sample_buffer[y * width + x];
        dst = accum * inverse_sample_rate + dst * (1.0f - Asum * inverse_sample_rate);
        dst.a = 1.0f;
      }
    }
  }

  // Rasterize a triangle.
  void RasterizerImp::rasterize_triangle(
    float x0, float y0, float z0,
    float x1, float y1, float z1,
    float x2, float y2, float z2,
    Color color) {
      // Task 1: Implement basic triangle rasterization here, no supersampling
      // std::vector<int> rows(maxY - minY + 1);
      // for (int i = 0; i < rows.size(); i++) {
      //   rows[i] = minY + i;
      // }
    
      // std::for_each(std::execution::par, rows.begin(), rows.end(),
      //         [&](int y) {
      //             for (int x = minX; x <= maxX; ++x) {
      //                 if (point_in_triangle(
      //                         x + 0.5, y + 0.5,
      //                         x0, y0, x1, y1, x2, y2)) {
      //                     fill_pixel(x, y, color);
      //                 }
      //             }
      //         });
      // return;

    // Task 2: Constant-color supersampled rasterization (coverage compositing)
    rasterize_triangle_shaded(x0, y0, z0, x1, y1, z1, x2, y2, z2,
      [&](float, float, float, float, float) { return color; });
  }



  void RasterizerImp::rasterize_interpolated_color_triangle(
    float x0, float y0, float z0, Color c0,
    float x1, float y1, float z1, Color c1,
    float x2, float y2, float z2, Color c2)
  {
    // Task 4: Supersampled rasterization with barycentric color interpolation.
    // Shares the rasterization loop with rasterize_triangle via a shading callback.
    rasterize_triangle_shaded(x0, y0, z0, x1, y1, z1, x2, y2, z2,
      [&](float, float, float alpha, float beta, float gamma) {
        return c0 * alpha + c1 * beta + c2 * gamma;
      });
  }



  void RasterizerImp::rasterize_textured_triangle(
    float x0, float y0, float z0, float u0, float v0,
    float x1, float y1, float z1, float u1, float v1,
    float x2, float y2, float z2, float u2, float v2,
    Texture& tex)
  {
    // Task 5: Fill in the SampleParams struct and pass it to the tex.sample function.
    // switch (psm) {
    //   case P_NEAREST:
    //     rasterize_triangle_shaded(x0, y0, x1, y1, x2, y2,
    //       [&](float sx, float sy, float alpha, float beta, float gamma
    //       ) {
    //         Vector2D uv = Vector2D(u0 * alpha + u1 * beta + u2 * gamma,
    //         v0 * alpha + v1 * beta + v2 * gamma);
    //         return tex.sample_nearest(uv);
    //       }
    //     );
    //     break;
    //   case P_LINEAR:
    //     rasterize_triangle_shaded(x0, y0, x1, y1, x2, y2,
    //       [&](float sx, float sy, float alpha, float beta, float gamma
    //       ) {
    //         Vector2D uv = Vector2D(u0 * alpha + u1 * beta + u2 * gamma,
    //         v0 * alpha + v1 * beta + v2 * gamma);
    //         return tex.sample_bilinear(uv);
    //       }
    //     );
    //     break;
    // }

    // Task 6: Set the correct barycentric differentials in the SampleParams struct.
    // Hint: You can reuse code from rasterize_triangle/rasterize_interpolated_color_triangle
    return rasterize_triangle_shaded(x0, y0, z0, x1, y1, z1, x2, y2, z2,
      [&](float sx, float sy, float alpha, float beta, float gamma) {
        Vector2D uv = Vector2D(u0 * alpha + u1 * beta + u2 * gamma,
          v0 * alpha + v1 * beta + v2 * gamma);
        auto [alpha1, beta1, gamma1] = barycentric_coordinates(sx+1, sy, x0, y0, x1, y1, x2, y2);
        Vector2D dx_uv = Vector2D(u0 * alpha1 + u1 * beta1 + u2 * gamma1,
          v0 * alpha1 + v1 * beta1 + v2 * gamma1);
        auto [alpha2, beta2, gamma2] = barycentric_coordinates(sx, sy+1, x0, y0, x1, y1, x2, y2);
        Vector2D dy_uv = Vector2D(u0 * alpha2 + u1 *beta2 + u2 * gamma2,
          v0 * alpha2 + v1 * beta2 + v2 * gamma2);

        SampleParams sp;
        sp.p_uv = uv;
        sp.p_dx_uv = dx_uv;
        sp.p_dy_uv = dy_uv;
        sp.lsm = lsm;
        sp.psm = psm;
        return tex.sample(sp);
      });
  }

  void RasterizerImp::set_sample_rate(unsigned int rate) {
    // Task 2: You may want to update this function for supersampling support
    // Task 2: Current covarage implementation dont change the buffer

    this->sample_rate = rate;

    this->sample_buffer.resize(width * height, Color::White);
    this->sample_buffer_depth.resize(width * height, numeric_limits<float>::infinity());
  }


  void RasterizerImp::set_framebuffer_target(unsigned char* rgb_framebuffer,
    size_t width, size_t height)
  {
    // Task 2: You may want to update this function for supersampling support
    // Task 2: Current covarage implementation dont change the buffer


    this->width = width;
    this->height = height;
    this->rgb_framebuffer_target = rgb_framebuffer;

    this->sample_buffer.resize(width * height, Color::White);
    this->sample_buffer_depth.resize(width * height, numeric_limits<float>::infinity());
  }


  void RasterizerImp::clear_buffers() {
    std::fill(rgb_framebuffer_target, rgb_framebuffer_target + 3 * width * height, 255);
    std::fill(sample_buffer.begin(), sample_buffer.end(), Color::White);
    std::fill(sample_buffer_depth.begin(), sample_buffer_depth.end(), numeric_limits<float>::infinity());
  }


  // This function is called at the end of rasterizing all elements of the
  // SVG file.  If you use a supersample buffer to rasterize SVG elements
  // for antialiasing, you could use this call to fill the target framebuffer
  // pixels from the supersample buffer data.
  //
  void RasterizerImp::resolve_to_framebuffer() {
    // Task 2: You will likely want to update this function for supersampling supports
    // Task 2: Current covarage implementation dont change the buffer


    for (int x = 0; x < width; ++x) {
      for (int y = 0; y < height; ++y) {
        int pixel_index = y * width + x;
        int rgb_index = 3 * pixel_index;

        Color col = sample_buffer[pixel_index];

        for (int k = 0; k < 3; ++k) {
          this->rgb_framebuffer_target[rgb_index + k] = col[k] * 255;
        }
      }
    }
  }

  Rasterizer::~Rasterizer() { }


}// CGL
