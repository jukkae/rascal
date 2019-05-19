#ifndef PATHFINDING_HPP
#define PATHFINDING_HPP

#include <queue>

struct Point;
class Map;

template<typename T, typename priority_t>
using PQElement = std::pair<priority_t, T>;

template<typename T, typename priority_t>
class PQElementComparator {
public:
int operator()(const PQElement<T, priority_t>& lhs, const PQElement<T, priority_t>& rhs)
	{
		return lhs.first > rhs.first; // Equivalent to std::greater
	}
};

template<typename T, typename priority_t>
struct PriorityQueue {
	using U = priority_t;

  std::priority_queue<PQElement<T, U>, std::vector<PQElement<T, U>>,
                 PQElementComparator<T, U>> elements;

  inline bool empty() const {
     return elements.empty();
  }

  inline void put(T item, priority_t priority) {
    elements.emplace(priority, item);
  }

  T get() {
    T best_item = elements.top().second;
    elements.pop();
    return best_item;
  }
};

namespace pathfinding {
std::vector<Point> findPath(Map map, Point from, Point to);
} // namespace pathfinding

#endif /* PATHFINDING_HPP */
