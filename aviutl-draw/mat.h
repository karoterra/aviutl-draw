#pragma once

#include <cmath>

struct Vec2 {
	float x, y;
};

class Mat {
	float m11, m12, m13;
	float m21, m22, m23;
	float m31, m32, m33;

public:
	Mat()
		: m11(1), m12(0), m13(0)
		, m21(0), m22(1), m23(0)
		, m31(0), m32(0), m33(1)
	{}

	Mat(
		float m11, float m12, float m13,
		float m21, float m22, float m23,
		float m31, float m32, float m33)
		: m11(m11), m12(m12), m13(m13)
		, m21(m21), m22(m22), m23(m23)
		, m31(m31), m32(m32), m33(m33)
	{}

	void scale(float sx, float sy) {
		m11 *= sx;
		m12 *= sx;
		m13 *= sx;
		m21 *= sy;
		m22 *= sy;
		m23 *= sy;
	}

	void translate(float x, float y) {
		m11 += x * m31;
		m12 += x * m32;
		m13 += x * m33;
		m21 += y * m31;
		m22 += y * m32;
		m23 += y * m33;
	}

	void rotate(float theta) {
		float c = std::cos(theta);
		float s = std::sin(theta);
		float x, y;
		x = c * m11 - s * m21;
		y = s * m11 + c * m21;
		m11 = x; m21 = y;
		x = c * m12 - s * m22;
		y = s * m12 + c * m22;
		m12 = x; m22 = y;
		x = c * m13 - s * m23;
		y = s * m13 + c * m23;
		m13 = x; m23 = y;
	}

	Mat inverse() {
		float A = m11 * m22 * m33 + m12 * m23 * m32 + m13 * m21 * m32
			- m13 * m22 * m31 - m12 * m21 * m33 - m11 * m23 * m32;
		A = 1 / A;
		return Mat(
			A * (m22 * m33 - m23 * m32),
			-A * (m12 * m33 - m13 * m32),
			A * (m12 * m23 - m13 * m22),
			-A * (m21 * m33 - m23 * m31),
			A * (m11 * m33 - m13 * m31),
			-A * (m11 * m23 - m13 * m21),
			A * (m21 * m32 - m22 * m31),
			-A * (m11 * m32 - m12 * m31),
			A * (m11 * m22 - m12 * m21)
		);
	}

	Vec2 transform(Vec2 p) {
		return Vec2{
			p.x * m11 + p.y * m12 + m13,
			p.x * m21 + p.y * m22 + m23,
		};
	}
};