#ifndef CONTAINER_HPP
#define CONTAINER_HPP

class Actor;
#include <vector>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/vector.hpp>

class Container {
public:
	int capacity; // max number of actors in container, 0 = unlimited
	std::vector<std::unique_ptr<Actor>> inventory;
	int credits = 0;

	Container(int capacity = 0);
	bool isFull() { return capacity > 0 && getContentsWeight() >= capacity; }
	int getContentsWeight();
	bool add(std::unique_ptr<Actor> actor);
	void remove(Actor* actor);

private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & capacity;
		ar & inventory;
		ar & credits;
	}
};
#endif /* CONTAINER_HPP */
