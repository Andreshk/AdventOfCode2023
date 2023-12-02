#include "Lines.h"
#include <print>
#include <cstdio> // sscanf_s
#include <ranges> // std::views::split
#include <cassert>

using namespace std::string_view_literals;

void doIt(const char* filename) {
	int part1 = 0, part2 = 0;
	for (std::string_view line : lines(filename)) {
		int id = -1;
		const int numParsed = sscanf_s(line.data(), "Game %d: ", &id);
		assert(id >= 1 && numParsed > 0);
		line.remove_prefix(line.find(':') + 2);
		int minR = 0, minG = 0, minB = 0;
		bool ok = true;
		for (auto&& str : std::views::split(line, "; "sv)) {
			for (auto&& piece : std::views::split(str, ", "sv)) {
				int x;
				char c;
				const int numParsed = sscanf_s(&*piece.begin(), "%d %c", &x, &c, 1);
				assert(numParsed > 0);
				switch (c) {
				case 'r': { minR = std::max(minR, x); ok &= (x <= 12); break; }
				case 'g': { minG = std::max(minG, x); ok &= (x <= 13); break; }
				case 'b': { minB = std::max(minB, x); ok &= (x <= 14); break; }
				}
			}
		}
		if (ok) { part1 += id; }
		part2 += (minR * minG * minB);
	}
	std::println("{} {}", part1, part2);
}

int main02() {
	doIt("input/2test.txt"); // 8 2286
	doIt("input/2full.txt"); // 2076 70950
	return 0;
}