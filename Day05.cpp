#include <scn/scan.h>
#include <fstream>
#include <string>
#include <print>
#include <ranges> // std::views::*
#include <vector>
#include <algorithm> // std::ranges::{sort,min}

// Almanac map entry
struct Entry {
	int64_t begin, end, off;
};

std::pair<std::vector<int64_t>, std::array<std::vector<Entry>, 7>> parseInput(const char* filename) {
	std::ifstream file{ filename };
	std::string line;

	std::getline(file, line);
	auto seeds = std::views::split(line.substr(7), ' ') // skip "seeds: " part
		| std::views::transform([&](auto rng) { return scn::scan<int64_t>(rng, "{}")->value(); })
		| std::ranges::to<std::vector>();
	std::getline(file, line); // empty line, will be discarded
	std::array<std::vector<Entry>, 7> maps{};
	for (int i = 0; i < 7; ++i) {
		std::getline(file, line); // this is text, will be discarded
		assert(!line.empty() && !std::isdigit(line[0]));
		while (true) {
			std::getline(file, line);
			if (!file || line.empty()) {
				break;
			}
			const auto [dest, src, count] = scn::scan<int64_t, int64_t, int64_t>(line, "{} {} {}")->values();
			maps[i].emplace_back(src, src + count, dest - src); // switch to [begin;end) + offset
		}
		// Helps for part2
		std::ranges::sort(maps[i], std::less<>{}, &Entry::begin);
	}
	return { std::move(seeds), std::move(maps) };
}

int64_t day05(const char* filename) {
	const auto [seeds, maps] = parseInput(filename);
	auto remap = [&](int64_t idx) {
		for (const auto& stage : maps) {
			// to-do: why not lower_bound over the begin's?
			const auto it = std::ranges::find_if(stage, [&](const Entry& m) { return (idx >= m.begin && idx < m.end); });
			if (it != stage.end()) {
				idx += it->off;
			}
		}
		return idx;
	};
	return std::ranges::min(seeds | std::views::transform(remap));
}

int64_t day05_2(const char* filename) {
	const auto [seeds, maps] = parseInput(filename);
	using Pair = std::pair<int64_t, int64_t>; // [from;to) interval
	auto remap = [&](Pair p) {
		std::vector<Pair> currs{ p };
		for (const auto& stage : maps) {
			std::vector<Pair> nexts;
			for (const auto [from, to] : currs) {
				for (const auto [begin, end, off] : stage) {
					const int64_t from2 = std::max(from, begin);
					const int64_t to2 = std::min(to, end);
					if (from2 < to2) { // If the intersection is valid, offset it & add to the list
						nexts.emplace_back(from2 + off, to2 + off);
					}
				}
				// Add the unmapped parts of the interval - assumes there are no other gaps (!)
				if (from < stage[0].begin) {
					nexts.emplace_back(from, std::min(to, stage[0].begin));
				}
				if (to > stage.back().end) {
					nexts.emplace_back(std::max(from, stage.back().end), to);
				}
			}
			currs = std::move(nexts);
		}
		return currs;
	};
	return std::ranges::min(seeds
		| std::views::chunk(2)
		| std::views::transform([](auto&& chunk) { return Pair(chunk[0], chunk[0] + chunk[1]); })
		| std::views::transform(remap)
		| std::views::transform([](auto&& v) {
			return (v.empty() ? INT64_MAX : std::ranges::min(v | std::views::transform(&Pair::first)));
		}));
}

int main05() {
	std::println("{}", day05("input/5test.txt")); // 35
	std::println("{}", day05("input/5full.txt")); // 173706076
	std::println("{}", day05_2("input/5test.txt")); // 46
	std::println("{}", day05_2("input/5full.txt")); // 11611182
	return 0;
}