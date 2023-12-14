#include "Lines.h"
#include "IntPairs.h"
#include <print>
#include <vector>
#include <ranges> // std::views::as_rvalue
#include <map>

int day14(const char* filename) {
	std::vector<std::string> map{ std::from_range, std::views::as_rvalue(lines(filename)) };
	const int n = int(map.size());
	const int m = int(map[0].size());
	// This is sum(IntPairs | filter | transform), but it's
	// unusable without structured bindings in function arguments
	int sum = 0;
	for (auto [i, j] : IntPairs(n, m)) {
		if (map[i][j] == 'O') {
			while (i > 0 && map[i - 1][j] == '.') {
				std::swap(map[i][j], map[i - 1][j]);
				--i;
			}
			sum += (n - i);
		}
	}
	return sum;
}

int day14_2(const char* filename) {
	std::vector<std::string> map{ std::from_range, std::views::as_rvalue(lines(filename)) };
	const int n = int(map.size());
	const int m = int(map[0].size());

	std::map<decltype(map), int> cache{{ map,0 }};
	bool found = false;
	for (int cycle = 1; cycle <= 1'000'000'000; ++cycle) {
		const int offsets[4][2] = {{-1,0},{0,-1},{1,0},{0,1}}; // N W S E
		for (const auto [di, dj] : offsets) {
			for (const auto [ii, jj] : IntPairs(n, m)) {
				// The position of the rock to be moved at this step
				int i = (di <= 0 ? ii : n - 1 - ii);
				int j = (dj <= 0 ? jj : m - 1 - jj);
				if (map[i][j] == 'O') {
					while (i + di >= 0 && i + di < n && j + dj >= 0 && j + dj < m && map[i + di][j + dj] == '.') {
						std::swap(map[i][j], map[i + di][j + dj]);
						i += di; j += dj;
					}
				}
			}
		}
		// If a cycle is found, skip ahead by the biggest multiple of the cycle lengt
		const auto [it, inserted] = cache.insert(std::make_pair(map, cycle));
		if (!found && !inserted) {
			const int cycleLen = cycle - it->second;
			cycle += (((1'000'000'000 - cycle) / cycleLen) * cycleLen);
			found = true; // Finish the remaining cycles without trying to skip ahead
		}
	}
	int sum = 0;
	for (const auto [i, j] : IntPairs(n, m)) {
		if (map[i][j] == 'O') {
			sum += (n - i);
		}
	}
	return sum;
}

int main14() {
	std::println("{}", day14("input/14test.txt")); // 136
	std::println("{}", day14("input/14full.txt")); // 106378
	std::println("{}", day14_2("input/14test.txt")); // 64
	std::println("{}", day14_2("input/14full.txt")); // 90795
	return 0;
}
