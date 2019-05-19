#ifndef OPENABLE_HPP
#define OPENABLE_HPP

#include <boost/archive/text_iarchive.hpp>

enum class LockType { NONE, RED, GREEN, BLUE };
class Openable {
public:
	bool open = false;
	LockType lockType = LockType::NONE;
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & open;
		ar & lockType;
	}
};
#endif /* OPENABLE_HPP */
