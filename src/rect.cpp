#include "rect.hpp"
#include <algorithm>

Rect::Rect(int x0, int y0, int x1, int y1) {
	if(x0 > x1) std::swap(x0, x1);
	if(y0 > y1) std::swap(y0, y1);
	topLeft = Point(x0, y0);
	bottomRight = Point(x1, y1);
}
