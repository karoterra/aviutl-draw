#pragma once

#include <stdint.h>
#include <vector>
#include "mat.h"

struct YCbCr;

struct BGRA {
	uint8_t b;
	uint8_t g;
	uint8_t r;
	uint8_t a;

	BGRA() : b(0), g(0), r(0), a(0) {}

	BGRA(uint8_t b, uint8_t g, uint8_t r, uint8_t a = 255)
		: b(b), g(g), r(r), a(a)
	{}

	BGRA(const YCbCr c);
};

struct YCbCr {
	short y;
	short cb;
	short cr;

	YCbCr() : y(0), cb(0), cr(0) {}

	YCbCr(short y, short cb, short cr)
		: y(y), cb(cb), cr(cr)
	{}

	YCbCr(const BGRA rgb);
};

struct ReadOnlyImage {
	const BGRA* data;
	int width;
	int height;

	ReadOnlyImage() : data(nullptr), width(0), height(0) {}

	ReadOnlyImage(const BGRA* buf, int w, int h)
		: data(buf), width(w), height(h)
	{}

	inline BGRA getPixel(int x, int y) const {
		return data[x + width * y];
	}

	inline BGRA getPixelSafe(int x, int y) const {
		if (x < 0 || width <= x || y < 0 || height <= y) {
			return BGRA(0, 0, 0, 0);
		}
		return data[x + width * y];
	}

	template<class T>
	BGRA samplePixel(Vec2<T> p) const {
		int x = static_cast<int>(p.x + 0.5);
		int y = static_cast<int>(p.y + 0.5);
		return getPixelSafe(x, y);
	}
};

struct Image {
	std::vector<BGRA> data;
	int width;
	int height;

	Image() : data(), width(0), height(0) {}

	Image(const BGRA* buf, int w, int h) : data(w* h), width(w), height(h) {
		for (int i = 0; i < w * h; i++) {
			data[i] = buf[i];
		}
	}

	void clear() {
		for (int i = 0; i < width * height; i++) {
			data[i] = BGRA(0, 0, 0, 0);
		}
	}

	void clear(int w, int h) {
		width = w;
		height = h;
		data.resize(w * h);
		for (int i = 0; i < w * h; i++) {
			data[i] = BGRA(0, 0, 0, 0);
		}
	}

	void setData(const BGRA* buf, int w, int h) {
		width = w;
		height = h;
		data.resize(w * h);
		for (int i = 0; i < w * h; i++) {
			data[i] = buf[i];
		}
	}

	inline BGRA getPixel(int x, int y) const {
		return data[x + width * y];
	}

	inline void setPixel(int x, int y, const BGRA px) {
		data[x + width * y] = px;
	}

	template<class T>
	BGRA samplePixel(Vec2<T> p) const {
		int x = static_cast<int>(p.x + 0.5);
		int y = static_cast<int>(p.y + 0.5);
		if (x < 0 || width <= x || y < 0 || height <= y) {
			return BGRA{ 0,0,0,0 };
		}
		return data[x + width * y];
	}
};
