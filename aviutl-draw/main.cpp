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
#include "interpolate.h"

using Number = double;

static Image dest;
static composite::Composite compositeMode = composite::sourceOver;
static blend::Blend blendMode = blend::normal;
static interpolate::Interpolate<Number> interpolateFunc = interpolate::bilinear;

int version(lua_State* L) {
	lua_pushstring(L, "0.1.0beta1");
	return 1;
}

int clear(lua_State* L) {
	if (lua_gettop(L) < 2) {
		dest.clear();
	}
	else {
		int w = lua_tointeger(L, 1);
		int h = lua_tointeger(L, 1);
		dest.clear(w, h);
	}
	return 0;
}

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

	if (lua_isnumber(L, 1)) {
		blendMode = blend::toBlend(lua_tointeger(L, 1));
	}
	else if (lua_isstring(L, 1)) {
		blendMode = blend::toBlend(lua_tostring(L, 1));
	}
	return 0;
}

int setInterpolate(lua_State* L) {
	if (lua_gettop(L) < 1) {
		return luaL_error(L, "setInterpolate() require 1 arg");
	}

	switch (lua_tointeger(L, 1)) {
	case 0:
		interpolateFunc = interpolate::nearestNeighbor;
		break;
	case 1:
		interpolateFunc = interpolate::bilinear;
		break;
	}
	return 0;
}

BGRA blendColor(BGRA pd, BGRA ps, Number alpha) {
	ps.a = static_cast<uint8_t>(ps.a * alpha);
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

	BGRA p(
		static_cast<uint8_t>(b),
		static_cast<uint8_t>(g),
		static_cast<uint8_t>(r),
		static_cast<uint8_t>(a)
	);
	return p;
}

// draw(data,w,h, ox,oy,zoom,alpha,rotate)
int draw(lua_State* L) {
	const int argn = lua_gettop(L);
	if (argn < 3) {
		return luaL_error(L, "draw() require 3 args");
	}

	const ReadOnlyImage src(
		static_cast<BGRA*>(lua_touserdata(L, 1)),
		lua_tointeger(L, 2),
		lua_tointeger(L, 3)
	);
	const int ox = (argn >= 4) ? lua_tointeger(L, 4) : 0;
	const int oy = (argn >= 5) ? lua_tointeger(L, 5) : 0;
	Number zoom = static_cast<Number>((argn >= 6) ? lua_tonumber(L, 6) : 1);
	Number alpha = static_cast<Number>((argn >= 7) ? lua_tonumber(L, 7) : 1);
	Number rotate = static_cast<Number>((argn >= 8) ? lua_tonumber(L, 8) : 0);

	if (zoom < 0) return 0;
	alpha = std::clamp(alpha, static_cast<Number>(0), static_cast<Number>(1));
	rotate = rotate / 180 * std::numbers::pi;

	Mat<Number> mat;
	mat.translate(-src.width * 0.5, -src.height * 0.5);
	mat.scale(zoom, zoom);
	mat.rotate(rotate);
	mat.translate(dest.width * 0.5 + ox, dest.height * 0.5 + oy);
	Mat<Number> inv = mat.inverse();

	Vec2<Number> pts[4] = {
		mat.transform(Vec2<Number>{ 0,0 }),
		mat.transform(Vec2<Number>{ static_cast<Number>(src.width),0 }),
		mat.transform(Vec2<Number>{ static_cast<Number>(src.width),static_cast<Number>(src.height) }),
		mat.transform(Vec2<Number>{ 0,static_cast<Number>(src.height) }),
	};
	int sx = pts[0].x, sy = pts[0].y;
	int ex = sx, ey = sy;
	for (int i = 1; i < 4; i++) {
		if (pts[i].x < sx) sx = pts[i].x;
		else if (pts[i].x > ex) ex = pts[i].x;

		if (pts[i].y < sy) sy = pts[i].y;
		else if (pts[i].y > ey) ey = pts[i].y;
	}

	if (sx < 0) sx = 0;
	if (ex >= dest.width) ex = dest.width;
	if (sy < 0) sy = 0;
	if (ey >= dest.height) ey = dest.height;

	for (int y = sy; y < ey; y++) {
		for (int x = sx; x < ex; x++) {
			Vec2<Number> point = inv.transform(Vec2<Number>{
				static_cast<Number>(x), static_cast<Number>(y) });
			auto ps = interpolateFunc(src, point);
			auto pd = dest.getPixel(x, y);
			auto p = blendColor(pd, ps, alpha);
			dest.setPixel(x, y, p);
		}
	}

	return 0;
}

// drawPerspective(data,w,h,x0,y0,...,x3,y3,u0,v0,...,u3,v3,alpha)
int drawPerspective(lua_State* L) {
	const int argn = lua_gettop(L);
	if (argn < 19) {
		return luaL_error(L, "drawPerspective() require 19 args");
	}

	const ReadOnlyImage src(
		static_cast<BGRA*>(lua_touserdata(L, 1)),
		lua_tointeger(L, 2),
		lua_tointeger(L, 3)
	);
	Vec2<Number> xy[4] = {
		{lua_tonumber(L, 4) + dest.width / 2, lua_tonumber(L, 5) + dest.height / 2},
		{lua_tonumber(L, 6) + dest.width / 2, lua_tonumber(L, 7) + dest.height / 2},
		{lua_tonumber(L, 8) + dest.width / 2, lua_tonumber(L, 9) + dest.height / 2},
		{lua_tonumber(L, 10) + dest.width / 2, lua_tonumber(L, 11) + dest.height / 2},
	};
	Vec2<Number> uv[4] = {
		{lua_tonumber(L, 12), lua_tonumber(L, 13)},
		{lua_tonumber(L, 14), lua_tonumber(L, 15)},
		{lua_tonumber(L, 16), lua_tonumber(L, 17)},
		{lua_tonumber(L, 18), lua_tonumber(L, 19)},
	};
	Number alpha = static_cast<Number>((argn >= 20) ? lua_tonumber(L, 20) : 1);
	
	Mat<double> mat;
	getPerspective(uv, xy, mat);

	int sx = xy[0].x, sy = xy[0].y;
	int ex = sx, ey = sy;
	for (int i = 1; i < 4; i++) {
		if (xy[i].x < sx) sx = xy[i].x;
		else if (xy[i].x > ex) ex = xy[i].x;

		if (xy[i].y < sy) sy = xy[i].y;
		else if (xy[i].y > ey) ey = xy[i].y;
	}
	if (sx < 0) sx = 0;
	if (ex >= dest.width) ex = dest.width;
	if (sy < 0) sy = 0;
	if (ey >= dest.height) ey = dest.height;

	for (int y = sy; y < ey; y++) {
		for (int x = sx; x < ex; x++) {
			Vec2<Number> pt{ static_cast<Number>(x), static_cast<Number>(y) };
			if (cross(xy[0], pt, xy[1]) < 0
				&& cross(xy[1], pt, xy[2]) < 0
				&& cross(xy[2], pt, xy[3]) < 0
				&& cross(xy[3], pt, xy[0]) < 0)
			{
				Vec2<Number> point = mat.mapPerspective(pt);
				auto ps = interpolateFunc(src, point);
				auto pd = dest.getPixel(x, y);
				auto p = blendColor(pd, ps, alpha);
				dest.setPixel(x, y, p);
			}
		}
	}

	return 0;
}

static luaL_Reg functions[] = {
	{"version", version},
	{"clear", clear},
	{"setimage", setImage},
	{"getimage", getImage},
	{"setcomposite", setComposite},
	{"setblend", setBlend},
	{"setinterpolate", setInterpolate},
	{"draw", draw},
	{"drawperspective", drawPerspective},
	{nullptr, nullptr},
};

extern "C" __declspec(dllexport) int luaopen_KaroterraDraw(lua_State * L) {
	luaL_register(L, "KaroterraDraw", functions);
	return 1;
}
