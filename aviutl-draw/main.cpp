#include <stdint.h>
#include <lua.hpp>
#include <vector>
#include <algorithm>
#include <numbers>
#include <Windows.h>

#include "mat.h"
#include "graphic.h"
#include "composite.h"
#include "blend.h"

static Image dest;
static composite::Composite compositeMode = composite::sourceOver;
static blend::Blend blendMode = blend::normal;

int setImage(lua_State* L) {
	if (lua_gettop(L) < 3) {
		return luaL_error(L, "setImage() require 3 args");
	}

	dest.setData(
		static_cast<BGRA*>(lua_touserdata(L, 1)),
		lua_tointeger(L, 2),
		lua_tointeger(L, 3)
	);
	return 0;
}

int getImage(lua_State* L) {
	lua_pushlightuserdata(L, dest.data.data());
	lua_pushinteger(L, dest.width);
	lua_pushinteger(L, dest.height);
	return 3;
}

int setComposite(lua_State* L) {
	if (lua_gettop(L) < 1) {
		return luaL_error(L, "setComposite() require 1 arg");
	}

	switch (lua_tointeger(L, 1)) {
	case 0:
		compositeMode = composite::clear;
		break;
	case 1:
		compositeMode = composite::copy;
		break;
	case 2:
		compositeMode = composite::destination;
		break;
	case 3:
		compositeMode = composite::sourceOver;
		break;
	case 4:
		compositeMode = composite::destinationOver;
		break;
	case 5:
		compositeMode = composite::sourceIn;
		break;
	case 6:
		compositeMode = composite::destinationIn;
		break;
	case 7:
		compositeMode = composite::sourceOut;
		break;
	case 8:
		compositeMode = composite::destinationOut;
		break;
	case 9:
		compositeMode = composite::sourceAtop;
		break;
	case 10:
		compositeMode = composite::destinationAtop;
		break;
	case 11:
		compositeMode = composite::exclusiveOR;
		break;
	case 12:
		compositeMode = composite::lighter;
		break;
	}
	return 0;
}

int setBlend(lua_State* L) {
	if (lua_gettop(L) < 1) {
		return luaL_error(L, "setBlend() require 1 arg");
	}

	switch (lua_tointeger(L, 1)) {
	case 0:
		blendMode = blend::normal;
		break;
	case 1:
		blendMode = blend::addition;
		break;
	case 2:
		blendMode = blend::subtract;
		break;
	case 3:
		blendMode = blend::multiply;
		break;
	case 4:
		blendMode = blend::screen;
		break;
	case 5:
		blendMode = blend::overlay;
		break;
	case 6:
		blendMode = blend::lighten;
		break;
	case 7:
		blendMode = blend::darken;
		break;
	case 8:
		blendMode = blend::luminosity;
		break;
	case 9:
		blendMode = blend::color;
		break;
	case 10:
		blendMode = blend::linearBurn;
		break;
	case 11:
		blendMode = blend::linearLight;
		break;
	case 12:
		blendMode = blend::difference;
		break;
	}
	return 0;
}

// draw(data,w,h, ox,oy,zoom,alpha,rotate)
int draw(lua_State* L) {
	const int argn = lua_gettop(L);
	if (argn < 3) {
		return luaL_error(L, "draw() require 3 args");
	}

	const Image src(
		static_cast<BGRA*>(lua_touserdata(L, 1)),
		lua_tointeger(L, 2),
		lua_tointeger(L, 3)
	);
	const int ox = (argn >= 4) ? lua_tointeger(L, 4) : 0;
	const int oy = (argn >= 5) ? lua_tointeger(L, 5) : 0;
	float zoom = (argn >= 6) ? lua_tonumber(L, 6) : 1;
	float alpha = (argn >= 7) ? lua_tonumber(L, 7) : 1;
	float rotate = (argn >= 8) ? lua_tonumber(L, 8) : 0;

	if (zoom < 0) return 0;
	alpha = std::clamp(alpha, 0.f, 1.f);
	rotate = rotate / 180 * std::numbers::pi;

	Mat mat;
	mat.translate(-src.width * 0.5, -src.height * 0.5);
	mat.scale(zoom, zoom);
	mat.rotate(rotate);
	mat.translate(dest.width * 0.5 + ox, dest.height * 0.5 + oy);
	Mat inv = mat.inverse();

	Vec2 pt1 = mat.transform(Vec2{ 0,0 });
	Vec2 pt2 = mat.transform(Vec2{ (float)src.width,0 });
	Vec2 pt3 = mat.transform(Vec2{ (float)src.width,(float)src.height });
	Vec2 pt4 = mat.transform(Vec2{ 0,(float)src.height });
	int sx = pt1.x, sy = pt1.y;
	int ex = sx, ey = sy;
	if (pt2.x < sx) sx = pt2.x;
	else if (pt2.x > ex) ex = pt2.x;
	if (pt2.y < sy) sy = pt2.y;
	else if (pt2.y > ey) ey = pt2.y;
	if (pt3.x < sx) sx = pt3.x;
	else if (pt3.x > ex) ex = pt3.x;
	if (pt3.y < sy) sy = pt3.y;
	else if (pt3.y > ey) ey = pt3.y;
	if (pt4.x < sx) sx = pt4.x;
	else if (pt4.x > ex) ex = pt4.x;
	if (pt4.y < sy) sy = pt4.y;
	else if (pt4.y > ey) ey = pt4.y;

	if (sx < 0) sx = 0;
	if (ex >= dest.width) ex = dest.width;
	if (sy < 0) sy = 0;
	if (ey >= dest.height) ey = dest.height;

	for (int y = sy; y < ey; y++) {
		for (int x = sx; x < ex; x++) {
			Vec2 point = inv.transform(Vec2{ (float)x, (float)y });
			auto ps = src.samplePixel(point);
			auto pd = dest.getPixel(x, y);
			ps.a = (uint8_t)((float)ps.a * alpha);
			int fd, fs;
			compositeMode(pd, ps, fd, fs);

			int a = (pd.a * fd + ps.a * fs) / 255;
			auto px = blendMode(pd, ps);

			int r = (pd.a * px.r + (255 - pd.a) * ps.r) / 255;
			int g = (pd.a * px.g + (255 - pd.a) * ps.g) / 255;
			int b = (pd.a * px.b + (255 - pd.a) * ps.b) / 255;
			if (a == 0) {
				r = g = b = 0;
			}
			else {
				r = (pd.a * fd * pd.r + ps.a * fs * r) / (a * 255);
				g = (pd.a * fd * pd.g + ps.a * fs * g) / (a * 255);
				b = (pd.a * fd * pd.b + ps.a * fs * b) / (a * 255);
			}

			BGRA p{
				static_cast<uint8_t>(b),
				static_cast<uint8_t>(g),
				static_cast<uint8_t>(r),
				static_cast<uint8_t>(a),
			};
			dest.setPixel(x, y, p);
		}
	}

	return 0;
}

static luaL_Reg functions[] = {
	{"setimage", setImage},
	{"getimage", getImage},
	{"setcomposite", setComposite},
	{"setblend", setBlend},
	{"draw", draw},
	{nullptr, nullptr},
};

extern "C" __declspec(dllexport) int luaopen_KaroterraDraw(lua_State * L) {
	luaL_register(L, "KaroterraDraw", functions);
	return 1;
}
