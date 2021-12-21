#include "graphic.h"
#include <algorithm>

using std::clamp;

BGRA::BGRA(const YCbCr c) {
	a = 255;
	r = static_cast<uint8_t>(clamp(c.y + 1.402 * c.cr, 0., 255.));
	g = static_cast<uint8_t>(clamp(c.y - 0.344136 * c.cb - 0.714136 * c.cr, 0., 255.));
	b = static_cast<uint8_t>(clamp(c.y + 1.772 * c.cb, 0., 255.));
}

YCbCr::YCbCr(const BGRA rgb) {
	y = static_cast<short>(0.299 * rgb.r + 0.587 * rgb.g + 0.114 * rgb.b);
	cb = static_cast<short>(-0.168736 * rgb.r - 0.331264 * rgb.g + 0.5 * rgb.b);
	cr = static_cast<short>(0.5 * rgb.r - 0.418688 * rgb.g - 0.081312 * rgb.b);
}
