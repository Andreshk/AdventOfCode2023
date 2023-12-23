#include "Lines.h"
#include "IntPairs.h"
#include <print>
#include <vector>
#include <ranges> // std::views::{as_rvalue,enumerate,drop,reverse}
#include <algorithm> // std::ranges::{all_of, contains}
#include <unordered_map>
#include <queue>
using namespace std::string_view_literals;

namespace std { // to-do: add this to IntPairs.h without #include <unordered_map>
template <> struct hash<IntPair> {
	auto operator()(const IntPair& p) const { return std::hash<int64_t>{}((int64_t(p.i) << 32) | p.j); }
};
}

// Build the graph from the given map
auto makeGraph(const char* filename, const bool part1) {
	std::vector<std::string> map{std::from_range, std::views::as_rvalue(lines(filename))};
	const IntPair goal = {int(map.size()) - 1, int(map[0].size()) - 2};

	std::unordered_map<IntPair, int> idxs;
	idxs[{0, 1}] = 0;
	std::vector<std::vector<IntPair>> dists;
	dists.emplace_back();

	struct Item { IntPair curr; int prev, dist, dir; };
	std::queue<Item> q;
	q.emplace(IntPair{1,1}, 0, 1, 1);

	while (!q.empty()) {
		const auto [curr, prev, dist, dir] = q.front(); q.pop();
		if (idxs.contains(curr)) { // Found another path to an existing split point -> mark the edge (!)
			if (prev != idxs.at(curr)) { // We went down a "blocked" path and returned back -> nothing to do
				dists[prev].emplace_back(idxs.at(curr), dist);
			} 
			continue;
		}
		assert(prev < idxs.size() && idxs.size() == dists.size());
		assert(map[curr.i][curr.j] != '#');
		const IntPair offs[4] = {IntPair{0,1},{1,0},{0,-1},{-1,0}}; // R D L U
		std::vector<int> neighbs;
		// Look for neighbors
		if (curr != goal) {
			if (const auto pos = ">v<^"sv.find(map[curr.i][curr.j]); part1 && pos != -1) {
				neighbs.push_back(int(pos));
			} else for (const auto&& [i,d] : std::views::enumerate(offs)) {
				if (i != (dir + 2) % 4 && map[curr.i + d.i][curr.j + d.j] != '#') {
					neighbs.push_back(int(i));
				}
			}
		}
		if (neighbs.size() == 1) { // continue current path
			q.emplace(curr + offs[neighbs[0]], prev, dist + 1, neighbs[0]);
		} else { // reached a split, should be the first time
			const int idx = int(idxs.size());
			const auto [_, inserted] = idxs.insert(std::make_pair(curr, idx));
			assert(inserted);
			dists.emplace_back();
			dists[prev].emplace_back(idx, dist);
			for (const int d : neighbs) {
				q.emplace(curr + offs[d], idx, 1, d);
			}
		}
	}
	if (!part1) { // Complete the undirected graph, so that all paths are available
		const int n = int(dists.size());
		for (int u = 0; u < n; ++u) {
			for (const auto& [v, d] : dists[u]) {
				if (!std::ranges::contains(dists[v], u, &IntPair::i)) {
					dists[v].emplace_back(u, d);
				}
			}
		}
	}
	return std::make_pair(std::move(dists), idxs.at(goal));
}

std::vector<int> topoSort(const std::vector<std::vector<IntPair>>& dists) {
	const int n = int(dists.size());
	std::vector<int> ins(n, 0);
	for (const auto& ns : dists) {
		for (const auto& [v, _] : ns) {
			++ins[v];
		}
	}
	std::queue<int> q;
	// The only source in this task is vertex 0
	assert(ins[0] == 0 && !std::ranges::contains(ins | std::views::drop(1), 0));
	q.emplace(0);
	std::vector<int> res;
	std::vector<int> counts(n, 0);
	while (!q.empty()) {
		const int u = q.front(); q.pop();
		res.push_back(u);
		for (const auto& [v, _] : dists[u]) {
			if (++counts[v] == ins[v]) {
				q.emplace(v);
			}
		}
	}
	assert(res.size() == n); // Otherwise, a cycle was detected
	return res;
}

int day23(const char* filename, const bool part1) {
	const auto [dists, goal] = makeGraph(filename, part1);
	if (part1) { // Sort topologically to find the longest distance
		std::vector<int> paths(dists.size(), -1);
		for (const int u : std::views::reverse(topoSort(dists))) {
			paths[u] = 0;
			for (const auto& [v, d] : dists[u]) {
				assert(paths[v] != -1);
				paths[u] = std::max(paths[u], paths[v] + d);
			}
		}
		return paths[0];
	} else {
		// We'll cache the longest path for each pair of (current vertex, visited set),
		// compressed in 64 bits. Lowest 6 bits is the index of the current vertex.
		using Key = uint64_t;
		constexpr uint64_t mask = (1ULL << 6) - 1;
		auto makeKey = [](const Key old, const int v) -> Key {
			assert(v <= mask);
			return (old & ~mask) | (1ULL << (v + 6)) | v;
		};
		std::unordered_map<Key, int> cache;
		// Immediately invoked recursive lambda ftw
		return [&](this auto&& self, const Key k) -> int {
			const int u = (k & mask);
			if (u == goal) { // iei
				return 0;
			} else if (const auto [it, inserted] = cache.insert(std::make_pair(k, 0)); !inserted) {
				return it->second;
			} else {
				// This is ranges::max(dists[u] | filter | transform),
				// but it doesn't know how to return 0 for the empty range
				int res = 0;
				for (const auto& [v, d] : dists[u]) {
					if ((k & (1ULL << (v + 6))) == 0) {
						res = std::max(res, d + self(makeKey(k, v)));
					}
				}
				if (k == 64) { std::println("(cache:{}) ", cache.size()); }
				return (it->second = res);
			}
		}(makeKey(0, 0));
	}
}

int main23() {
	std::println("{}", day23("input/23test.txt", true)); // 94
	std::println("{}", day23("input/23full.txt", true)); // 2246
	std::println("{}", day23("input/23test.txt", false)); // 154
	std::println("{}", day23("input/23full.txt", false)); // 6622, ~12million in cache
	return 0;
}
