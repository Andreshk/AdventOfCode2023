#include "Lines.h"
#include "IntPairs.h"
#include <print>
#include <vector>
#include <ranges> // std::views::as_rvalue
#include <algorithm> // std::ranges::{sort,unique}

// Interpolate a quadratic function & find f(x), given the values for x=[0,1,2] (hardcoded)
int64_t interp(const std::array<int64_t, 3>& ys, const int x) {
	//ys == {3744,33417,92680};
	const int64_t y0 = ys[0];
	const int64_t y01 = (ys[1] - ys[0]);
	const int64_t y02 = (ys[2] - 2 * ys[1] + ys[0]) / 2;
	return y0 + y01 * x + y02 * x * (x - 1);
}

int64_t day21(const char* filename, const int steps) {
	std::vector<std::string> map{std::from_range, std::views::as_rvalue(lines(filename))};
	const int n = int(map.size());
	const int m = int(map[0].size());

	const int si = int(std::ranges::find_if(map, [](const std::string& row) { return row.contains('S'); }) - map.begin());
	const int sj = int(map[si].find('S'));
	map[si][sj] = '.';

	const bool part2 = (steps > n);
	if (part2) { // Specially crafted part2 inputs only
		assert(n == m && si == sj && si == n / 2);
		assert((steps - si) % n == 0);
		assert(!map[si].contains('#'));
		assert(std::ranges::none_of(map, [&](const auto& row) { return row[sj] == '#'; }));
	}
	std::array<int64_t, 3> ys = {};

	std::vector<IntPair> level;
	level.emplace_back(si, sj);

	for (int i = 0; i < steps; ++i) {
		if ((i - si) % n == 0) {
			const int x = (i - si) / n;
			ys[x] = level.size();
			if (x == 2) { break; }
		}
		std::vector<IntPair> next;
		for (const auto [i, j] : level) {
			const int offsets[4][2] = {{0,1},{1,0},{0,-1},{-1,0}};
			for (const auto [di, dj] : offsets) {
				//if (i + di < 0 || i + di >= n || j + dj < 0 || j + dj >= m) { continue; }
				int pi = (i + di) % n; if (pi < 0) { pi += n; }
				int pj = (j + dj) % m; if (pj < 0) { pj += m; }
				if (map[pi][pj] == '.') {
					next.emplace_back(i + di, j + dj);
				}
			}
		}
		std::ranges::sort(next);
		const auto u = std::ranges::unique(next);
		next.erase(u.begin(), u.end());
		level = std::move(next);
	}
	// ys[x] = the tiles after 65*x+131 steps => interpolate to x = (26501365 - 65) / 131
	return (part2 ? interp(ys, (steps - si) / n) : level.size());
}

int main21() {
	// 605492680232051 -> high
	// 605492636939744 -> low
	std::println("{}", day21("input/21test.txt", 6)); // 16
	std::println("{}", day21("input/21full.txt", 64)); // 3660
	std::println("{}", day21("input/21full.txt", 26501365)); // 605492675373144
	return 0;
}
