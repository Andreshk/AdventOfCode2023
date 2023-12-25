#include "Lines.h"
#include "IntPairs.h"
#include <print>
#include <vector>
#include <ranges> // std::views::{filter,cartesian_product}
#include <algorithm> // std::ranges::{shuffle,to,contains}
#include <map>
#include <queue>
#include <random> // std::{mt19937,random_device} for ranges::shuffle

class UnionFind {
	std::vector<IntPair> values; // {parent,rank} pairs
public:
	// Find w/ path halving.
	int Find(int x) {
		while (values[x].i != x) {
			values[x].i = values[values[x].i].i;
			x = values[x].i;
		}
		return x;
	}
	explicit UnionFind(const int n) : values(n) {
		for (int i = 0; i < n; ++i) {
			values[i] = {i,0};
		}
	}
	// Union by rank. Returns true on successful union,
	// false if x and y are already in the same component.
	bool Union(int x, int y) {
		const int xroot = Find(x), yroot = Find(y);
		if (xroot == yroot) {
			return false;
		}
		// Add the smaller-ranked as subtree to the larger & bump rank if matching
		if (values[xroot].j < values[yroot].j) {
			values[xroot].i = yroot;
		} else {
			values[yroot].i = xroot;
		}
		if (values[xroot].j == values[yroot].j) {
			++values[xroot].j;
		}
        return true;
    }
};

int day25(const char* filename) {
	// string->int map to replace vertex labels w/ ints starting from 0
	std::map<std::string, int> names;
	auto idx = [&](std::string str) {
		const auto [it, _] = names.insert(std::make_pair(str, int(names.size())));
		return it->second;
	};
	std::vector<IntPair> edges;
	for (const std::string& line : lines(filename)) {
		const int u = idx(line.substr(0, 3));
		for (size_t pos = 5; pos < line.size(); pos += 4) {
			const int v = idx(line.substr(pos, 3));
			edges.emplace_back(u, v);
		}
	}
	const int n = int(names.size());
	// Just run Karger's algorithm until it finds a cut of size 3. Note: graph not required
	std::vector<IntPair> bridges;
	std::mt19937 rng{std::random_device{}()};
	while (bridges.size() != 3) {
		std::ranges::shuffle(edges, rng);
		UnionFind uf{n};
		for (int remaining = n; const auto [u, v] : edges) {
			if (uf.Union(u, v) && --remaining == 2) {
				break;
			}
		}
		bridges = edges
			| std::views::filter([&](IntPair p) { return uf.Find(p.i) != uf.Find(p.j); })
			| std::ranges::to<std::vector>();
	}

	std::vector<std::vector<int>> graph(n);
	for (const auto [u, v] : std::views::filter(edges, [&](IntPair e) { return !std::ranges::contains(bridges, e); })) {
		graph[u].push_back(v);
		graph[v].push_back(u);
	}
	// BFS from any vertex will determine the size of one of the components
	std::queue<int> q;
	std::vector<int> visited(n, false);
	q.push(0); visited[0] = true;
	int m = 0;
	while (!q.empty()) {
		const int v = q.front(); q.pop();
		++m;
		for (const int w : graph[v]) {
			if (!visited[w]) {
				q.push(w); visited[w] = true;
			}
		}
	}
	return m * (n - m);
}

int main25() {
	std::println("{}", day25("input/25test.txt")); // 54
	std::println("{}", day25("input/25full.txt")); // 551196
	return 0;
}

// Initial solution: brute-force removing every possible pair of edges and find out whether
// there is a bridge remaining => if yes, it + the pair are the edges we're looking for.
// Requires building the full graph beforehand. Will take a lot of time for the full input :(
// Example usage: 
/*const auto bridges = (test
	? findBridges(edges, graph)
	: std::array<IntPair, 3>{IntPair{126, 1007},{838, 1171},{640, 1349}});
for (const auto& [u, v] : bridges) {
	std::erase_if(graph[u], [&](auto w) { return w == v; });
	std::erase_if(graph[v], [&](auto w) { return w == u; });
}*/
std::array<IntPair, 3> findBridges(const std::vector<IntPair>& edges, const std::vector<std::vector<int>>& graph) {
	std::array<IntPair, 3> res = {};
	const int n = int(graph.size());
	for (const auto&& [e1, e2] : std::views::cartesian_product(edges, edges)) {
		const auto [u1, v1] = e1;
		const auto [u2, v2] = e2;
		if (u1 > v1 || u2 > v2 || u1 > u2) { continue; }
		if (u1 == u2 && v1 == v2 || u1 == v2 && u2 == v1) { continue; }
		auto copy = graph;
		auto erase = [&](int u, int v) {
			std::erase_if(copy[u], [&](int w) { return w == v; });
		};
		erase(u1, v1);
		erase(v1, u1);
		erase(u2, v2);
		erase(v2, u2);
		// Run Tarjan's algorithm to find if a bridge is remaining
		std::vector<int> visited(n, false);
		std::vector<int> tin(n, 0), low(n, 0);
		int time = 0;
		bool found = false;
		auto dfs = [&](this auto&& self, const int u, const int p) -> void {
			visited[u] = true;
			tin[u] = low[u] = time++;
			for (const int v : copy[u]) {
				if (v == p) { continue; }
				if (visited[v]) {
					low[u] = std::min(low[u], tin[v]);
				} else {
					self(v, u);
					if (found) { break; }
					low[u] = std::min(low[u], low[v]);
					if (low[v] > tin[u]) {
						res = { e1,e2,{u,v}};
						found = true;
						break;
					}
				}
			}
		};
		for (int u = 0; u < n && !found; ++u) {
			if (!visited[u]) {
				dfs(u, -1);
			}
		}
		if (found) { return res; }
	}
	assert(false && "Incorrect input..?");
	return {};
}
