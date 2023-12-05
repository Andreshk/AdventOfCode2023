#include <scn/scan.h>
#include <fstream>
#include <string>
#include <print>

#include <ranges>
#include <vector>
#include <algorithm>

struct Map {
	int64_t dest, src, count;
};

std::pair<std::vector<int64_t>, std::array<std::vector<Map>, 7>> parseInput(const char* filename) {
	std::ifstream file{ filename };
	std::string line;

	std::getline(file, line);
	auto seeds = std::views::split(line.substr(7), ' ') // skip "seeds: " part
		| std::views::transform([&](auto rng) { return scn::scan<int64_t>(rng, "{}")->value(); })
		| std::ranges::to<std::vector>();
	std::getline(file, line); // empty line, will be discarded
	std::array<std::vector<Map>, 7> maps{};
	for (int i = 0; i < 7; ++i) {
		std::getline(file, line); // this is text, will be discarded
		assert(!line.empty() && !std::isdigit(line[0]));
		while (true) {
			std::getline(file, line);
			if (!file || line.empty()) {
				break;
			}
			Map& m = maps[i].emplace_back();
			std::tie(m.dest, m.src, m.count) = scn::scan<int64_t, int64_t, int64_t>(line, "{} {} {}")->values();
		}
		// Helps for part2
		std::ranges::sort(maps[i], std::less<>{}, &Map::src);
	}
	return { std::move(seeds), std::move(maps) };
}

int64_t day05(const char* filename) {
	const auto [seeds, maps] = parseInput(filename);
	auto remap = [&](int64_t idx) {
		for (const auto& stage : maps) {
			// to-do: why not lower_bound over the src's?
			const auto it = std::ranges::find_if(stage, [&](const Map& m) { return (idx >= m.src && idx < m.src + m.count); });
			if (it != stage.end()) {
				idx += it->dest - it->src;
			}
		}
		return idx;
	};
	return std::ranges::min(seeds | std::views::transform(remap));
}

int64_t day05_2(const char* filename) {
	const auto [seeds, maps] = parseInput(filename);
	using Pair = std::pair<int64_t, int64_t>;
	auto remap = [&](Pair p) {
		std::vector<Pair> currs{ p };
		for (const auto& stage : maps) {
			std::vector<Pair> nexts;
			for (const auto [from, to] : currs) {
				for (const auto [dest, src, count] : stage) {
					auto remapSingle = [&](int64_t idx) { return dest + idx - src; };
					const int64_t from2 = std::max(remapSingle(from), dest);
					const int64_t to2 = std::min(remapSingle(from + to), dest + count);
					if (from2 < to2) {
						nexts.emplace_back(from2, to2 - from2);
					}
				}
				if (from < stage[0].src) {
					nexts.emplace_back(from, std::min(to, stage[0].src - from));
				}
				const int64_t last = stage.back().src + stage.back().count;
				if (from + to > last) {
					nexts.emplace_back(std::max(from, last), to - std::max(last - from, 0ll));
				}
			}
			currs = std::move(nexts);
		}
		return currs;
	};
	return std::ranges::min(seeds
		| std::views::chunk(2)
		| std::views::transform([](auto&& chunk) { return Pair(chunk[0], chunk[1]); })
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