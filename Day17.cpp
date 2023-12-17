#include "Lines.h"
#include <print>
#include <vector>
#include <ranges> // std::views::as_rvalue
#include <queue>
#include <unordered_map>
#include <bit> // std::bit_cast

struct alignas(int) Item {
	uint8_t x, y;
	uint8_t dir, run;
	friend bool operator==(const Item& lhs, const Item& rhs) {
		return std::bit_cast<int>(lhs) == std::bit_cast<int>(rhs);
	}
};
static_assert(std::has_unique_object_representations_v<Item>); // No padding bits to mess up bit_cast
namespace std {
template<> struct hash<Item> {
	auto operator()(const Item& i) const { return std::hash<int>{}(std::bit_cast<int>(i)); }
};
}

int day17(const char* filename, const int min, const int max) {
	const std::vector<std::string> map{std::from_range, std::views::as_rvalue(lines(filename))};
	const int n = int(map.size());
	const int m = int(map[0].size());
	std::unordered_map<Item, int> visited;

	using Pair = std::pair<int, Item>;
	auto cmp = [](const Pair& lhs, const Pair& rhs) { return (lhs.first > rhs.first); };
	std::priority_queue<Pair, std::vector<Pair>, decltype(cmp)> pq;
	// Two possible directions from the start
	pq.emplace(0, Item{0,0,0,0});
	pq.emplace(0, Item{0,0,1,0});

	while (!pq.empty()) {
		const auto [total, item] = pq.top(); pq.pop();
		const auto [it, inserted] = visited.insert(std::make_pair(item, total));
		if (!inserted) { // Already visited, this is a duplicate due to lack of decreaseKey()
			assert(it->second <= total);
			continue;
		}
		if (item.x == n - 1 && item.y == m - 1 && item.run >= min) {
			return total;
		}
		const int8_t offs[4][2] = {{0,1},{1,0},{0,-1},{-1,0}};
		for (uint8_t d = 0; d < 4; ++d) {
			if (std::abs(d - item.dir) == 2) { continue; } // No turning back
			if (d == item.dir ? item.run == max : item.run < min) { continue; } // Only allowed dir
			const int newX = item.x + offs[d][0];
			const int newY = item.y + offs[d][1];
			if (newX < 0 || newX >= n || newY < 0 || newY >= m) { continue; } // No out-of-bounds
			const int newTotal = total + map[newX][newY] - '0';
			const uint8_t newRun = (d == item.dir ? item.run + 1 : 1);
			pq.emplace(newTotal, Item{uint8_t(newX), uint8_t(newY), d, newRun});
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
