#pragma once

#include "graphic.h"
#include <algorithm>

namespace blend
{
	using std::min;
	using std::max;
	using std::abs;
	using std::clamp;

	using Blend = BGRA(*)(BGRA dest, BGRA src);

	// �ʏ�
	BGRA normal(BGRA dest, BGRA src) {
		return src;
	}

	// ���Z
	BGRA addition(BGRA dest, BGRA src) {
		return BGRA(
			static_cast<uint8_t>(min(dest.b + src.b, 255)),
			static_cast<uint8_t>(min(dest.g + src.g, 255)),
			static_cast<uint8_t>(min(dest.r + src.r, 255))
		);
	}

	// ���Z
	BGRA subtract(BGRA dest, BGRA src) {
		return BGRA(
			static_cast<uint8_t>(max(dest.b - src.b, 0)),
			static_cast<uint8_t>(max(dest.g - src.g, 0)),
			static_cast<uint8_t>(max(dest.r - src.r, 0))
		);
	}

	// ��Z
	BGRA multiply(BGRA dest, BGRA src) {
		return BGRA(
			static_cast<uint8_t>(dest.b * src.b / 255),
			static_cast<uint8_t>(dest.g * src.g / 255),
			static_cast<uint8_t>(dest.r * src.r / 255)
		);
	}

	// �X�N���[��
	BGRA screen(BGRA dest, BGRA src) {
		return BGRA(
			static_cast<uint8_t>(dest.b + src.b - dest.b * src.b / 255),
			static_cast<uint8_t>(dest.g + src.g - dest.g * src.g / 255),
			static_cast<uint8_t>(dest.r + src.r - dest.r * src.r / 255)
		);
	}

	uint8_t overlayElem(int a, int b) {
		if (a < 128)
			return static_cast<uint8_t>(2 * a * b / 255);
		else
			return static_cast<uint8_t>(255 - 2 * (255 - a) * (255 - b) / 255);
	}

	// �I�[�o�[���C
	BGRA overlay(BGRA dest, BGRA src) {
		return BGRA(
			overlayElem(dest.b, src.b),
			overlayElem(dest.g, src.g),
			overlayElem(dest.r, src.r)
		);
	}

	// ��r(��)
	BGRA lighten(BGRA dest, BGRA src) {
		return BGRA(
			max(dest.b, src.b),
			max(dest.g, src.g),
			max(dest.r, src.r)
		);
	}

	// ��r(��)
	BGRA darken(BGRA dest, BGRA src) {
		return BGRA(
			min(dest.b, src.b),
			min(dest.g, src.g),
			min(dest.r, src.r)
		);
	}

	// �P�x
	BGRA luminosity(BGRA dest, BGRA src) {
		YCbCr yd(dest), ys(src);
		return BGRA(YCbCr(ys.y, yd.cb, yd.cr));
	}

	// �F��
	BGRA color(BGRA dest, BGRA src) {
		YCbCr yd(dest), ys(src);
		return BGRA(YCbCr(yd.y, ys.cb, ys.cr));
	}

	// �A�e(�Ă����݃��j�A)
	BGRA linearBurn(BGRA dest, BGRA src) {
		return BGRA(
			max(dest.b + src.b - 255, 0),
			max(dest.g + src.g - 255, 0),
			max(dest.r + src.r - 255, 0)
		);
	}

	// ����
	BGRA linearLight(BGRA dest, BGRA src) {
		return BGRA(
			clamp(dest.b + 2 * src.b - 255, 0, 255),
			clamp(dest.g + 2 * src.g - 255, 0, 255),
			clamp(dest.r + 2 * src.r - 255, 0, 255)
		);
	}

	// ����
	BGRA difference(BGRA dest, BGRA src) {
		return BGRA(
			abs(dest.b - src.b),
			abs(dest.g - src.g),
			abs(dest.r - src.r)
		);
	}
}
