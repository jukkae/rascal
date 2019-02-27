#ifndef RECT_HPP
#define RECT_HPP

#include "point.hpp"

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/export.hpp>

/* Axis-aligned rectangle with integer coordinates */
struct Rect {
public:
	Point topLeft;
	Point bottomRight;

	Rect(Point topLeft, Point bottomRight) : topLeft(topLeft), bottomRight(bottomRight) {;}
	Rect(int x0, int y0, int x1, int y1);
	Rect(): Rect(Point(), Point()) {;}

	// naming conventions & constness?
	int x0() { return topLeft.x; }
	int x1() { return bottomRight.x; }
	int y0() { return topLeft.y; }
	int y1() { return bottomRight.y; }
	int width() { return x1()-x0() + 1; }
	int height() { return y1()-y0() + 1; }

	bool operator==(const Rect& rhs) const { return this->topLeft == rhs.topLeft && this->bottomRight == rhs.bottomRight; }
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & topLeft;
		ar & bottomRight;
	}
};

#endif /* RECT_HPP */
