#include "Lines.h"
#include <scn/scan.h>
#include <print>
#include <vector>
#include <ranges> // std::views::{iota,filter,transform}
#include <algorithm> // std::ranges::{sort,find,lexicographical_compare}
#include <array>

int getType(const std::array<char, 5>& h) {
	std::array<int, 'U'-'2'> counts = {};
	for (char c : h) { ++counts[c - '2']; }
	std::ranges::sort(counts, std::greater<>{});
	switch (counts[0]) {
	case 5: return 0;
	case 4: return 1;
	case 3: return (counts[1] == 2 ? 2 : 3);
	case 2: return (counts[1] == 2 ? 4 : 5);
	default: return 6;
	}
}

int getType2(std::array<char, 5> h) {
	const auto jokers = std::views::iota(0, 5)
		| std::views::filter([&](int i) { return h[i] == 'J'; })
		| std::ranges::to<std::vector>();

	std::string_view cs = "AKQT98765432"; // The non-jokers
	switch (jokers.size()) {
	case 0: return getType(h);
	case 1: // Just try every possibility
		return std::ranges::min(cs | std::views::transform([&](char c) {
			h[jokers[0]] = c;
			return getType(h);
		}));
	case 2: { // Match with the most frequent non-joker card
		const int c = std::ranges::count_if(cs, [&](char c) { return std::ranges::contains(h, c); });
		return (c < 3 ? c - 1 : 3);
	}
	case 3: // Four of five of a kind (if the non-jokers match)
		return (std::ranges::count_if(cs, [&](char c) { return std::ranges::contains(h, c); }) - 1);
	default: // Enough for a five-of-a-kind
		return 0;
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
		type1 = getType(h);
		type2 = getType2(h);
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
