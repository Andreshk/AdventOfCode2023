#include "Lines.h"
#include <print>
#include <algorithm> // std::ranges::{find_if,find_last_if}
#include <ranges> // std::views::{iota,transform}

int part1(const char* filename) {
	return ranges::sum(lines(filename), [&](const std::string_view line) {
		const int dig1 = *std::ranges::find_if(line, [](char c) { return std::isdigit(c); }) - '0';
		const int dig2 = *std::ranges::find_last_if(line, [](char c) { return std::isdigit(c); }).begin() - '0';
		return (dig1 * 10 + dig2);
	});
}

int part2(const char* filename) {
	const std::array<std::string_view, 10> digits{ "zero", "one", "two", "three", "four", "five", "six", "seven", "eight", "nine" };
	return ranges::sum(lines(filename), [&](const std::string_view line) {
		auto findF = [&](const int d) -> size_t { // Use npos as a valid maximum value
			return std::min(line.find(char('0' + d)), line.find(digits[d]));
		};
		auto findR = [&](const int d) -> int64_t { // Cast to signed to turn npos into -1, a valid minimum value
			return std::max<int64_t>(line.rfind(char('0' + d)), line.rfind(digits[d]));
		};
		// Elegant, but unefficient: does not cache the result of find*
		//const int dig1 = std::ranges::min(std::views::iota(0, 10), std::less<>{}, findF);
		//const int dig2 = std::ranges::max(std::views::iota(0, 10), std::less<>{}, findR);
		const int dig1 = std::ranges::min(
			std::views::iota(0, 10) | std::views::transform([&](const int d) { return std::pair{ d, findF(d) }; }),
			std::less<>{}, &std::pair<int, size_t>::second
		).first;
		const int dig2 = std::ranges::max(
			std::views::iota(0, 10) | std::views::transform([&](const int d) { return std::pair{ d, findR(d) }; }),
			std::less<>{}, &std::pair<int, int64_t>::second
		).first;
		return (dig1 * 10 + dig2);
	});
}

int main01() {
	//std::println("{}", part1("input/1test.txt"));
	std::println("{}", part1("input/1full.txt")); // 55208
	std::println("{}", part2("input/1test.txt")); // 281
	std::println("{}", part2("input/1full.txt")); // 54578
	return 0;
}
