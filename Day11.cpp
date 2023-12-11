#include "Lines.h"
#include "IntPairs.h"
#include <print>
#include <vector>
#include <ranges> // std::views::{as_rvalue,filter,transform,iota}
#include <algorithm> // std::ranges::none_of
#include <numeric> // std::inclusive_scan
#include <initializer_list>

void day11(const char* filename, std::initializer_list<int> ages) {
	const std::vector<std::string> map{ std::from_range, std::views::as_rvalue(lines(filename)) };
	const int n = int(map.size());
	const int m = int(map[0].size());

	// Build prefix sums over the indices of the empty rows & cols
	std::vector<int> rows = std::views::iota(0, n)
		| std::views::transform([&](int i) { return int(!map[i].contains('#')); })
		| std::ranges::to<std::vector>();
	std::inclusive_scan(rows.begin(), rows.end(), rows.begin()); // Y NO ranges::inclusive_scan

	std::vector<int> cols = std::views::iota(0, m)
		| std::views::transform([&](int j) { return int(std::ranges::none_of(map, [&](auto&& row) { return row[j] == '#'; })); })
		| std::ranges::to<std::vector>();
	std::inclusive_scan(cols.begin(), cols.end(), cols.begin());

	const std::vector<IntPair> pos = IntPairs(n, m)
		| std::views::filter([&](IntPair p) { return map[p.i][p.j] == '#'; })
		| std::ranges::to<std::vector>();

	for (const int age : ages) {
		// pos can be transformed in-place for any single age
		const std::vector<IntPair> pos2 = pos
			| std::views::transform([&](IntPair p) { return p + IntPair{rows[p.i] * (age - 1), cols[p.j] * (age - 1)}; })
			| std::ranges::to<std::vector>();
		const int k = int(pos2.size());
		const int64_t res = ranges::sum(OrderedIntPairs(k),
			[&](IntPair idx) {
				const auto& [i1, j1] = pos2[idx.i];
				const auto& [i2, j2] = pos2[idx.j];
				return int64_t(std::abs(i1 - i2) + std::abs(j1 - j2));
			}
		);
		std::print("{} ", res);
	}
	std::println("");
}

int main11() {
	day11("input/11test.txt", { 2,10,100 }); // 374 1030 8410
	day11("input/11full.txt", { 2,1000000 }); // 9623138 726820169514
	return 0;
}
