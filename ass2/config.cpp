#include <fstream>
#include <string>
#include "include/json.hpp"
#include "config.hpp"

using json = nlohmann::json;

cfg::Config::Config(const std::string& filename, bool command_line, float angle, const std::string& output_filename) : command_line(command_line), angle(angle), output_filename(output_filename) {
    std::ifstream f(filename);
    json config = json::parse(f);

    eye_pos = parse_vector4f(config["eye_pos"]);

    triangle = init_triangle(config);

    eye_fov = config["eye_fov"];
    aspect_ratio = config["aspect_ratio"];
    zNear = config["zNear"];
    zFar = config["zFar"];
    T = parse_vector3f(config["T"]);
    S = parse_vector3f(config["S"]);
    P0 = parse_vector3f(config["P0"]);
    P1 = parse_vector3f(config["P1"]);
}

Eigen::Vector4f cfg::Config::get_eye_pos() {return eye_pos;}

Triangle cfg::Config::get_triangle() {return triangle;}

float cfg::Config::get_eye_fov() {return eye_fov;}

float cfg::Config::get_aspect_ratio() {return aspect_ratio;}

float cfg::Config::get_zNear() {return zNear;}

float cfg::Config::get_zFar() {return zFar;}

float cfg::Config::get_angle() {return angle;}

std::string cfg::Config::get_output_filename() {return output_filename;}

bool cfg::Config::get_command_line() {return command_line;}

Eigen::Vector3f cfg::Config::get_T() {return T;}

Eigen::Vector3f cfg::Config::get_S() {return S;}

Eigen::Vector3f cfg::Config::get_P0() {return P0;}

Eigen::Vector3f cfg::Config::get_P1() {return P1;}

Triangle cfg::Config::init_triangle(const nlohmann::json& config) {
    Triangle triangle;
    for (int i = 0; i < 3; ++i) {
        triangle.setVertex(i, parse_vector3f(config["triangle"]["vertices"][i]));
        triangle.setColor(i, config["triangle"]["colors"][i][0],
            config["triangle"]["colors"][i][1],
            config["triangle"]["colors"][i][2]);
        triangle.setTexCoord(i, config["triangle"]["tex_coords"][i][0],
            config["triangle"]["tex_coords"][i][1]);
        triangle.setNormal(i, parse_vector3f(config["triangle"]["normals"][i]));
    }
    return triangle;
}

Eigen::Vector4f cfg::Config::parse_vector4f(const nlohmann::json& j) {
    Eigen::Vector4f vec;
    std::vector<float> vec_data = j.get<std::vector<float>>();
    for (int i = 0; i < 4; ++i) {
        vec[i] = vec_data[i];
    }
    return vec;
}

Eigen::Vector3f cfg::Config::parse_vector3f(const nlohmann::json& j) {
    Eigen::Vector3f vec;
    std::vector<float> vec_data = j.get<std::vector<float>>();
    for (int i = 0; i < 3; ++i) {
        vec[i] = vec_data[i];
    }
    return vec;
}