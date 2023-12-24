#include "Lines.h"
#include "IntPairs.h"
#include <scn/scan.h>
#include <print>
#include <vector>
#include <ranges> // std::views::{iota,filter,cartesian_product}
#include <algorithm> // std::ranges::{count_if,all_of}

#include <__msvc_int128.hpp>
using int128 = std::_Signed128;
using Point = std::array<int128, 3>;
Point operator+(const Point& a, const Point& b) { return {a[0] + b[0], a[1] + b[1], a[2] + b[2]}; }
Point operator-(const Point& a, const Point& b) { return {a[0] - b[0], a[1] - b[1], a[2] - b[2]}; }
Point operator*(const int128 k, const Point& a) { return {k * a[0], k * a[1], k * a[2]}; }

int128 dot(const Point& a, const Point& b) {
	return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}
Point cross(const Point& a, const Point& b) {
	return { a[1]*b[2]-b[1]*a[2],
	         a[2]*b[0]-b[2]*a[0],
	         a[0]*b[1]-b[0]*a[1] };
}
struct Hail { Point p, v; };

// Solution idea: "adjust" all rays' directions by the rock's velocity
// => the task is reduced to whether all rays intersect at the same point.
// If so, this point is the solution (rock throwing location)

// Returns true if the adjusted rays intersect at integer barys
bool intersect(Hail h1, Hail h2, const Point adj, Point& res) {
	h1.v = h1.v - adj;
	h2.v = h2.v - adj;
	const Point dp = h2.p - h1.p;
	const Point v1v2 = cross(h1.v, h2.v);
	if (dot(dp, v1v2) != 0) { // Not coplanar
		return false;
	}
	const auto n = dot(cross(dp, h2.v), v1v2);
	const auto d = dot(v1v2, v1v2);
	if (d == 0 || n % d != 0) { // Integral intersections only
		return false;
	} else {
		res = h1.p + (n / d) * h1.v;
		return true;
	}
}

// Begins intersecting of two rays' projections in the XY plane & returns some intermediate data
std::array<int128, 3> intersectXY(const Point& p1, const Point& v1, const Point& p2, const Point& v2) {
	const int128 x1 = p1[0];
	const int128 y1 = p1[1];
	const int128 x2 = p1[0] + v1[0];
	const int128 y2 = p1[1] + v1[1];

	const int128 x3 = p2[0];
	const int128 y3 = p2[1];
	const int128 x4 = p2[0] + v2[0];
	const int128 y4 = p2[1] + v2[1];

	const int128 denom = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
	const int128 t = (x1 - x3) * (y3 - y4) - (y1 - y3) * (x3 - x4);
	const int128 u = (x1 - x3) * (y1 - y2) - (y1 - y3) * (x1 - x2);
	return {denom,t,u};
}

// Returns true if the XY projections of the adjusted rays intersect at integer barys
bool intersectXY(const Hail& h1, const Hail& h2, const Point adj, Point& res) {
	const auto [denom, t, u] = intersectXY(h1.p, h1.v - adj, h2.p, h2.v - adj);
	if (denom == 0 || t % denom != 0 || u % denom != 0) { // Integral intersections only
		return false;
	} else {
		res = h1.p + (t / denom) * h1.v;
		return true;
	}
}

void day24(const char* filename, const int64_t from, const int64_t to) {
	std::vector<Hail> hs;
	for (std::string_view line : lines(filename)) {
		// Can't have a '@' in the format string ???
		const auto res = scn::scan<int64_t, int64_t, int64_t, char, int64_t, int64_t, int64_t>(line, "{}, {}, {} {} {}, {}, {}");
		const auto& [px, py, pz, _, vx, vy, vz] = res->values();
		hs.emplace_back(Point{px,py,pz}, Point{vx,vy,vz});
	}
	const int n = int(hs.size());
	
	const auto part1 = std::ranges::count_if(OrderedIntPairs(n), [&](const IntPair& p) {
		const auto& [p1, v1] = hs[p.i];
		const auto& [p2, v2] = hs[p.j];
		const auto [denom, t, u] = intersectXY(p1, v1, p2, v2);
		auto cast = [](int128 x) { return double(int64_t(x)); };
		// Non-parallel && positive ray times only; non-integer results allowed (so no p1+t*v1)
		if (denom == 0 || t * denom < 0 || u * denom < 0) { return false; }
		const double px = cast(p1[0]) + cast(t) * cast(v1[0]) / cast(denom);
		const double py = cast(p1[1]) + cast(t) * cast(v1[1]) / cast(denom);
		return (px >= double(from) && px <= double(to) && py >= double(from) && py <= double(to));
	});
	std::print("{} ", part1);

	const auto vs = std::views::iota(-300,300);
	// First pass: to reduce the search space, find whether only the XY
	// projections of all adjusted rays intersect in the same point.
	auto xys = std::views::filter(std::views::cartesian_product(vs, vs), [&](const auto& xy) {
		const auto [vx, vy] = xy;
		const Point adj = {vx,vy,0};
		Point p = {};
		return intersectXY(hs[0], hs[1], adj, p)
			&& std::ranges::all_of(std::views::iota(2, n), [&](const int i) {
				Point pp{};
				return (intersectXY(hs[0], hs[i], adj, pp) && pp[0] == p[0] && pp[1] == p[1]);
			});
		});
	// Double-check by searching all possible Z-s only for the XY that pass
	for (const auto [vx, vy] : xys) {
		for (const int vz : vs) {
			const Point adj = {vx,vy,vz};
			Point p = {};
			const bool ok = intersect(hs[0], hs[1], adj, p)
				&& std::ranges::all_of(std::views::iota(2,n), [&](const int i) {
					Point pp{};
					return (intersect(hs[0], hs[i], adj, pp) && pp == p);
				});
			if (ok) {
				std::println("{}", int64_t(p[0] + p[1] + p[2]));
			}
		}
	}
}

int main24() {
	day24("input/24test.txt", 7, 27); // 2 47
	day24("input/24full.txt", 200000000000000ll, 400000000000000ll); // 25261 549873212220117
	return 0;
}
