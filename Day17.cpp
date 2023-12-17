#include "Lines.h"
#include "IntPairs.h"
#include <print>
#include <vector>
#include <ranges> // std::views::as_rvalue
#include <queue>
#include <map>

int day17(const char* filename, const int min, const int max) {
	const std::vector<std::string> map{std::from_range, std::views::as_rvalue(lines(filename))};
	const int n = int(map.size());
	const int m = int(map[0].size());

	struct Item {
		IntPair pos;
		int dir, run;
		auto operator<=>(const Item&) const = default;
	};
	std::map<Item, int> visited;
	std::priority_queue<std::pair<int, Item>, std::vector<std::pair<int, Item>>, std::greater<>> pq;
	// Two possible directions from the start
	pq.emplace(0, Item{{0,0},0,0});
	pq.emplace(0, Item{{0,0},1,0});

	while (!pq.empty()) {
		const auto [total, item] = pq.top(); pq.pop();
		const auto [it, inserted] = visited.insert(std::make_pair(item, total));
		if (!inserted) { // Already visited, this is a duplicate due to lack of decreaseKey()
			assert(it->second <= total);
			continue;
		}
		if (item.pos == IntPair{n - 1,m - 1} && item.run >= min) {
			return total;
		}
		const IntPair offs[4] = {{0,1},{1,0},{0,-1},{-1,0}};
		for (int d = 0; d < 4; ++d) {
			if (std::abs(d - item.dir) == 2) { continue; } // No turning back
			if (d == item.dir ? item.run == max : item.run < min) { continue; } // Only allowed dir
			const IntPair newPos = item.pos + offs[d];
			if (newPos.i < 0 || newPos.i >= n || newPos.j < 0 || newPos.j >= m) { continue; } // No out-of-bounds
			const int newTotal = total + map[newPos.i][newPos.j] - '0';
			const int newRun = (d == item.dir ? item.run + 1 : 1);
			pq.emplace(newTotal, Item{newPos, d, newRun});
		}
	}
	std::unreachable();
}

int main17() {
	std::println("{}", day17("input/17test.txt", 0, 3));   // 102
	std::println("{}", day17("input/17test.txt", 4, 10));  // 94
	std::println("{}", day17("input/17test2.txt", 4, 10)); // 71
	std::println("{}", day17("input/17full.txt", 0, 3));   // 1138
	std::println("{}", day17("input/17full.txt", 4, 10));  // 1312
	return 0;
}
