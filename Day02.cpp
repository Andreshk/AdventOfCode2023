#include "Lines.h"
#include "scn/scan.h" // scnlib
#include <print>
#include <ranges> // std::views::split
#include <cassert>

using namespace std::string_view_literals;

void doIt(const char* filename) {
	int part1 = 0, part2 = 0;
	for (std::string_view line : lines(filename)) {
		const auto res = *scn::scan<int>(line, "Game {}: ");
		int minR = 0, minG = 0, minB = 0;
		bool ok = true;
		for (auto&& str : std::views::split(res.range(), "; "sv)) {
			for (auto&& piece : std::views::split(str, ", "sv)) {
				const auto [x, c] = scn::scan<int, char>(piece, "{} {}")->values();
				switch (c) {
				case 'r': { minR = std::max(minR, x); ok &= (x <= 12); break; }
				case 'g': { minG = std::max(minG, x); ok &= (x <= 13); break; }
				case 'b': { minB = std::max(minB, x); ok &= (x <= 14); break; }
				}
			}
		}
		if (ok) { part1 += res.value(); }
		part2 += (minR * minG * minB);
	}
	std::println("{} {}", part1, part2);
}

int main02() {
	doIt("input/2test.txt"); // 8 2286
	doIt("input/2full.txt"); // 2076 70950
	return 0;
}