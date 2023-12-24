#include "Lines.h"
#include "IntPairs.h"
#include <scn/scan.h>
#include <print>

#include <vector>
#include <ranges>
#include <algorithm>

using Point = std::array<int64_t, 3>;
Point operator+(const Point& a, const Point& b) { return {a[0] + b[0], a[1] + b[1], a[2] + b[2]}; }
Point operator-(const Point& a, const Point& b) { return {a[0] - b[0], a[1] - b[1], a[2] - b[2]}; }
Point operator*(const int64_t k, const Point& a) { return {k * a[0], k * a[1], k * a[2]}; }

int64_t dot(const Point& a, const Point& b) {
	return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}
Point cross(const Point& a, const Point& b) {
	return { a[1]*b[2]-b[1]*a[2],
	         a[2]*b[0]-b[2]*a[0],
	         a[0]*b[1]-b[0]*a[1] };
}
// Returns true if the rays (p1,v1) and (p2,v2) intersect at integer barys
bool intersect(const Point& p1, const Point& v1, const Point& p2, const Point& v2, Point& res) {
	const Point dp = p2 - p1;
	const Point v1v2 = cross(v1, v2);
	if (dot(dp, v1v2) != 0) { // Not coplanar
		return false;
	}
	const auto n = dot(cross(dp, v2), v1v2);
	const auto d = dot(v1v2, v1v2);
	if (d == 0 || n % d != 0) { // Integral intersections only
		return false;
	} else {
		res = p1 + (n / d) * v1;
		return true;
	}
}

// Begins intersecting of two rays' projections in the XY plane & returns some intermediate data
std::array<int64_t, 3> intersectXY(const Point& p1, const Point& v1, const Point& p2, const Point& v2) {
	const int64_t x1 = p1[0];
	const int64_t y1 = p1[1];
	const int64_t x2 = p1[0] + v1[0];
	const int64_t y2 = p1[1] + v1[1];

	const int64_t x3 = p2[0];
	const int64_t y3 = p2[1];
	const int64_t x4 = p2[0] + v2[0];
	const int64_t y4 = p2[1] + v2[1];

	const int64_t denom = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
	const int64_t t = (x1 - x3) * (y3 - y4) - (y1 - y3) * (x3 - x4);
	const int64_t u = (x1 - x3) * (y1 - y2) - (y1 - y3) * (x1 - x2);
	return {denom,t,u};
}

// Returns true if the XY projections of the rays (p1,v1) and (p2,v2) intersect at integer barys
bool intersectXY(const Point& p1, const Point& v1, const Point& p2, const Point& v2, Point& res) {
	const auto [denom, t, u] = intersectXY(p1, v1, p2, v2);
	if (denom == 0 || t % denom != 0 || u % denom != 0) { // Integral intersections only
		return false;
	} else {
		res = p1 + (t / denom) * v1;
		return true;
	}
}

void day24(const char* filename, const int64_t from, const int64_t to) {
	struct Hail { Point p, v; };
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
		// Non-parallel && positive ray times only; non-integer results allowed (so no p1+t*v1)
		if (denom == 0 || double(t) / denom < 0 || double(u) / denom < 0) { return false; }
		const double px = double(p1[0]) + double(t) * (v1[0]) / denom;
		const double py = double(p1[1]) + double(t) * (v1[1]) / denom;
		return (px >= double(from) && px <= double(to) && py >= double(from) && py <= double(to));
	});
	std::print("{} ", part1);

	const auto vs = std::views::iota(-250,250);
	for (const auto [vx, vy] : std::views::cartesian_product(vs, vs)) {
		auto adjustXY = [&](const Point& v) { return v - Point{vx, vy, 0}; };
		Point p = {};
		const bool ok = intersectXY(hs[0].p, adjustXY(hs[0].v), hs[1].p, adjustXY(hs[1].v), p)
			&& std::ranges::all_of(std::views::iota(2, n), [&](const int i) {
				Point pp{};
				return (intersectXY(hs[0].p, adjustXY(hs[0].v), hs[i].p, adjustXY(hs[i].v), pp) && pp == p);
			});
		if (!ok) { continue; }
		// XY intersection passed, go to full
		for (const int vz : vs) {
			auto adjust = [&](const Point& v) { return v - Point{vx, vy, vz}; };
			Point p = {};
			const bool ok = intersect(hs[0].p, adjust(hs[0].v), hs[1].p, adjust(hs[1].v), p)
				&& std::ranges::all_of(std::views::iota(2,n), [&](const int i) {
					Point pp{};
					return (intersect(hs[0].p, adjust(hs[0].v), hs[i].p, adjust(hs[i].v), pp) && pp == p);
				});
			if (!ok) { continue; }
			std::println("{}", p[0] + p[1] + p[2]);
			return;
		}
	}
	std::println("FAIL!"); // :(
}

int main24() {
	day24("input/24test.txt", 7, 27); // 2 47
	day24("input/24full.txt", 200000000000000ll, 400000000000000ll); // 25261 549873212220117 :(
	return 0;
}
