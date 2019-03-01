#ifndef POINT_HPP
#define POINT_HPP

struct Point {
	int x;
	int y;

	explicit Point(int x = 0, int y = 0) : x(x), y(y) {;}

	bool operator==(const Point& rhs) const { return this->x == rhs.x && this->y == rhs.y; }
	bool operator<(const Point& rhs) const { // needed for std::set
		if(this->x != rhs.x)
			return this->x < rhs.x;
		else return this->y < rhs.y;
	}
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & x;
		ar & y;
	}
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
