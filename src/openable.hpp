#ifndef OPENABLE_HPP
#define OPENABLE_HPP
class Openable {
public:
	bool open = false;
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & open;
	}
};
#endif /* OPENABLE_HPP */
