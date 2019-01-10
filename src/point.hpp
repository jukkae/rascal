#ifndef POINT_HPP
#define POINT_HPP

struct Point {
	int x;
	int y;

	explicit Point(int x = 0, int y = 0) : x(x), y(y) {;}

	bool operator==(const Point& rhs) const { return this->x == rhs.x && this->y == rhs.y; }
};

struct PointHasher
{
  std::size_t operator()(const Point& p) const
  {
    using std::size_t;
    using std::hash;
    using std::string;

		// TODO probably a shit hash
    return ((hash<int>()(p.x)
             ^ (hash<int>()(p.y) << 1)) >> 1);
  }
};

#endif /* POINT_HPP */
