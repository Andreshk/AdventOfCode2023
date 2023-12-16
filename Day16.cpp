#include "Lines.h"
#include "IntPairs.h"
#include <print>
#include <queue>
#include <vector>
#include <ranges> // std::views::{as_rvalue,iota,transform}
#include <algorithm> // std::ranges::{count,max}

struct Beam { IntPair pos; int dir; };

int run(Beam b, const std::vector<std::string>& map) {
	const int n = int(map.size());
	const int m = int(map[0].size());
	std::vector<std::vector<int>> visited(n, std::vector(m, 0));
	std::queue<Beam> beams;
	beams.push(b);
	while (!beams.empty()) {
		const IntPair offs[4] = {{0,1},{1,0},{0,-1},{-1,0}};
		auto [pos, dir] = beams.front(); beams.pop();
		while (pos.i >= 0 && pos.i < n && pos.j >= 0 && pos.j < m && (visited[pos.i][pos.j] & (1 << dir)) == 0) {
			// Mark as visited in this dir
			visited[pos.i][pos.j] |= (1 << dir);
			// Update dir and split if needed
			switch (map[pos.i][pos.j]) {
			case '\\': { dir = (dir < 2 ? 1 : 5) - dir; } break;
			case '/': { dir = 3 - dir; } break;
			case '|': if (dir % 2 == 0) {
				beams.emplace(pos + offs[1], 1);
				dir = 3;
			} break;
			case '-': if (dir % 2 != 0) {
				beams.emplace(pos + offs[0], 0);
				dir = 2;
			} break;
			}
			// Move in the selected dir
			pos += offs[dir];
		}
	}
	return ranges::sum(visited, [&](const auto& row) { return m - int(std::ranges::count(row, 0)); });
}

void day16(const char* filename) {
	const std::vector<std::string> map{std::from_range, std::views::as_rvalue(lines(filename))};
	const int n = int(map.size());
	const int m = int(map[0].size());

	const int part1 = run({0,0,0}, map);
	const int part2 = std::max(
		std::ranges::max(std::views::iota(0, n)
			| std::views::transform([&](int i) { return std::max(run({i,0,0}, map), run({i,n-1,2}, map)); })),
		std::ranges::max(std::views::iota(0, m)
			| std::views::transform([&](int j) { return std::max(run({0,j,1}, map), run({m-1,j,3}, map)); })));
	std::println("{} {}", part1, part2);
}

int main16() {
	day16("input/16test.txt"); // 46 51
	day16("input/16full.txt"); // 7517 7741
	return 0;
}
