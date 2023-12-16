#include "Lines.h"
#include "IntPairs.h"
#include <print>
#include <vector>
#include <ranges> // std::views::{as_rvalue,iota,transform}
#include <algorithm> // std::ranges::{count,max}

void day16(const char* filename) {
	const std::vector<std::string> map{std::from_range, std::views::as_rvalue(lines(filename))};
	const int n = int(map.size());
	const int m = int(map[0].size());

	auto count = [&](IntPair pos, int dir) {
		std::vector<std::vector<int>> visited(n, std::vector(m, 0));
		// Recursive lambda to handle the recursive beam tracing
		auto run = [&](this auto&& self, IntPair pos, int dir) -> void {
			const IntPair offs[4] = {{0,1},{1,0},{0,-1},{-1,0}};
			while (true) {
				if (pos.i < 0 || pos.i >= n || pos.j < 0 || pos.j >= m || (visited[pos.i][pos.j] & (1 << dir)) != 0) {
					break;
				}
				// Mark as visited in this dir
				visited[pos.i][pos.j] |= (1 << dir);
				// Update dir and split if needed
				switch (map[pos.i][pos.j]) {
				case '\\': { dir = (dir < 2 ? 1 : 5) - dir; } break;
				case '/': { dir = 3 - dir; } break;
				case '|': if (dir % 2 == 0) {
					self(pos + offs[1], 1);
					dir = 3;
				} break;
				case '-': if (dir % 2 != 0) {
					self(pos + offs[0], 0);
					dir = 2;
				} break;
				}
				// Move in the selected dir
				pos += offs[dir];
			}
		};
		run(pos, dir);
		return ranges::sum(visited, [&](const auto& row) { return m - int(std::ranges::count(row, 0)); });
	};

	const int part1 = count({0,0}, 0);
	const int part2 = std::max(
		std::ranges::max(std::views::iota(0, n)
			| std::views::transform([&](int i) { return std::max(count({i,0}, 0), count({i,n - 1}, 2)); })),
		std::ranges::max(std::views::iota(0, m)
			| std::views::transform([&](int j) { return std::max(count({0,j}, 1), count({m - 1,j}, 3)); })));
	std::println("{} {}", part1, part2);
}

int main16() {
	day16("input/16test.txt"); // 46 51
	day16("input/16full.txt"); // 7517 7741
	return 0;
}
