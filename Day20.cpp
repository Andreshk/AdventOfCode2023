#include "Lines.h"
#include <print>
#include <vector>
#include <ranges> // std::views::{split,values}
#include <algorithm> // std::ranges::{containd,find,all_of,fold_left}
#include <map>
#include <queue>
#include <numeric> // std::lcm
using namespace std::string_view_literals;

void day20(const char* filename, bool part2 = false) {
	struct Info {
		std::vector<std::string> outputs;
		char type = 'b';
		bool onOff = false;
	};
	std::map<std::string, Info> map;
	for (std::string_view line : lines(filename)) {
		const auto pos = line.find(" -> ");
		const bool b = !std::isalpha(line[0]);
		const std::string name{line, b, pos - b};
		auto& [outputs, type, onOff] = map[name];
		type = line[0];
		onOff = false;
		for (auto&& chunk : std::views::split(line.substr(pos + 4), ", "sv)) {
			outputs.push_back(std::string{chunk.begin(), chunk.end()});
		}
	}
	// Memory of all conjunction modules
	std::map<std::string, std::vector<std::pair<std::string, bool>>> conjs;
	for (const auto& [name, info] : map) {
		for (const std::string& out : info.outputs) {
			if (map[out].type == '&') {
				conjs[out].emplace_back(name, false); // default to remembering low
			}
		}
	}
	// Part2: the 4 conj modules rx depends on in my (?) input
	std::string names[] = {"mp","ng","qb","qt"};
	std::map<std::string, int> pulses; // Time to first low for them

	struct Pulse { std::string name; bool high; std::string sender; };
	int counts[2] = {0,0};
	for (int i = 0; (part2 || i <= 1000); ++i) {
		if (i == 1000) {
			std::println("{}", counts[0] * counts[1]);
			if (!part2) { return; }
		}
		std::queue<Pulse> ps;
		ps.emplace("broadcaster", false, "button");
		while (!ps.empty()) {
			const auto [name, high, sender] = ps.front(); ps.pop();
			if (part2 && std::ranges::contains(names, name) && !high && !pulses.contains(name)) {
				pulses[name] = (i + 1);
				if (pulses.size() == 4) {
					const auto part2 = std::ranges::fold_left(std::views::values(pulses), 1LL, std::lcm<int64_t, int>);
					std::println("{}", part2);
					return; // Nothing more to do
				}
			}
			++counts[high];
			if (!map.contains(name)) { continue; } // Dummy node, no outputs, just receive stuff
			auto& [outputs, type, onOff] = map.at(name);
			switch (type) {
			case '&': {
				const auto it = std::ranges::find(conjs.at(name), sender, &std::pair<std::string, bool>::first);
				it->second = high; // Update the memory
				const bool allHigh = (std::ranges::all_of(conjs.at(name), std::identity{}, &std::pair<std::string, bool>::second));
				for (const auto& out : outputs) {
					ps.emplace(out, !allHigh, name);
				}
			} break;
			case '%': {
				if (!high) {
					for (const auto& out : outputs) {
						ps.emplace(out, !onOff, name);
					}
					onOff ^= true;
				}
			} break;
			case 'b': { // This is the broadcaster
				for (const auto& out : outputs) {
					ps.emplace(out, high, name);
				}
			} break;
			default: { assert(false); break; }
			}
		}
	}
}

int main20() {
	day20("input/20test.txt"); // 32000000
	day20("input/20test2.txt"); // 11687500
	day20("input/20full.txt", true); // 919383692 247702167614647
	return 0;
}
