#include "Lines.h"
#include "IntPairs.h"
#include <scn/scan.h>
#include <print>

int64_t day18(const char* filename, const bool part1) {
	const IntPair offs[4] = {{0,1},{1,0},{0,-1},{-1,0}}; // R D L U
	IntPair curr{0,0};
	int64_t border = 0;
	int64_t inside = 0;
	for (std::string_view line : lines(filename)) {
		const auto pos = line.find('#');
		const int k = (part1
			? scn::scan<int>(line.substr(2), "{}")->value()
			: scn::scan<int>(line.substr(pos + 1, 5), "{:x}")->value());
		const int dir = (part1
			? int(std::string_view{"RDLU"}.find(line[0]))
			: line[pos + 6] - '0');
		assert(dir >= 0 && dir < 4);
		const IntPair next = curr + (offs[dir] * k);
		border += k;
		inside += int64_t(curr.i) * next.j - int64_t(next.i) * curr.j; // Shoelace formula
		curr = next;
	}
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
