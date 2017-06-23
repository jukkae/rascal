class TargetSelector {
public:
	// TODO move to enum class as soon as makefile trouble is fixed
	enum SelectorType { CLOSEST_MONSTER, SELECTED_MONSTER, WEARER, WEARER_RANGE, SELECTED_RANGE };
	TargetSelector();
	TargetSelector(SelectorType type, float range);
	void selectTargets(Actor* wearer, std::vector<Actor*>& list);
protected:
	SelectorType type;
	float range;
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & type;
		ar & range;
	}
};

class Effect {
public:
	virtual bool applyTo(Actor* actor) = 0;
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {

	}
};

class HealthEffect : public Effect {
public:
	float amount;
	std::string message;

	HealthEffect(float amount = 0, std::string message = "");
	bool applyTo(Actor* actor) override;
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Effect);
		ar & amount;
		ar & message;
	}
};

class AiChangeEffect : public Effect {
public:
	TemporaryAi* newAi;
	std::string message;

	AiChangeEffect(TemporaryAi* newAi = NULL, std::string message = "");
	bool applyTo(Actor* actor) override;
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Effect);
		ar & newAi;
		ar & message;
	}
};

class Pickable {
public:
	bool pick(Actor* owner, Actor* wearer);
	virtual bool use(Actor* owner, Actor* wearer);
	virtual ~Pickable() {};
	void drop(Actor* owner, Actor* wearer);
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
		ar.template register_type<Pickable>();
		ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Pickable);
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
		ar.template register_type<Pickable>();
		ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Pickable);
		ar & range;
		ar & damage;
    }
};

class FragmentationGrenade : public BlasterBolt {
public:
	FragmentationGrenade(float range = 0, float damage = 0);
	bool use(Actor* owner, Actor* wearer);
private:
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {
		ar.template register_type<Pickable>();
		ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(BlasterBolt);
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
		ar.template register_type<Pickable>();
		ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Pickable);
		ar & turns;
		ar & range;
    }
};

BOOST_CLASS_EXPORT_KEY(HealthEffect)
BOOST_CLASS_EXPORT_KEY(AiChangeEffect)	

BOOST_CLASS_EXPORT_KEY(Healer)
BOOST_CLASS_EXPORT_KEY(BlasterBolt)
BOOST_CLASS_EXPORT_KEY(FragmentationGrenade)
BOOST_CLASS_EXPORT_KEY(Confusor)
