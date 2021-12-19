#pragma once

#include "graphic.h"

namespace composite
{
	using Composite = void(*)(BGRA dest, BGRA src, int& fd, int& fs);

	void clear(BGRA dest, BGRA src, int& fd, int& fs) {
		fd = 0;
		fs = 0;
	}

	void copy(BGRA dest, BGRA src, int& fd, int& fs) {
		fd = 0;
		fs = 255;
	}

	void destination(BGRA dest, BGRA src, int& fd, int& fs) {
		fd = 255;
		fs = 0;
	}

	void sourceOver(BGRA dest, BGRA src, int& fd, int& fs) {
		fd = 255 - src.a;
		fs = 255;
	}

	void destinationOver(BGRA dest, BGRA src, int& fd, int& fs) {
		fd = 255;
		fs = 255 - dest.a;
	}

	void sourceIn(BGRA dest, BGRA src, int& fd, int& fs) {
		fd = 0;
		fs = dest.a;
	}

	void destinationIn(BGRA dest, BGRA src, int& fd, int& fs) {
		fd = src.a;
		fs = 0;
	}

	void sourceOut(BGRA dest, BGRA src, int& fd, int& fs) {
		fd = 0;
		fs = 255 - dest.a;
	}

	void destinationOut(BGRA dest, BGRA src, int& fd, int& fs) {
		fd = 255 - src.a;
		fs = 0;
	}

	void sourceAtop(BGRA dest, BGRA src, int& fd, int& fs) {
		fd = 255 - src.a;
		fs = dest.a;
	}

	void destinationAtop(BGRA dest, BGRA src, int& fd, int& fs) {
		fd = src.a;
		fs = 255 - dest.a;
	}

	void exclusiveOR(BGRA dest, BGRA src, int& fd, int& fs) {
		fd = 255 - src.a;
		fs = 255 - dest.a;
	}

	void lighter(BGRA dest, BGRA src, int& fd, int& fs) {
		fd = 255;
		fs = 255;
	}
}
