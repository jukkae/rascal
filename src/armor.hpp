#ifndef ARMOR_HPP
#define ARMOR_HPP

class Armor {
public:
	Armor(int acBonus = 0): acBonus(acBonus) {;}
	int acBonus = 0;
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & acBonus;
	}
};

#endif /* ARMOR_HPP */
