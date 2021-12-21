#pragma once

#include <cmath>

template<class T>
struct Vec2 {
	T x, y;
};

template<class T>
class Mat {
public:
	T m11, m12, m13;
	T m21, m22, m23;
	T m31, m32, m33;

	Mat()
		: m11(1), m12(0), m13(0)
		, m21(0), m22(1), m23(0)
		, m31(0), m32(0), m33(1)
	{}

	Mat(
		T m11, T m12, T m13,
		T m21, T m22, T m23,
		T m31, T m32, T m33)
		: m11(m11), m12(m12), m13(m13)
		, m21(m21), m22(m22), m23(m23)
		, m31(m31), m32(m32), m33(m33)
	{}

	void scale(T sx, T sy) {
		m11 *= sx;
		m12 *= sx;
		m13 *= sx;
		m21 *= sy;
		m22 *= sy;
		m23 *= sy;
	}

	void translate(T x, T y) {
		m11 += x * m31;
		m12 += x * m32;
		m13 += x * m33;
		m21 += y * m31;
		m22 += y * m32;
		m23 += y * m33;
	}

	void rotate(T theta) {
		T c = std::cos(theta);
		T s = std::sin(theta);
		T x, y;
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

	Mat<T> inverse() {
		T A = m11 * m22 * m33 + m12 * m23 * m32 + m13 * m21 * m32
			- m13 * m22 * m31 - m12 * m21 * m33 - m11 * m23 * m32;
		A = 1 / A;
		return Mat<T>(
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

	Vec2<T> transform(Vec2<T> p) {
		return Vec2<T>{
			p.x * m11 + p.y * m12 + m13,
			p.x * m21 + p.y * m22 + m23,
		};
	}

	Vec2<T> mapPerspective(Vec2<T> p) {
		T x = p.x * m11 + p.y * m12 + m13;
		T y = p.x * m21 + p.y * m22 + m23;
		T w = p.x * m31 + p.y * m32 + m33;
		return Vec2<T>{ x / w, y / w };
	}
};

template<class T>
T cross(Vec2<T> p0, Vec2<T> p1, Vec2<T> p2) {
	return (p1.x - p0.x) * (p2.y - p0.y) - (p2.x - p0.x) * (p1.y - p0.y);
}

void getPerspective(Vec2<double> dst[], Vec2<double> src[], Mat<double>& mat);
