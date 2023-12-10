#include "Lines.h"
#include <print>
#include <vector>
#include <ranges> // std::views::as_rvalue
#include <algorithm> // std::ranges::{find_if,count_if}

bool in(const char cs[3], char c) {
	return (c == cs[0] || c == cs[1] || c == cs[2]);
}

void day10(const char* filename) {
	std::vector<std::string> map{std::from_range, std::views::as_rvalue(lines(filename))};
	const int n = int(map.size());
	const int m = int(map[0].size());
	// Start coordinates
	int i = int(std::ranges::find_if(map, [](const std::string& str) { return str.contains('S'); }) - map.begin());
	int j = int(map[i].find('S'));
	// Finds the replacement of S
	auto rep = [&]() {
		const bool up = (i > 0 && in("|7F", map[i - 1][j]));
		const bool down = (i < n - 1 && in("|JL", map[i + 1][j]));
		const bool left = (j > 0 && in("-LF", map[i][j - 1]));
		const bool right = (j < m - 1 && in("-J7", map[i][j + 1]));
		if (up && down) { return '|'; }
		if (up && left) { return 'J'; }
		if (up && right) { return 'L'; }
		if (down && left) { return '7'; }
		if (down && right) { return 'F'; }
		if (left && right) { return '-'; }
		assert(false); return '-';
	};
	// Replace S with the correct symbol
	map[i][j] = rep();
	// Make a copy for the line only
	std::vector<std::string> map2(n, std::string(m, '.'));
	int part1 = 0;
	for (int steps = 1;; ++steps) {
		map2[i][j] = map[i][j];
		if (in("|7F", map[i][j]) && map[i + 1][j] != '.') {
			map[i][j] = '.';
			++i;
		} else if (in("-J7", map[i][j]) && map[i][j - 1] != '.') {
			map[i][j] = '.';
			--j;
		} else if (in("|JL", map[i][j]) && map[i - 1][j] != '.') {
			map[i][j] = '.';
			--i;
		} else if (in("-LF", map[i][j]) && map[i][j + 1] != '.') {
			map[i][j] = '.';
			++j;
		} else {
			part1 = steps / 2;
			break;
		}
	}
	// Scan each line, keeping track of "half-walls" up or down, e.g.
	// "LJ" cancel eachother (so we're out), but "L7" don't (and we're in)
	const auto part2 = ranges::sum(map2, [](const std::string& row) {
		return std::ranges::count_if(row, [u = false, d = false](char c) mutable {
			if (in("|JL", c)) { u ^= 1; }
			if (in("|7F", c)) { d ^= 1; }
			return (c == '.' && u && d);
		});
	});
	std::println("{} {}", part1, part2);
}

int main10() {
	day10("input/10test.txt"); // 8 1
	day10("input/10test2.txt"); // 23 4
	day10("input/10test3.txt"); // 70 8
	day10("input/10full.txt"); // 7030 285
	return 0;
}
