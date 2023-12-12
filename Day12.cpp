#include "Lines.h"
#include <scn/scan.h>
#include <print>
#include <vector>
#include <ranges> // std::views::{filter,transform,iota,chunk_by}
#include <algorithm> // std::ranges::count_if
#include <functional> // std::ranges::equal_to
#include <map>
#include <span>
#include <cassert>

int64_t matchSimple(const std::string& map, const std::vector<int>& counts) {
	const auto xs = std::views::iota(0, int(map.size()))
		| std::views::filter([&](int i) { return map[i] == '?'; })
		| std::ranges::to<std::vector>();
	const int k = int(xs.size());

	return std::ranges::count_if(std::views::iota(0u, 1u << k),
		[&](const unsigned bits) {
			std::string copy = map;
			for (int i = 0; i < k; ++i) {
				copy[xs[i]] = (bits & (1u << i)) ? '#' : '.';
			}
			const auto check = std::views::chunk_by(copy, std::ranges::equal_to{})
				| std::views::filter([](auto&& chunk) { return chunk[0] == '#'; })
				| std::views::transform([](auto&& chunk) { return int(chunk.size()); })
				| std::ranges::to<std::vector>();
			return (check == counts);
		});
}

struct Item {
	// to-do: find a way to keep a hash of the span instead of a copy of all values
	// and hope collisions don't happen (maybe keep the size, too)
	// or just compress into ~100 bytes or nibbles
	std::vector<int> xs;
	int strLen;
	bool started;

	friend auto operator<=>(const Item&, const Item&) = default;
};

int64_t match2(std::string_view str, std::span<int> xs, const bool started, std::map<Item, int64_t>& cache) {
	const auto [it, inserted] = cache.insert(std::make_pair(Item{{std::from_range,xs},int(str.size()),started}, 0ll));
	if (!inserted) {
		return it->second;
	} else if (str.empty()) {
		return (it->second = xs.empty() || (started && xs.size() == 1 && xs[0] == 0));
	} else if (xs.empty()) {
		return (it->second = !str.contains('#'));
	}
	int64_t sum = 0;
	// Next symbol is or can be a dot
	if (str[0] != '#' && !(started && xs[0] != 0)) {
		sum += match2(str.substr(1), xs.subspan(started), false, cache);
	}
	// Next symbol is or can be a hash
	if (str[0] != '.' && !(started && xs[0] == 0)) {
		assert(xs[0] > 0);
		--xs[0];
		sum += match2(str.substr(1), xs, true, cache);
		++xs[0];
	}
	// Cache the total & return
	return (it->second = sum);
}

int64_t day12(const char* filename, const int reps = 1) {
	return ranges::sum(lines(filename),
		[&](const std::string& line) {
			const auto pos = line.find(' ');
			std::string map = line.substr(0, pos);
			std::string counts1 = line.substr(pos + 1);
			for (int i = 0; i < reps - 1; ++i) {
				map += '?';
				map.append(line, 0, pos);
				counts1 += ',';
				counts1.append(line, pos + 1); // It's easier to duplicate a string, than a vector :)
			}
			auto counts = std::views::split(counts1, ',')
				| std::views::transform([](auto&& chunk) { return scn::scan<int>(chunk, "{}")->value(); })
				| std::ranges::to<std::vector>();

			// return matchSimple(map, counts);
			std::map<Item, int64_t> cache;
			return match2(map, counts, false, cache);
		});
}

int main12() {
	std::println("{}", day12("input/12test.txt")); // 21
	std::println("{}", day12("input/12full.txt")); // 6871
	std::println("{}", day12("input/12test.txt", 5)); // 525152
	std::println("{}", day12("input/12full.txt", 5)); // 2043098029844
	return 0;
}
