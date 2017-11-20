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
	int size; // max number of actors in container, 0 = unlimited
	std::vector<std::unique_ptr<Actor>> inventory;

	Container(int size = 0);
	~Container();
	bool add(std::unique_ptr<Actor> actor);
	void remove(Actor* actor);

private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & size;
		ar & inventory;
	}
};
#endif /* CONTAINER_HPP */
