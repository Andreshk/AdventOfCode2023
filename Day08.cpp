#include "Lines.h"
#include <print>
#include <ranges> // std::views::{enumerate,keys,filter}
#include <map>
#include <numeric> // std::lcm

int64_t day08(const char* filename, const bool part1) {
	std::string code;
	// ints can be used in place of null-terminated 3-symbol strings, if needed
	std::map<std::string, std::array<std::string, 2>> map;
	for (auto&& [i, line] : std::views::enumerate(lines(filename))) {
		if (i == 0) {
			code = std::move(line);
		} else if (i > 1) {
			map[line.substr(0, 3)] = { line.substr(7, 3),line.substr(12, 3) };
		}
	}
	// Count the steps to reach a goal from a given string
	auto foo = [&](std::string str) {
		int steps = 0;
		while (part1 ? str != "ZZZ" : str.back() != 'Z') {
			str = map[str][code[steps % code.size()] == 'R'];
			++steps;
		}
		return steps;
	};
	if (part1) {
		return foo("AAA");
	} else {
		int64_t res = 1;
		for (const std::string& str : map
			 | std::views::keys
			 | std::views::filter([](const std::string& k) { return k.back() == 'A'; })
		) {
			res = std::lcm(res, foo(str)); // No ranges::lcm :(
		}
		return res;
	}
}

int main08() {
	std::println("{}", day08("input/8full.txt", true)); // 15871
	std::println("{}", day08("input/8test.txt", false)); // 6
	std::println("{}", day08("input/8full.txt", false)); // 11283670395017
	return 0;
}
