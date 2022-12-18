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

	// í èÌ
	BGRA normal(BGRA dest, BGRA src) {
		return src;
	}

	// â¡éZ
	BGRA addition(BGRA dest, BGRA src) {
		return BGRA(
			static_cast<uint8_t>(min(dest.b + src.b, 255)),
			static_cast<uint8_t>(min(dest.g + src.g, 255)),
			static_cast<uint8_t>(min(dest.r + src.r, 255))
		);
	}

	// å∏éZ
	BGRA subtract(BGRA dest, BGRA src) {
		return BGRA(
			static_cast<uint8_t>(max(dest.b - src.b, 0)),
			static_cast<uint8_t>(max(dest.g - src.g, 0)),
			static_cast<uint8_t>(max(dest.r - src.r, 0))
		);
	}

	// èÊéZ
	BGRA multiply(BGRA dest, BGRA src) {
		return BGRA(
			static_cast<uint8_t>(dest.b * src.b / 255),
			static_cast<uint8_t>(dest.g * src.g / 255),
			static_cast<uint8_t>(dest.r * src.r / 255)
		);
	}

	// ÉXÉNÉäÅ[Éì
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

	// ÉIÅ[ÉoÅ[ÉåÉC
	BGRA overlay(BGRA dest, BGRA src) {
		return BGRA(
			overlayElem(dest.b, src.b),
			overlayElem(dest.g, src.g),
			overlayElem(dest.r, src.r)
		);
	}

	// î‰är(ñæ)
	BGRA lighten(BGRA dest, BGRA src) {
		return BGRA(
			max(dest.b, src.b),
			max(dest.g, src.g),
			max(dest.r, src.r)
		);
	}

	// î‰är(à√)
	BGRA darken(BGRA dest, BGRA src) {
		return BGRA(
			min(dest.b, src.b),
			min(dest.g, src.g),
			min(dest.r, src.r)
		);
	}

	// ãPìx
	BGRA luminosity(BGRA dest, BGRA src) {
		YCbCr yd(dest), ys(src);
		return BGRA(YCbCr(ys.y, yd.cb, yd.cr));
	}

	// êFç∑
	BGRA color(BGRA dest, BGRA src) {
		YCbCr yd(dest), ys(src);
		return BGRA(YCbCr(yd.y, ys.cb, ys.cr));
	}

	// âAâe(èƒÇ´çûÇ›ÉäÉjÉA)
	BGRA linearBurn(BGRA dest, BGRA src) {
		return BGRA(
			max(dest.b + src.b - 255, 0),
			max(dest.g + src.g - 255, 0),
			max(dest.r + src.r - 255, 0)
		);
	}

	// ñæà√
	BGRA linearLight(BGRA dest, BGRA src) {
		return BGRA(
			clamp(dest.b + 2 * src.b - 255, 0, 255),
			clamp(dest.g + 2 * src.g - 255, 0, 255),
			clamp(dest.r + 2 * src.r - 255, 0, 255)
		);
	}

	// ç∑ï™
	BGRA difference(BGRA dest, BGRA src) {
		return BGRA(
			abs(dest.b - src.b),
			abs(dest.g - src.g),
			abs(dest.r - src.r)
		);
	}

	Blend toBlend(int num) {
		switch (num) {
		case 0: return normal;
		case 1: return addition;
		case 2: return subtract;
		case 3: return multiply;
		case 4: return screen;
		case 5: return overlay;
		case 6: return lighten;
		case 7: return darken;
		case 8: return luminosity;
		case 9: return color;
		case 10: return linearBurn;
		case 11: return linearLight;
		case 12: return difference;
		}
		return normal;
	}

	Blend toBlend(const char* str) {
		if (strcmp(str, "Normal") == 0) return normal;
		else if (strcmp(str, "Addition") == 0) return addition;
		else if (strcmp(str, "Subtract") == 0) return subtract;
		else if (strcmp(str, "Multiply") == 0) return multiply;
		else if (strcmp(str, "Screen") == 0) return screen;
		else if (strcmp(str, "Overlay") == 0) return overlay;
		else if (strcmp(str, "Lighten") == 0) return lighten;
		else if (strcmp(str, "Darken") == 0) return darken;
		else if (strcmp(str, "Luminosity") == 0) return luminosity;
		else if (strcmp(str, "Color") == 0) return color;
		else if (strcmp(str, "LinearBurn") == 0) return linearBurn;
		else if (strcmp(str, "LinearLight") == 0) return linearLight;
		else if (strcmp(str, "Difference") == 0) return difference;
		else return normal;
	}
}
