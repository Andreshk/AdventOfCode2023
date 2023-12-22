#include "Lines.h"
#include "IntPairs.h"
#include <scn/scan.h>
#include <print>

#include <vector>
#include <ranges>
#include <algorithm>
#include <queue>

struct Brick {
	IntPair x, y, z;
};

void day22(const char* filename) {
	std::vector<Brick> bricks;
	for (std::string_view line : lines(filename)) {
		const auto [xa,ya,za,xb,yb,zb] = scn::scan<int,int,int,int,int,int>(line, "{},{},{}~{},{},{}")->values();
		assert(xa == xb && ya == yb || xa == xb && za == zb && ya == yb || za == zb);
		assert(xa <= xb && ya <= yb && za <= zb);
		assert(xa >= 0 && xb <= 9 && ya >= 0 && yb <= 9);
		bricks.push_back({IntPair(xa, xb + 1), IntPair(ya, yb + 1), IntPair(za, zb + 1)});
	}
	const int zmax = std::ranges::max(bricks | std::views::transform([](Brick b) { return b.z.j; }));
	using Slice = std::array<std::array<int, 10>, 10>;
	std::vector<Slice> map(zmax);

	auto free = [&](const Brick& b) {
		for (int i = b.x.i; i < b.x.j; ++i) {
			for (int j = b.y.i; j < b.y.j; ++j) {
				for (int k = b.z.i; k < b.z.j; ++k) {
					if (map[k][i][j] != 0) { return false; }
				}
			}
		}
		return true;
	};
	// Place bricks from lowest to highest
	std::ranges::sort(bricks, std::less<>{}, [](Brick b) { return b.z.i; });
	for (auto&& [idx, b] : std::views::enumerate(bricks)) {
		assert(free(b)); // b can be placed where it starts
		while (b.z.i > 0 && free(Brick{b.x,b.y,b.z - IntPair{1,1}})) {
			b.z -= {1, 1};
		}
		for (int i = b.x.i; i < b.x.j; ++i) {
			for (int j = b.y.i; j < b.y.j; ++j) {
				for (int k = b.z.i; k < b.z.j; ++k) {
					map[k][i][j] = int(idx) + 1;
				}
			}
		}
	}
	// Build a graph for each brick -> the ones it supports
	std::vector<std::vector<int>> neighbs(bricks.size());
	for (auto&& [idx, b] : std::views::enumerate(bricks)) {
		// check each square above
		for (int i = b.x.i; i < b.x.j; ++i) {
			for (int j = b.y.i; j < b.y.j; ++j) {
				const int up = map[b.z.j][i][j] - 1;
				assert(up != idx);
				if (up >= 0) {
					neighbs[idx].push_back(up);
				}
			}
		}
	}
	// Remove duplicates before counting
	for (std::vector<int>& ns : neighbs) {
		std::ranges::sort(ns);
		const auto mid = std::ranges::unique(ns).begin();
		ns.erase(mid, ns.end());
	}
	// vertex in-degrees
	std::vector<int> ins(bricks.size(), 0);
	for (const auto& ns : neighbs) {
		for (const int v : ns) {
			++ins[v];
		}
	}
	// Find out how many bricks will fall if u is removed (other than u)
	auto drop = [&](int u) {
		std::queue<int> q; q.push(u);
		std::vector<int> preds(bricks.size(), 0);
		int res = 0;
		while (!q.empty()) {
			u = q.front(); q.pop();
			for (int v : neighbs[u]) {
				if (++preds[v] == ins[v]) {
					q.push(v); ++res;
				}
			}
		}
		return res;
	};
	// Try removing each brick that would trigger any reaction
	int part1 = 0;
	int part2 = 0;
	for (auto&& [i, ns] : std::views::enumerate(neighbs)) {
		if (!ns.empty() && std::ranges::any_of(ns, [&](int v) { return ins[v] == 1; })) {
			const int fallen = drop(int(i));
			part1 += (fallen != 0); // Any reaction
			part2 += fallen;
		}
	}
	std::println("{} {}", bricks.size() - part1, part2);
}

int main22() {
	day22("input/22test.txt"); // 5 7
	day22("input/22full.txt"); // 451 66530
	return 0;
}
