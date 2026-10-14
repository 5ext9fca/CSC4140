#include <string>
#include <eigen3/Eigen/Eigen>
#include "Triangle.hpp"
#include "include/json.hpp"


namespace cfg {
class Config {
public:
    Config(const std::string& filename, bool command_line, float angle, const std::string& output_filename);
    Eigen::Vector4f get_eye_pos();
    Triangle get_triangle();
    float get_eye_fov();
    float get_aspect_ratio();
    float get_zNear();
    float get_zFar();
    float get_angle();
    std::string get_output_filename();
    bool get_command_line();
    Eigen::Vector3f get_T();
    Eigen::Vector3f get_S();
    Eigen::Vector3f get_P0();
    Eigen::Vector3f get_P1();
private:
    Eigen::Vector4f eye_pos;
    Triangle triangle;
    float eye_fov;
    float aspect_ratio;
    float zNear;
    float zFar;
    float angle;
    std::string output_filename;
    bool command_line;
    Eigen::Vector3f T;
    Eigen::Vector3f S;
    Eigen::Vector3f P0;
    Eigen::Vector3f P1;
private:
    Triangle init_triangle(const nlohmann::json& config);
    Eigen::Vector4f parse_vector4f(const nlohmann::json& j);
    Eigen::Vector3f parse_vector3f(const nlohmann::json& j);
};
}