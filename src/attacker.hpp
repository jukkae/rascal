class Attacker {
public :
	float power;

	Attacker(float power);
	Attacker(); // TODO dirty hack
	void attack(Actor* owner, Actor* target);

private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & power;
	}
};
