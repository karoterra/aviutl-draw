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

	// 通常
	BGRA normal(BGRA dest, BGRA src) {
		return src;
	}

	// 加算
	BGRA addition(BGRA dest, BGRA src) {
		return BGRA(
			static_cast<uint8_t>(min(dest.b + src.b, 255)),
			static_cast<uint8_t>(min(dest.g + src.g, 255)),
			static_cast<uint8_t>(min(dest.r + src.r, 255))
		);
	}

	// 減算
	BGRA subtract(BGRA dest, BGRA src) {
		return BGRA(
			static_cast<uint8_t>(max(dest.b - src.b, 0)),
			static_cast<uint8_t>(max(dest.g - src.g, 0)),
			static_cast<uint8_t>(max(dest.r - src.r, 0))
		);
	}

	// 乗算
	BGRA multiply(BGRA dest, BGRA src) {
		return BGRA(
			static_cast<uint8_t>(dest.b * src.b / 255),
			static_cast<uint8_t>(dest.g * src.g / 255),
			static_cast<uint8_t>(dest.r * src.r / 255)
		);
	}

	// スクリーン
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

	// オーバーレイ
	BGRA overlay(BGRA dest, BGRA src) {
		return BGRA(
			overlayElem(dest.b, src.b),
			overlayElem(dest.g, src.g),
			overlayElem(dest.r, src.r)
		);
	}

	// 比較(明)
	BGRA lighten(BGRA dest, BGRA src) {
		return BGRA(
			max(dest.b, src.b),
			max(dest.g, src.g),
			max(dest.r, src.r)
		);
	}

	// 比較(暗)
	BGRA darken(BGRA dest, BGRA src) {
		return BGRA(
			min(dest.b, src.b),
			min(dest.g, src.g),
			min(dest.r, src.r)
		);
	}

	// 輝度
	BGRA luminosity(BGRA dest, BGRA src) {
		YCbCr yd(dest), ys(src);
		return BGRA(YCbCr(ys.y, yd.cb, yd.cr));
	}

	// 色差
	BGRA color(BGRA dest, BGRA src) {
		YCbCr yd(dest), ys(src);
		return BGRA(YCbCr(yd.y, ys.cb, ys.cr));
	}

	// 陰影(焼き込みリニア)
	BGRA linearBurn(BGRA dest, BGRA src) {
		return BGRA(
			max(dest.b + src.b - 255, 0),
			max(dest.g + src.g - 255, 0),
			max(dest.r + src.r - 255, 0)
		);
	}

	// 明暗
	BGRA linearLight(BGRA dest, BGRA src) {
		return BGRA(
			clamp(dest.b + 2 * src.b - 255, 0, 255),
			clamp(dest.g + 2 * src.g - 255, 0, 255),
			clamp(dest.r + 2 * src.r - 255, 0, 255)
		);
	}

	// 差分
	BGRA difference(BGRA dest, BGRA src) {
		return BGRA(
			abs(dest.b - src.b),
			abs(dest.g - src.g),
			abs(dest.r - src.r)
		);
	}

	// 除外
	BGRA exclusion(BGRA dest, BGRA src) {
		return BGRA(
			(int)dest.b + (int)src.b - 2 * (int)dest.b * (int)src.b / 255,
			(int)dest.g + (int)src.g - 2 * (int)dest.g * (int)src.g / 255,
			(int)dest.r + (int)src.r - 2 * (int)dest.r * (int)src.r / 255
		);
	}

	// 除算
	BGRA divide(BGRA dest, BGRA src) {
		return BGRA(
			src.b == 0 ? 255 : min(255, 255 * (int)dest.b / (int)src.b),
			src.g == 0 ? 255 : min(255, 255 * (int)dest.g / (int)src.g),
			src.r == 0 ? 255 : min(255, 255 * (int)dest.r / (int)src.r)
		);
	}

	// 覆い焼きカラー
	BGRA colorDodge(BGRA dest, BGRA src) {
		auto f = [](int d, int s) {
			if (d == 0) return 0;
			else if (s == 255) return 255;
			return min(255, 255 * d / (255 - s));
		};
		return BGRA(f(dest.b, src.b), f(dest.g, src.g), f(dest.r, src.r));
	}

	// 焼き込みカラー
	BGRA colorBurn(BGRA dest, BGRA src) {
		auto f = [](int d, int s) {
			if (d == 255) return 255;
			else if (s == 0) return 0;
			return 255 - min(255, 255 * (255 - d) / s);
		};
		return BGRA(f(dest.b, src.b), f(dest.g, src.g), f(dest.r, src.r));
	}

	// ハードミックス
	BGRA hardMix(BGRA dest, BGRA src) {
		return BGRA(
			(int)dest.b + (int)src.b >= 255 ? 255 : 0,
			(int)dest.g + (int)src.g >= 255 ? 255 : 0,
			(int)dest.r + (int)src.r >= 255 ? 255 : 0
		);
	}

	// AND
	BGRA binaryAnd(BGRA dest, BGRA src) {
		return BGRA(
			dest.b & src.b,
			dest.g & src.b,
			dest.r & src.b
		);
	}

	// NAND
	BGRA binaryNand(BGRA dest, BGRA src) {
		return BGRA(
			~(dest.b & src.b),
			~(dest.g & src.b),
			~(dest.r & src.b)
		);
	}

	// OR
	BGRA binaryOr(BGRA dest, BGRA src) {
		return BGRA(
			dest.b | src.b,
			dest.g | src.b,
			dest.r | src.b
		);
	}

	// NOR
	BGRA binaryNor(BGRA dest, BGRA src) {
		return BGRA(
			~(dest.b | src.b),
			~(dest.g | src.b),
			~(dest.r | src.b)
		);
	}

	// XOR
	BGRA binaryXor(BGRA dest, BGRA src) {
		return BGRA(
			dest.b ^ src.b,
			dest.g ^ src.b,
			dest.r ^ src.b
		);
	}

	// XNOR
	BGRA binaryXnor(BGRA dest, BGRA src) {
		return BGRA(
			~(dest.b ^ src.b),
			~(dest.g ^ src.b),
			~(dest.r ^ src.b)
		);
	}

	// IMPLICATION
	BGRA binaryImplication(BGRA dest, BGRA src) {
		return BGRA(
			~dest.b | src.b,
			~dest.g | src.b,
			~dest.r | src.b
		);
	}

	// NOT IMPLICATION
	BGRA binaryNotImplication(BGRA dest, BGRA src) {
		return BGRA(
			dest.b & ~src.b,
			dest.g & ~src.b,
			dest.r & ~src.b
		);
	}

	// CONVERSE
	BGRA binaryConverse(BGRA dest, BGRA src) {
		return BGRA(
			dest.b | ~src.b,
			dest.g | ~src.b,
			dest.r | ~src.b
		);
	}

	// NOT CONVERSE
	BGRA binaryNotConverse(BGRA dest, BGRA src) {
		return BGRA(
			~dest.b & src.b,
			~dest.g & src.b,
			~dest.r & src.b
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
		else if (strcmp(str, "Exclusion") == 0) return exclusion;
		else if (strcmp(str, "Divide") == 0) return divide;
		else if (strcmp(str, "ColorDodge") == 0) return colorDodge;
		else if (strcmp(str, "ColorBurn") == 0) return colorBurn;
		else if (strcmp(str, "HardMix") == 0) return hardMix;
		else if (strcmp(str, "AND") == 0) return binaryAnd;
		else if (strcmp(str, "NAND") == 0) return binaryNand;
		else if (strcmp(str, "OR") == 0) return binaryOr;
		else if (strcmp(str, "NOR") == 0) return binaryNor;
		else if (strcmp(str, "XOR") == 0) return binaryXor;
		else if (strcmp(str, "XNOR") == 0) return binaryXnor;
		else if (strcmp(str, "IMPLICATION") == 0) return binaryImplication;
		else if (strcmp(str, "NOT IMPLICATION") == 0) return binaryNotImplication;
		else if (strcmp(str, "CONVERSE") == 0) return binaryConverse;
		else if (strcmp(str, "NOT CONVERSE") == 0) return binaryNotConverse;
		else return normal;
	}
}
