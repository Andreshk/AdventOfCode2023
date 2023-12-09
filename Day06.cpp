#include "Lines.h"
#include <scn/scan.h>
#include <print>
#include <vector>
#include <ranges> // std::views::{filter,transform,iota,zip_transform}
#include <algorithm> // std::ranges::{lower_bound,to}
#include <numeric> // std::midpoint

std::pair<std::vector<int>, int64_t> parse(std::string_view str) {
	str.remove_prefix(10);
	std::vector<int> xs = scanList<int>(str);
	const std::string digits = str | std::views::filter(isdigit) | std::ranges::to<std::string>();
	return { std::move(xs), scn::scan<int64_t>(digits, "{}")->value() };
}

void day06(const char* filename) {
	std::ifstream file{ filename };
	std::string line;
	std::getline(file, line);
	const auto [times, totalTime] = parse(line);
	std::getline(file, line);
	const auto [dists, totalDist] = parse(line);

	auto solve = [](const int64_t t, const int64_t d) {
		const auto ints = std::views::transform(std::views::iota(0ll, t / 2), [=](int64_t i) { return (t - i) * i - d; });
		const int64_t x = *std::ranges::lower_bound(ints, 1).base(); // >0 to win <=> look for >=1
		return (t - 2 * x + 1);
	};
	const auto part1 = ranges::product(std::views::zip_transform(solve, times, dists));
	const auto part2 = solve(totalTime, totalDist);
	std::println("{} {}", part1, part2);
}

int main06() {
	day06("input/6test.txt"); // 288 71503
	day06("input/6full.txt"); // 220320 34454850
	return 0;
}