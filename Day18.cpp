#include "Lines.h"
#include "IntPairs.h"
#include <scn/scan.h>
#include <print>
#include <vector>
#include <ranges> // std::views::pairwise_transform
#include <algorithm> // std::ranges::find

int64_t day18(const char* filename, const bool part1) {
	const IntPair offs[4] = {{0,1},{1,0},{0,-1},{-1,0}};
	std::vector<IntPair> coords;
	IntPair curr{0,0};
	coords.push_back(curr);
	int64_t border = 0;
	for (std::string_view line : lines(filename)) {
		const auto pos = line.find('#');
		const int k = (part1
			? scn::scan<int>(line.substr(2), "{}")->value()
			: scn::scan<int>(line.substr(pos + 1, 5), "{:x}")->value());
		const int dir = (part1
			? int(std::string_view{"RDLU"}.find(line[0]))
			: line[pos + 6] - '0');
		assert(dir >= 0 && dir < 4);
		curr += (offs[dir] * k);
		coords.push_back(curr);
		border += k;
	}
	// Instructions should give out a closed loop, required for flood filling and the Shoelace formula
	assert(coords.back() == coords.front());
	const int64_t inside = ranges::sum(std::views::pairwise_transform(coords,
		[](IntPair p1, IntPair p2) { return int64_t(p1.i) * p2.j - int64_t(p2.i) * p1.j; }));
	// The whole border is overcounting (we need +1, not +2)
	return (border/2 + std::abs(inside / 2) + 1);
}

int main18() {
	std::println("{}", day18("input/18test.txt", true)); // 62
	std::println("{}", day18("input/18full.txt", true)); // 61661
	std::println("{}", day18("input/18test.txt", false)); // 952408144115
	std::println("{}", day18("input/18full.txt", false)); // 111131796939729
	return 0;
}
