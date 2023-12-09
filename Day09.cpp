#include "Lines.h"
#include <scn/scan.h>
#include <print>
#include <vector>
#include <algorithm> // std::ranges::all_of

void day09(const char* filename) {
	int part1 = 0;
	int part2 = 0;
	for (std::string_view line : lines(filename)) {
		auto xs = scanList<int>(line);
		std::vector<int> firsts;
		for (bool even = true; !std::ranges::all_of(xs, [](int x) { return x == 0; }); even = !even) {
			part1 += xs.back();
			part2 += (even ? xs.front() : -xs.front());
			// xs.assign_range(xs | pairwise_transform(minus) | transform(negate)) if it didn't invalidate iterators :(
			for (int i = 0; i < xs.size() - 1; ++i) {
				xs[i] = xs[i + 1] - xs[i];
			}
			xs.pop_back();
		}
	}
	std::println("{} {}", part1, part2);
}

int main09() {
	day09("input/9test.txt"); //114 2
	day09("input/9full.txt"); //1479011877 973
	return 0;
}
