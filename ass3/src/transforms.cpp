#include "transforms.h"

#include "CGL/matrix3x3.h"
#include "CGL/vector2D.h"
#include "CGL/vector3D.h"

namespace CGL {

Vector2D operator*(const Matrix3x3 &m, const Vector2D &v) {
	Vector3D mv = m * Vector3D(v.x, v.y, 1);
	return Vector2D(mv.x / mv.z, mv.y / mv.z);
}

Matrix3x3 translate(float dx, float dy) {
	// Task 3: Fill this in.
	Matrix3x3 translation_matrix = Matrix3x3(
		1, 0, dx,
		0, 1, dy,
		0, 0, 1);

	return translation_matrix;
}

Matrix3x3 scale(float sx, float sy) {
	// Task 3: Fill this in.
	Matrix3x3 scale_matrix = Matrix3x3(
		sx, 0, 0,
		0, sy, 0,
		0, 0, 1);

	return scale_matrix;
}

// The input argument is in degrees counterclockwise
Matrix3x3 rotate(float deg) {
	// Task 3: Fill this in.
	float rad = deg * M_PI / 180.0;
	Matrix3x3 rotation_matrix = Matrix3x3(
		cos(rad), -sin(rad), 0,
		sin(rad), cos(rad), 0,
		0, 0, 1);

	return rotation_matrix;
}

}
