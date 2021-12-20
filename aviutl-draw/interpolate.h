#pragma once

#include "graphic.h"
#include <cmath>

namespace interpolate {
	using Interpolate = BGRA(*)(const ReadOnlyImage&, Vec2);

	BGRA nearestNeighbor(const ReadOnlyImage& img, Vec2 p) {
		return img.samplePixel(p);
	}

	BGRA bilinear(const ReadOnlyImage& img, Vec2 p) {
		int x = static_cast<int>(std::floor(p.x));
		int y = static_cast<int>(std::floor(p.y));
		auto dx = p.x - x;
		auto dy = p.y - y;
		auto c1 = img.getPixelSafe(x, y);
		auto c2 = img.getPixelSafe(x, y + 1);
		auto c3 = img.getPixelSafe(x + 1, y);
		auto c4 = img.getPixelSafe(x + 1, y + 1);

		return BGRA(
			(1 - dx) * (1 - dy) * c1.b + (1 - dx) * dy * c2.b + dx * (1 - dy) * c3.b + dx * dy * c4.b,
			(1 - dx) * (1 - dy) * c1.g + (1 - dx) * dy * c2.g + dx * (1 - dy) * c3.g + dx * dy * c4.g,
			(1 - dx) * (1 - dy) * c1.r + (1 - dx) * dy * c2.r + dx * (1 - dy) * c3.r + dx * dy * c4.r,
			(1 - dx) * (1 - dy) * c1.a + (1 - dx) * dy * c2.a + dx * (1 - dy) * c3.a + dx * dy * c4.a
		);
	}
}
