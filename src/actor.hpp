class Actor {
public:
	int x, y;
	int ch; // ASCII code
	TCODColor col; // color
	std::string name;
	bool blocks; // does it block movement?
	Attacker* attacker;
	Destructible* destructible;
	Ai* ai; // self-updating
	Pickable* pickable;
	Container* container;

	Actor(int x, int y, int ch, std::string name, const TCODColor& col);
	~Actor();
	void update();
	void render() const;
	float getDistance(int cx, int cy) const;

private:
	friend class boost::serialization::access;                                                                
    template<class Archive>                                                                                   
    void serialize(Archive & ar, const unsigned int version) {                                                
        ar & x; // works on save
		ar & y; // works on save
		ar & col; // works on save
		ar & name; // works on save
		ar & blocks; // works on save
		ar & attacker; // works on save
		ar & destructible; // works on save
		ar & ai; // works on save
		ar & pickable; // works on save
		ar & container; // works on save
    }   
};
