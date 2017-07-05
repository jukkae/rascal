#ifndef POINT_HPP
#define POINT_HPP

struct Point {
	int x;
	int y;

	explicit Point(int x = 0, int y = 0) : x(x), y(y) {;}
};

#endif /* POINT_HPP */
