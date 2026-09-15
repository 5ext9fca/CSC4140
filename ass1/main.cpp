#include <cmath>
#include <iostream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <vector>
#include <eigen3/Eigen/Core>
#include <eigen3/Eigen/Dense>
#include <eigen3/Eigen/SVD>
#include <eigen3/Eigen/Geometry>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core/eigen.hpp>

using namespace std;
using namespace Eigen;
using namespace cv;

void basic_vec_op() {
    Vector4f v{1, 1.5, 2, 3};
    Vector4f w{0, 1, 2, 4};

    cout << "2.1 Basic vector operations: \n";
    cout << "v + w: \n";
    cout << v + w << '\n';
    cout << "v dot w: \n";
    cout << v.dot(w) << '\n';
}

void init_mat4(Matrix4i& mat, int (*val_func)(int m, int n)) {
    for (int m=0; m<4; m++) {
        for (int n=0; n<4; n++) {
            mat(m, n) = val_func(m, n);
        }
    }
}

void basic_mat_op() {
    Vector4f v{1, 1.5, 2, 3};
    Matrix4i i, j;
    init_mat4(i, [](int m, int n) -> int {return 4*m+n+1;});
    init_mat4(j, [](int m, int n) -> int {return 4*m-n+4;});

    cout << "2.2 Basic matrix operations: \n";
    cout << "i + j: \n";
    cout << i + j << '\n';
    cout << "i * j: \n";
    cout << i * j << '\n';
    cout << "i * v: \n";
    cout << i.cast<float>() * v << '\n';
}

Mat read_grey_scale() {
    Mat gray = imread("lenna.png", IMREAD_GRAYSCALE);

    if (gray.empty()) {
        throw runtime_error("Cannot read image");
    }

    return gray;
}

MatrixXf normalize(Mat gray) {
    Mat normalized_cv;
    gray.convertTo(
        normalized_cv,
        CV_32F,
        1.0 / 255.0
    );

    MatrixXf normalized_eigen;
    cv2eigen(normalized_cv, normalized_eigen);
    return normalized_eigen;
}

tuple<MatrixXf, VectorXf, MatrixXf> decomp(MatrixXf A) {
    JacobiSVD<MatrixXf> svd(
        A,
        ComputeThinU |
        ComputeThinV
    );

    const MatrixXf& U = svd.matrixU();
    const VectorXf& singular_values = svd.singularValues();
    const MatrixXf& V = svd.matrixV();

    return tuple{U, singular_values, V};
}

MatrixXf reconstruct_with_first_n(
    int n,
    const MatrixXf& U,
    const VectorXf& singular_values,
    const MatrixXf& V
) {
    return U.leftCols(n) * singular_values.head(n).asDiagonal() * V.leftCols(n).transpose();
}

bool write_gray_matrix(
    const string& filename,
    const MatrixXf& gray
) {
    Mat normalized_gray_cv;
    eigen2cv(gray, normalized_gray_cv);

    Mat gray_cv;
    normalized_gray_cv.convertTo(
        gray_cv,
        CV_8U,
        255.0
    );

    if (!imwrite(filename, gray_cv)) {
        cerr << "Failed to write: " << filename << '\n';
        return false;
    }

    return true;

}

void svd_decomp() {
    auto grey = read_grey_scale();
    auto normalized = normalize(grey);
    auto [U, singular_vals, V] = decomp(normalized);
    
    auto ns = vector<int>{1, 10, 50};
    vector<MatrixXf> reconstructed;
    for (auto n : ns) {
        reconstructed.push_back(reconstruct_with_first_n(n, U, singular_vals, V));
    }

    cout << "2.3 SVD decomposition of \"lenna\":\n";
    for (auto i=0; i<reconstructed.size(); i++) {
        string filename = "reconstructed_"+to_string(ns[i])+".png";
        write_gray_matrix(filename, reconstructed[i]);
        cout << filename << " written\n";
    }
}

void basic_tf_op() {
    constexpr double pi = 3.14159265358979323846;
    constexpr double degree_to_radian = pi / 180.0;

    Vector3d point{1, 2, 3};
    Vector3d center{4, 5, 6};

    const double angle_x = 45.0 * degree_to_radian;
    const double angle_y = 30.0 * degree_to_radian;
    const double angle_z = 60.0 * degree_to_radian;

    AngleAxisd R_x {angle_x, Vector3d::UnitX()};
    AngleAxisd R_y {angle_y, Vector3d::UnitY()};
    AngleAxisd R_z {angle_z, Vector3d::UnitZ()};
    Matrix3d R = R_z.toRotationMatrix() * R_y.toRotationMatrix() * R_x.toRotationMatrix();

    Vector3d rotated = center + R * (point - center);

    cout << "2.4 Rotated Point: \n";
    cout << rotated << '\n';
}

int main() {
    basic_vec_op();
    basic_mat_op();
    svd_decomp();
    basic_tf_op();
}