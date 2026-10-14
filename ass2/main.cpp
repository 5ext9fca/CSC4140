#include "Triangle.hpp"
#include "rasterizer.hpp"
#include "include/CLI11.hpp"
#include <eigen3/Eigen/Eigen>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <algorithm>
#include <cmath>
#include <filesystem>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#include "config.hpp"

constexpr double MY_PI = 3.1415926;

using Vec3 = Eigen::Vector3f;
using Vec4 = Eigen::Vector4f;
using Mat3 = Eigen::Matrix3f;
using Mat4 = Eigen::Matrix4f;

Vec4 poi3_to_vec4(Vec3 vec3) {
    Vec4 vec4 = Vec4::Ones();
    vec4.topLeftCorner<3, 1>() = vec3;
    return vec4;
}

Vec4 vec3_to_vec4(Vec3 vec3) {
    Vec4 vec4 = Vec4::Zero();
    vec4.topLeftCorner<3, 1>() = vec3;
    return vec4;
}

Vec3 vec4_to_vec3(Vec4 vec4) {
    Vec3 vec3 = Vec3::Zero();
    vec3 = vec4.topLeftCorner<3, 1>();
    return vec3;
}

Mat4 mat3_to_mat4(Mat3 mat3) {
    Mat4 mat4 = Mat4::Identity();
    mat4.topLeftCorner<3, 3>() = mat3;
    return mat4;
}

Mat3 mat4_to_mat3(Mat4 mat4) {
    Mat3 mat3 = Mat3::Identity();
    mat3 = mat4.topLeftCorner<3, 3>();
    return mat3;
}

Mat4 get_translation_matrix(Vec3 translation) {
    Mat4 translator;
    translator << 
    1, 0, 0, translation[0], 
    0, 1, 0, translation[1], 
    0, 0, 1, translation[2], 
    0, 0, 0, 1;
    return translator;
}

Mat4 get_scaling_matrix(Vec3 scale) {
    Mat4 scaler;
    scaler <<
    scale[0], 0, 0, 0, 
    0, scale[1], 0, 0, 
    0, 0, scale[2], 0,
    0, 0, 0, 1;
    return scaler;
}

Mat4 get_view_matrix(Vec3 eye_pos)
{
    Mat4 view = Mat4::Identity();

    Mat4 translate = get_translation_matrix(-eye_pos);

    // The camera looks along -z with +y up, so the view rotation is the identity
    // and only the translation of the eye to the origin is needed.
    view = translate * view;
    // std::clog << "view" << std::endl << view << std::endl;  // check data

    return view;
}

Mat3 get_cross_product_matrix(Vec3 vec) {
    Mat3 mat;
    mat <<
    0, -vec[2], vec[1], 
    vec[2], 0, -vec[0], 
    -vec[1], vec[0], 0;
    return mat;
}

bool check_axis(Vec3 axis) {
    return axis.allFinite() && std::isfinite(axis.norm()) && axis.norm() > 1e-6;
}

Mat4 get_rotation_matrix(float angle, Vec3 axis) {

    if (!check_axis(axis)) {
        std::ostringstream oss;
        oss << "Invalid rotation axis: " << axis.transpose().format(Eigen::IOFormat());
        throw std::invalid_argument(oss.str());
    }

    axis.normalize();

    Mat3 rotation_matrix_3 = 
    Mat3::Identity() * cosf(angle)
    + axis * axis.transpose() * (1 - cosf(angle))
    + get_cross_product_matrix(axis) * sinf(angle);

    return mat3_to_mat4(rotation_matrix_3);
}

Mat4 get_rotation_matrix(float angle, Vec3 P0, Vec3 P1) {
    Vec3 axis = (P1 - P0);
    
    return 
    get_translation_matrix(P0)
    * get_rotation_matrix(angle, axis)
    * get_translation_matrix(-P0);
}

Mat4 get_model_matrix(float rotation_angle, Vec3 T, Vec3 S, Vec3 P0, Vec3 P1)
{

    // rotation_angle is given in degrees; convert it to radians before using sin/cos.
    rotation_angle = rotation_angle * MY_PI / 180;

    //Step 1: Build the translation matrix M_trans from T (4x4, homogeneous coordinates).
    Mat4 model_translation = get_translation_matrix(T);

    //Step 2: Build the scale matrix M_scale from S (4x4, homogeneous coordinates).
    Mat4 model_scaling = get_scaling_matrix(S);

    //Step 3: Implement Rodrigues' rotation formula: rotation by angle theta around axis u.
    // The axis is determined by two points: it passes through P0, with direction u = P1 - P0.
    // Hint: normalize u first. Because the axis passes through P0 (not necessarily the origin),
    // translate P0 to the origin, rotate, then translate back: M_rot = T(P0) * R(u, theta) * T(-P0).
    Mat4 model_rotation = get_rotation_matrix(rotation_angle, P0, P1);

    //Step 4: Combine the steps into the model matrix: scale first, then rotate, then translate:
    // model = M_trans * M_rot * M_scale
    Mat4 model_transform = model_translation * model_rotation * model_scaling;

    //Step 5: Use Eigen's "AngleAxisf" to verify your rotation, e.g.
    //Eigen::AngleAxisf rotation_vector(radian, u.normalized());
    //Eigen::Matrix3f rotation_matrix = rotation_vector.toRotationMatrix();
    Vec3 axis = (P1 - P0).normalized();
    Eigen::AngleAxisf rotation_vector(rotation_angle, axis);
    Mat3 rotation_matrix_3 = rotation_vector.toRotationMatrix();

    Mat4 rotation_matrix_4 = mat3_to_mat4(rotation_matrix_3);
    rotation_matrix_4 = get_translation_matrix(P0) * rotation_matrix_4 * get_translation_matrix(-P0);

    if (!model_rotation.isApprox(rotation_matrix_4)) {
        throw std::logic_error("Rodrigues rotation does not match Eigen::AngleAxisf");
    }

	return model_transform;
}

Mat4 get_perspective_to_orthogornal_matrix(float near, float far) {
    Mat4 mat;
    mat << 
    near, 0, 0, 0, 
    0, near, 0, 0, 
    0, 0, near+far, -near*far, 
    0, 0, 1, 0;
    return mat;
}

Mat4 get_orthogornal_projection_matrix(float ver_fov, float aspect_ratio, float near, float far) {
    float top = tanf(ver_fov/2) * -near;
    float right = top * aspect_ratio;

    Mat4 scaling, translation;
    scaling << 
    1/right, 0, 0, 0, 
    0, 1/top, 0, 0, 
    0, 0, 2/(far-near), 0,
    0, 0, 0, 1;
    translation << 
    1, 0, 0, 0, 
    0, 1, 0, 0, 
    0, 0, 1, -(far+near)/2, 
    0, 0, 0, 1;

    return scaling * translation;
}

Mat4 get_projection_matrix(float eye_fov, float aspect_ratio,
                                      float zNear, float zFar)
{
    // TODO: Implement this function
    // Create the projection matrix for the given parameters.
    // Then return it.
    // Note: eye_fov is in degrees. zNear and zFar are positive distances; since the
    // camera looks along -z, the near and far planes are at z = -zNear and z = -zFar.
    float ver_fov = eye_fov * MY_PI / 180;

    Mat4 ortho, persp_to_ortho, projection;
    
    // frustum -> cubic
    ortho = get_orthogornal_projection_matrix(ver_fov, aspect_ratio, -zNear, -zFar);
    
    // orthographic projection
    persp_to_ortho = get_perspective_to_orthogornal_matrix(-zNear, -zFar);

    // squash all transformations
    projection = ortho * persp_to_ortho;

    return projection;
}

namespace {

std::string resolve_config_filename(const std::string& requested)
{
    namespace fs = std::filesystem;

    const fs::path requested_path(requested);
    if (fs::is_regular_file(requested_path)) {
        return requested_path.string();
    }

    if (requested_path.is_relative()) {
        const fs::path config_path = fs::path("config") / requested_path;
        if (fs::is_regular_file(config_path)) {
            return config_path.string();
        }
    }

    throw std::runtime_error("Config file not found: " + requested);
}

std::string make_snapshot_filename(const std::string& output_filename, int snapshot_index)
{
    namespace fs = std::filesystem;

    const fs::path output_path(output_filename);
    const std::string extension = output_path.has_extension()
        ? output_path.extension().string()
        : ".png";
    const std::string stem = output_path.has_stem()
        ? output_path.stem().string()
        : "result";

    std::ostringstream numbered_name;
    numbered_name << stem << "_interactive_" << std::setfill('0') << std::setw(3)
                  << snapshot_index << extension;
    return (output_path.parent_path() / numbered_name.str()).string();
}

void save_image(const std::string& filename, const cv::Mat& image)
{
    if (!cv::imwrite(filename, image)) {
        throw std::runtime_error("Failed to save image: " + filename);
    }
}

void print_interactive_help()
{
    std::cout
        << "Interactive controls:\n"
        << "  a / d       rotate +10 / -10 degrees\n"
        << "  j / l       translate along -x / +x\n"
        << "  w / s       translate along +y / -y\n"
        << "  q / e       translate along +z / -z\n"
        << "  + / -       scale up / down uniformly\n"
        << "  [ / ]       decrease / increase vertical FOV\n"
        << "  p           save a numbered screenshot\n"
        << "  r           reset angle, translation, scale, and FOV\n"
        << "  h           show this help\n"
        << "  Esc         quit\n";
}

void print_interactive_state(float angle, const Vec3& translation,
                             const Vec3& scale, float eye_fov)
{
    std::cout << std::fixed << std::setprecision(2)
              << "angle=" << angle
              << ", T=(" << translation.x() << ", " << translation.y() << ", "
              << translation.z() << ")"
              << ", S=(" << scale.x() << ", " << scale.y() << ", "
              << scale.z() << ")"
              << ", FOV=" << eye_fov << '\n';
}

} // namespace

int main(int argc, const char** argv)
{
    CLI::App app{"Render a transformed triangle in batch or interactive mode"};

    float angle = 0.0f;
    bool interactive = false;
    std::string output_filename = "result.png";
    std::string config_filename = "config/config.json";
    std::string legacy_output_filename;
    std::string legacy_config_filename;

    CLI::Option* rotation_option = app.add_option(
        "-r,--rotation", angle, "Initial model rotation angle in degrees");
    CLI::Option* output_option = app.add_option(
        "-o,--output", output_filename, "Output PNG filename");
    CLI::Option* config_option = app.add_option(
        "-c,--config", config_filename, "JSON configuration file");
    app.add_flag("-i,--interactive", interactive,
                 "Open the interactive window instead of rendering one image");
    app.add_option("OUTPUT_FILE", legacy_output_filename,
                   "Legacy positional output filename used after -r <angle>");
    app.add_option("CONFIG_FILE", legacy_config_filename,
                   "Legacy positional configuration filename");
    app.footer(
        "Examples:\n"
        "  ./ass2 -r 30 result.png\n"
        "  ./ass2 -r 45 -o results_1.png -c config/config1.json\n"
        "  ./ass2 --interactive -r 15 -c config/config2.json");

    try {
        app.parse(argc, argv);
    }
    catch (const CLI::ParseError& error) {
        return app.exit(error);
    }

    if (!legacy_output_filename.empty()) {
        if (output_option->count() != 0) {
            std::cerr << "Error: output was supplied both positionally and with --output\n";
            return 2;
        }
        output_filename = legacy_output_filename;
    }
    if (!legacy_config_filename.empty()) {
        if (config_option->count() != 0) {
            std::cerr << "Error: config was supplied both positionally and with --config\n";
            return 2;
        }
        config_filename = legacy_config_filename;
    }

    // Rendering once remains the default whenever batch-only output arguments are
    // present. With no arguments, the original interactive behavior is preserved.
    const bool command_line = !interactive &&
        (rotation_option->count() != 0 || output_option->count() != 0 ||
         !legacy_output_filename.empty());

    try {
        config_filename = resolve_config_filename(config_filename);
        cfg::Config config(config_filename, command_line, angle, output_filename);

        constexpr int width = 1024;
        constexpr int height = 1024;
        rst::rasterizer rasterizer(width, height);

        const Vec3 eye_pos = vec4_to_vec3(config.get_eye_pos());
        const Triangle triangle = config.get_triangle();
        const std::vector<Vec3> positions{{triangle.a()}, {triangle.b()}, {triangle.c()}};
        const std::vector<Eigen::Vector3i> indices{{0, 1, 2}};
        const auto pos_id = rasterizer.load_positions(positions);
        const auto ind_id = rasterizer.load_indices(indices);

        Vec3 translation = config.get_T();
        Vec3 scale = config.get_S();
        const Vec3 P0 = config.get_P0();
        const Vec3 P1 = config.get_P1();
        float eye_fov = config.get_eye_fov();
        const float aspect_ratio = config.get_aspect_ratio();
        const float zNear = config.get_zNear();
        const float zFar = config.get_zFar();

        const float initial_angle = angle;
        const Vec3 initial_translation = translation;
        const Vec3 initial_scale = scale;
        const float initial_eye_fov = eye_fov;

        auto render_frame = [&]() {
            rasterizer.clear(rst::Buffers::Color | rst::Buffers::Depth);
            rasterizer.set_model(get_model_matrix(angle, translation, scale, P0, P1));
            rasterizer.set_view(get_view_matrix(eye_pos));
            rasterizer.set_projection(
                get_projection_matrix(eye_fov, aspect_ratio, zNear, zFar));
            rasterizer.draw(pos_id, ind_id, rst::Primitive::Triangle);

            cv::Mat float_image(height, width, CV_32FC3,
                                rasterizer.frame_buffer().data());
            cv::Mat image;
            float_image.convertTo(image, CV_8UC3, 1.0f);
            return image;
        };

        if (command_line) {
            const cv::Mat image = render_frame();
            save_image(output_filename, image);
            std::cout << "Saved " << output_filename << '\n';
            return 0;
        }

        constexpr float translation_step = 0.1f;
        constexpr float scale_factor = 1.1f;
        constexpr float fov_step = 2.0f;
        int snapshot_index = 1;
        int key = 0;

        print_interactive_help();
        print_interactive_state(angle, translation, scale, eye_fov);

        while (key != 27) {
            const cv::Mat image = render_frame();
            cv::imshow("Transformation - press h for help", image);
            key = cv::waitKey(10) & 0xff;

            bool state_changed = true;
            switch (key) {
            case 'a': angle += 10.0f; break;
            case 'd': angle -= 10.0f; break;
            case 'j': translation.x() -= translation_step; break;
            case 'l': translation.x() += translation_step; break;
            case 'w': translation.y() += translation_step; break;
            case 's': translation.y() -= translation_step; break;
            case 'q': translation.z() += translation_step; break;
            case 'e': translation.z() -= translation_step; break;
            case '+':
            case '=': scale *= scale_factor; break;
            case '-': scale /= scale_factor; break;
            case '[': eye_fov = std::max(1.0f, eye_fov - fov_step); break;
            case ']': eye_fov = std::min(179.0f, eye_fov + fov_step); break;
            case 'r':
                angle = initial_angle;
                translation = initial_translation;
                scale = initial_scale;
                eye_fov = initial_eye_fov;
                break;
            case 'p': {
                std::string snapshot_filename;
                do {
                    snapshot_filename =
                        make_snapshot_filename(output_filename, snapshot_index++);
                } while (std::filesystem::exists(snapshot_filename));
                save_image(snapshot_filename, image);
                std::cout << "Saved " << snapshot_filename << '\n';
                state_changed = false;
                break;
            }
            case 'h':
                print_interactive_help();
                state_changed = false;
                break;
            default:
                state_changed = false;
                break;
            }

            if (state_changed) {
                print_interactive_state(angle, translation, scale, eye_fov);
            }
        }

        cv::destroyAllWindows();
        return 0;
    }
    catch (const std::exception& error) {
        std::cerr << "Error: " << error.what() << '\n';
        return 1;
    }
}
