#include "Lines.h"
#include "IntPairs.h"
#include <print>
#include <vector>
#include <cassert>
#include <charconv> // std::from_chars
#include <map>

void day03(const char* filename) {
	std::vector<std::string> map{ std::from_range, std::views::as_rvalue(lines(filename)) };
	int part1 = 0, part2 = 0;
	std::map<IntPair, std::vector<int>> ns;
	for (auto&& [ii, line] : std::views::enumerate(map)) {
		size_t off = 0;
		while (off < line.size()) {
			auto from = line.find_first_of("0123456789", off);
			if (from == std::string::npos) {
				break;
			}
			auto end = line.find_first_not_of("0123456789", from + 1);
			end = std::min(end, line.size());
			int n = 0;
			const auto [ptr, ec] = std::from_chars(line.data() + from, line.data() + end, n);
			assert(ec == std::errc{} && ptr == line.data() + end);
			// Just loop through the neighboring spaces
			for (int i = std::max(int(ii) - 1, 0); i <= std::min(int(ii) + 1, int(map.size() - 1)); ++i) {
				for (int j = std::max(int(from) - 1, 0); j < std::min(end + 1, line.size()); ++j) {
					if (map[i][j] != '.' && !std::isdigit(map[i][j])) { // Don't count the digits of n
						part1 += n;
					}
					if (map[i][j] == '*') {
						ns[{i, j}].push_back(n);
					}
				}
			}
			off = end + 1;
		}
	}
	for (const auto& nss : std::views::values(ns)) {
		for (int i = 0; i < nss.size() - 1; ++i) {
			for (int j = i + 1; j < nss.size(); ++j) {
				part2 += (nss[i] * nss[j]);
			}
		}
	}
	std::println("{} {}", part1, part2);
}

int main03() {
	day03("input/3test.txt"); // 4361 467835
	day03("input/3full.txt"); // 525119 76504829
	return 0;
}
