#ifndef COMESTIBLE_HPP
#define COMESTIBLE_HPP
class Actor;

class Comestible {
public:
	int nutrition = 20000;
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & nutrition;
	}
};
#endif /* COMESTIBLE_HPP */
