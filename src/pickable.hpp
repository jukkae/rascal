class Pickable : public Persistent {
public:
	bool pick(Actor* owner, Actor* wearer);
	virtual bool use(Actor* owner, Actor* wearer);
	virtual ~Pickable() {};
	void drop(Actor* owner, Actor* wearer);
	static Pickable* create (TCODZip& zip);
protected:
	enum PickableType {
		HEALER, BLASTER_BOLT, FRAGMENTATION_GRENADE, CONFUSOR
	};
private:
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {

    }
};

class Healer : public Pickable {
public:
	float amount;

	Healer(float amount = 0);
	bool use(Actor* owner, Actor* wearer);
private:
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {
		ar & boost::serialization::base_object<Pickable>(*this);
		ar & amount;
    }
};

class BlasterBolt : public Pickable {
public:
	float range;
	float damage;
	BlasterBolt(float range = 0, float damage = 0);
	bool use(Actor* owner, Actor* wearer);
private:
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {
		ar & boost::serialization::base_object<Pickable>(*this);
		ar & range;
		ar & damage;
    }
};

class FragmentationGrenade : public BlasterBolt { // TODO inherits from BB for fast dev
public:
	FragmentationGrenade(float range = 0, float damage = 0);
	bool use(Actor* owner, Actor* wearer);
private:
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {
		ar & boost::serialization::base_object<BlasterBolt>(*this);
    }
};

class Confusor : public Pickable {
public:
	int turns;
	float range;
	Confusor(int turns = 0, float range = 0);
	bool use(Actor* owner, Actor* wearer);
private:
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {
		ar & boost::serialization::base_object<Pickable>(*this);
		ar & turns;
		ar & range;
    }
};

//BOOST_CLASS_EXPORT_GUID(Pickable, "Pickable")
//BOOST_CLASS_EXPORT_GUID(Healer, "Healer")
//BOOST_CLASS_EXPORT_GUID(BlasterBolt, "BlasterBolt")
//BOOST_CLASS_EXPORT_GUID(FragmentationGrenade, "FragmentationGrenade")
//BOOST_CLASS_EXPORT_GUID(Confusor, "Confusor")
