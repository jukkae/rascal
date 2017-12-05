#ifndef TRANSPORTER_HPP
#define TRANSPORTER_HPP
#include "direction.hpp"
class Transporter {
public:
	VerticalDirection direction = VerticalDirection::NONE;
private:
	friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {
		ar & direction;
	}
};
#endif /* TRANSPORTER_HPP */
