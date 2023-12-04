#include "Lines.h"
#include "IntPairs.h"
#include <print>
#include <ranges> // std::views::*
#include <vector>
#include <algorithm> // std::ranges::{sort,count_if}

void day04_1(const char* filename) {
	const std::vector<std::string> cards{ std::from_range, std::views::as_rvalue(lines(filename)) };
	std::vector<int> counts(cards.size(), 1);
	const int part1 = ranges::sum(std::views::enumerate(cards), [&](const std::tuple<ptrdiff_t, std::string_view>& p) {
		auto&& [idx, line] = p; // No structured bindings for arguments yet :/
		const auto pos0 = line.find(':') + 2;
		const auto pos1 = line.find('|');
		// Parse a single number from the 1 or 2 digits pointed to by it
		auto parse = [&](auto it) {
			const int d1 = (*it == ' ' ? 0 : *it - '0');
			const int d2 = *(it + 1) - '0';
			return (d1 * 10 + d2);
		};
		// Parse an entire space-separated list
		auto parseList =
			std::views::chunk(3)
			| std::views::keys
			| std::views::transform(parse)
			| std::ranges::to<std::vector>();
		std::vector<int> lucky = parseList(line.substr(pos0, pos1 - pos0 - 1));
		std::vector<int> mine = parseList(line.substr(pos1 + 2));
		std::ranges::sort(lucky);
		const auto matches = std::ranges::count_if(mine, [&](int x) { return std::ranges::binary_search(lucky, x); });
		for (int i = 1; i <= matches; ++i) {
			counts[idx + i] += counts[idx];
		}
		return (1 << (matches - 1));
	});
	std::println("{} {}", part1, ranges::sum(counts));
}

int main04() {
	day04_1("input/4test.txt"); // 13 30
	day04_1("input/4full.txt"); // 27454 6857330
	return 0;
}