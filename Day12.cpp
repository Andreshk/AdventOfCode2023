#include "Lines.h"
#include <scn/scan.h>
#include <print>
#include <vector>
#include <ranges> // std::views::{filter,transform,iota,chunk_by}
#include <algorithm> // std::ranges::count_if
#include <functional> // std::ranges::equal_to
#include <unordered_map>
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

// A compressed tuple of the three ints, identifying the current
// search state: string length, span length & current run length.
// Note: if the cache keeps std::pairs of <Key,int64_t>, there'll be
// padding after each Key, reducing the gain of going below 64 bits.
using Key = int;
Key makeKey(std::string_view str, std::span<const int> xs, const int currLen) {
	const auto off1 = CHAR_BIT * sizeof(Key) / 2;
	const auto off2 = off1 / 2;
	return (Key(str.size()) << off1) | (Key(xs.size()) << off2) | currLen;;
}

// If we're currently in a # run, currLen is the length of its processed part (so, always > 0)
// currLen == 0 means a # run is not yet started
int64_t match2(std::string_view str, std::span<const int> xs, const int currLen, std::unordered_map<Key, int64_t>& cache) {
	const auto [it, inserted] = cache.insert(std::make_pair(makeKey(str,xs,currLen), 0ll));
	if (!inserted) {
		return it->second;
	} else if (str.empty()) {
		return (it->second = xs.empty() || (xs.size() == 1 && xs[0] == currLen));
	} else if (xs.empty()) {
		return (it->second = !str.contains('#'));
	}
	int64_t sum = 0;
	// Next symbol is or can be a dot
	if (str[0] != '#' && !(currLen > 0 && currLen < xs[0])) {
		sum += match2(str.substr(1), xs.subspan(currLen > 0), 0, cache);
	}
	// Next symbol is or can be a hash
	if (str[0] != '.' && !(currLen > 0 && currLen == xs[0])) {
		assert(currLen < xs[0]);
		sum += match2(str.substr(1), xs, currLen + 1, cache);
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
			const auto counts = std::views::split(counts1, ',')
				| std::views::transform([](auto&& chunk) { return scn::scan<int>(chunk, "{}")->value(); })
				| std::ranges::to<std::vector>();

			// return matchSimple(map, counts);
			std::unordered_map<Key, int64_t> cache;
			return match2(map, counts, 0, cache);
		});
}

#include <chrono>

int main12() {
	std::println("{}", day12("input/12test.txt")); // 21
	std::println("{}", day12("input/12full.txt")); // 6871
	std::println("{}", day12("input/12test.txt", 5)); // 525152

	const auto start = std::chrono::steady_clock::now();
	const int64_t res = day12("input/12full.txt", 5); // 2043098029844
	const auto end = std::chrono::steady_clock::now();
	std::println("{} ({})", res, std::chrono::duration_cast<std::chrono::milliseconds>(end - start));
	return 0;
}
