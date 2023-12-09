#pragma once
#include <cppcoro/generator.hpp> // preferrably from github/andreasbuhr
#include <fstream>
#include <string>
#include <algorithm> // std::ranges::fold_left
#include <ranges> // std::views::transform
#include <vector>
#include <scn/scan.h>

// Used to iterate the lines of a file. Can be iterated as
//   for (std::string_view line : lines(...)) { ... }
// or have the string contents moved out with
//   for (std::string& line : lines(...)) { /* use std::move(line) */ }
inline cppcoro::generator<std::string> lines(const char* filename) {
	std::string line;
	std::ifstream file{ filename };
	while (std::getline(file, line)) {
		co_yield line;
	}
}

template <typename T>
std::vector<T> scanList(std::string_view str) {
	std::vector<T> xs;
	for (std::ranges::subrange rng{str}; !rng.empty();) {
		auto res = scn::scan<int>(rng, "{}");
		xs.push_back(res->value());
		rng = res->range();
	}
	return xs;
}

// not std (!)
namespace ranges {
template <typename Rng, typename Proj = std::identity>
auto sum(Rng&& rng, Proj proj = {}) {
	using T = std::decay_t<std::invoke_result_t<Proj, std::ranges::range_reference_t<Rng>>>; // as per rangev3's fold
	return std::ranges::fold_left(std::views::transform(rng, proj), T{ 0 }, std::plus<>{});
}
template <typename Rng, typename Proj = std::identity>
auto product(Rng&& rng, Proj proj = {}) {
	using T = std::decay_t<std::invoke_result_t<Proj, std::ranges::range_reference_t<Rng>>>; // as per rangev3's fold
	return std::ranges::fold_left(std::views::transform(rng, proj), T{ 1 }, std::multiplies<>{});
}
}
