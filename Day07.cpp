#include "Lines.h"
#include <scn/scan.h>
#include <print>
#include <vector>
#include <ranges> // std::views::{iota,filter,transform}
#include <algorithm> // std::ranges::{sort,find,lexicographical_compare}
#include <array>
#include <utility> // std::exchange

int getType(const std::array<char, 5>& h, const bool part2) {
	std::array<int, 'U'-'2'> counts = {};
	for (char c : h) { ++counts[c - '2']; }
	const int jokers = (part2 ? std::exchange(counts['J' - '2'], 0) : 0);
	std::ranges::sort(counts, std::greater<>{});
	counts[0] += jokers; // Optimum strategy is to just match the most frequent non-joker card
	switch (counts[0]) {
	case 5: return 0;
	case 4: return 1;
	case 3: return (counts[1] == 2 ? 2 : 3);
	case 2: return (counts[1] == 2 ? 4 : 5);
	default: return 6;
	}
}

struct Hand {
	std::array<char, 5> cards;
	int bid;
	int type1;
	int type2;
};

template <bool Part1>
bool cmp(const Hand& lhs, const Hand& rhs) {
	auto rank = [](char c) {
		static const std::string_view cs = (Part1 ? "AKQJT98765432" : "AKQT98765432J");
		return int(std::ranges::find(cs, c) - cs.begin());
	};
	const int lhsType = (Part1 ? lhs.type1 : lhs.type2);
	const int rhsType = (Part1 ? rhs.type1 : rhs.type2);
	return (lhsType > rhsType
		|| (lhsType == rhsType
			&& !std::ranges::lexicographical_compare(lhs.cards, rhs.cards, std::less<>{}, rank, rank)));
}

void day07(const char* filename) {
	std::vector<Hand> hands;
	for (std::string_view line : lines(filename)) {
		auto& [h, bid, type1, type2] = hands.emplace_back();
		for (int i = 0; i < 5; ++i) {
			h[i] = line[i];
		}
		bid = scn::scan<int>(line.substr(6), "{}")->value();
		type1 = getType(h, false);
		type2 = getType(h, true);
	}
	auto f = [](auto&& p) { auto& [i, h] = p; return (int(i) + 1) * h.bid; };
	std::ranges::sort(hands, cmp<true>);
	const int part1 = ranges::sum(std::views::enumerate(hands), f);
	std::ranges::sort(hands, cmp<false>);
	const int part2 = ranges::sum(std::views::enumerate(hands), f);
	std::println("{} {}", part1, part2);
}

int main07() {
	day07("input/7test.txt"); // 6440 5905
	day07("input/7full.txt"); // 249726565 251135960
	return 0;
}
