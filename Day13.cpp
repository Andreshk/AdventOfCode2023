#include "Lines.h"
#include <print>
#include <vector>
#include <ranges> // std::views::{filter,transform,iota,split}
#include <algorithm> // std::ranges::{count_if,all_of}

auto countDiffs(const std::string& lhs, const std::string& rhs) {
	return std::ranges::count_if(std::views::iota(0, int(lhs.size())),
		[&](int k) { return lhs[k] != rhs[k]; });
}

int checkRefl(const std::vector<std::string>& m, const int mod, const int limit) {
	const int k = int(m.size());
	return ranges::sum(std::views::iota(1, k)
		| std::views::filter([&](int row) {
			const auto rows = std::views::iota(0, std::min(row, k - row));
			return (limit == ranges::sum(rows, [&](int i) { return countDiffs(m[row + i], m[row - 1 - i]); }));
		})
		| std::views::transform([&](int row) { return row * mod; }));
};

int day13(const char* filename, const int limit) {
	const std::vector<std::string> map{std::from_range, std::views::as_rvalue(lines(filename))};

	return ranges::sum(std::views::split(map, std::string{}),
		[&](auto&& chunk) {
			const std::vector<std::string> piece{chunk.begin(), chunk.end()};
			const int n = int(piece.size());
			const int m = int(piece[0].size());
			assert(n > 0 && m > 0);
			assert(std::ranges::all_of(piece, [&](auto&& s) { return s.size() == m; }));

			std::vector<std::string> transp(m, std::string(n, '.'));
			for (int i = 0; i < n; ++i) {
				for (int j = 0; j < m; ++j) {
					transp[j][i] = piece[i][j];
				}
			}
			return checkRefl(piece, 100, limit) + checkRefl(transp, 1, limit);
		});
}

int main13() {
	std::println("{}", day13("input/13test.txt", 0)); // 405
	std::println("{}", day13("input/13full.txt", 0)); // 32371
	std::println("{}", day13("input/13test.txt", 1)); // 400
	std::println("{}", day13("input/13full.txt", 1)); // 37416
	return 0;
}
