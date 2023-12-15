#include "Lines.h"
#include <print>
#include <vector>
#include <ranges> // str::views::{transform,split,enumerate}
#include <algorithm> // std::ranges::{fold_left,find_first_of}
#include <cassert>

uint8_t hash(std::string_view str) {
	return std::ranges::fold_left(str, 0, [](uint8_t res, char c) { return uint8_t((res + c) * 17); });
}

int day15(const char* filename) {
	std::string line;
	std::ifstream{filename} >> line;
	return ranges::sum(std::views::split(line, ',')
		| std::views::transform([](auto&& chunk) { return std::string_view{chunk}; })
		| std::views::transform(hash));
}

int day15_2(const char* filename) {
	std::string line;
	std::ifstream{filename} >> line;
	struct Item {
		std::string_view str;
		int val;
	};
	std::array<std::vector<Item>, 256> map;
	for (auto&& chunk : std::views::split(line, ',')) {
		const auto mid = std::ranges::find_first_of(chunk, "-=");
		assert(mid != chunk.end());
		const std::string_view label{chunk.begin(), mid};
		std::vector<Item>& bucket = map[hash(label)];
		// Helper to find an item with the given label
		auto f = [&](const Item& it) { return it.str == label; };
		if (*mid == '-') {
			std::erase_if(bucket, f);
		} else {
			const int val = mid[1] - '0';
			if (const auto it = std::ranges::find_if(bucket, f); it == bucket.end()) {
				bucket.emplace_back(label, val);
			} else {
				it->val = val;
			}
		}
	}
	int sum = 0;
	for (auto&& [i, bucket] : std::views::enumerate(map)) {
		for (auto&& [j, item] : std::views::enumerate(bucket)) {
			sum += int((i + 1) * (j + 1) * item.val);
		}
	}
	return sum;
}

int main15() {
	std::println("{}", day15("input/15test.txt")); // 1320
	std::println("{}", day15("input/15full.txt")); // 509152
	std::println("{}", day15_2("input/15test.txt")); // 145
	std::println("{}", day15_2("input/15full.txt")); // 244403
	return 0;
}
