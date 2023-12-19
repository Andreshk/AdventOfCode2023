#include "Lines.h"
#include "IntPairs.h"
#include <scn/scan.h>
#include <print>
#include <vector>
#include <ranges> // std::views::split
#include <algorithm> // std::ranges::{find_if,all_of}
#include <array>
#include <map>
#include <queue>

struct Rule {
	int idx;
	IntPair range;
	std::string target;
};

void day19(const char* filename) {
	std::map<std::string, std::vector<Rule>> flows; // thank god for SSO
	std::vector<std::array<int, 4>> parts;
	// To discern between the two phases of parsing
	for (bool parseRules = true; std::string_view line : lines(filename)) {
		if (line.empty()) {
			parseRules = false;
		} else if (parseRules) {
			const auto pos = line.find('{');
			std::vector<Rule>& rules = flows[std::string{line, 0, pos}];
			// crop to between the braces
			line = line.substr(pos + 1, line.size() - pos - 2);
			for (auto&& chunk : std::views::split(line, ',')) {
				if (chunk.size() == 1 || chunk[1] != '>' && chunk[1] != '<') {
					// Direct jump to another flow (this is also the last chunk)
					rules.emplace_back(-1, IntPair{INT_MIN, INT_MAX}, std::string{chunk.begin(), chunk.end()});
				} else {
					const auto res = scn::scan<char, char, int>(chunk, "{}{}{}:");
					const auto& [label, c, n] = res->values();
					auto& [idx, range, target] = rules.emplace_back();
					idx = int(std::string_view{"xmas"}.find(label));
					range.i = (c == '>' ? n + 1 : INT_MIN);
					range.j = (c == '>' ? INT_MAX : n);
					target.assign_range(res->range());
					assert(idx >= 0 && idx < 4);
				}
			}
		} else {
			const auto [x,m,a,s] = scn::scan<int, int, int, int>(line, "{{x={},m={},a={},s={}}}")->values();
			parts.emplace_back() = {x,m,a,s};
		}
	}
	const int part1 = ranges::sum(parts, [&](const std::array<int, 4>& p) {
		std::string name = "in";
		while (name != "A" && name != "R") {
			name = std::ranges::find_if(flows.at(name), [&](const Rule& r) {
				return r.idx == -1 || p[r.idx] >= r.range.i && p[r.idx] < r.range.j;
			})->target;
		}
		return (name == "A" ? ranges::sum(p) : 0);
	});
	struct State {
		std::string name;
		std::array<IntPair, 4> ps = {IntPair{1,4001},{1,4001},{1,4001},{1,4001}};
	};
	int64_t part2 = 0;
	std::queue<State> q;
	q.emplace("in");
	while (!q.empty()) {
		auto [name, ps] = q.front(); q.pop();
		while (name != "A" && name != "R") { // or std::islower(name[0])
			assert(std::ranges::all_of(ps, [](IntPair p) { return p.i <= p.j; }));
			for (const auto& [idx, range, target] : flows.at(name)) {
				if (idx == -1) {
					name = target; // This is the last rule anyway
				} else if (ps[idx].i < range.i) {
					// Split into two: those inside go to the target, the rest continue to the next check
					auto ps2 = ps;
					ps2[idx].i = range.i;
					q.emplace(target, ps2);
					ps[idx].j = range.i;
				} else if (ps[idx].j > range.j) {
					auto ps2 = ps;
					ps2[idx].j = range.j;
					q.emplace(target, ps2);
					ps[idx].i = range.j;
				}
			}
		}
		if (name == "A") {
			part2 += ranges::product(ps, [](IntPair p) { return int64_t(p.j - p.i); });
		}
	}
	std::println("{} {}", part1, part2);
}

int main19() {
	day19("input/19test.txt"); // 19114 167409079868000
	day19("input/19full.txt"); // 434147 136146366355609
	return 0;
}
