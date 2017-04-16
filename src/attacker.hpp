class Attacker : public Persistent {
public :
	float power;

	Attacker(float power);
	void attack(Actor* owner, Actor* target);

	void save(TCODZip& zip);
	void load(TCODZip& zip);
};
