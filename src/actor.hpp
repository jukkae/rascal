class Actor {
public:
	int x, y;
	int ch; // ASCII code
	TCODColor col; // color
	std::string name;
	bool blocks; // does it block movement?
	bool fovOnly; // visible only when in fov?
	std::unique_ptr<Attacker> attacker;
	std::unique_ptr<Destructible> destructible;
	std::unique_ptr<Ai> ai;
	std::unique_ptr<Pickable> pickable;
	std::unique_ptr<Container> container;

	Actor(int x = 0, int y = 0, int ch = 'x', std::string name = "", const TCODColor& col = TCODColor::white);
	~Actor();
	float update();
	void render() const;
	float getDistance(int cx, int cy) const;

private:
	friend class boost::serialization::access;                                                                
    template<class Archive>                                                                                   
    void serialize(Archive & ar, const unsigned int version) {                                                
        ar & x;
		ar & y;
		ar & ch;
		ar & col;
		ar & name;
		ar & blocks;
		ar & fovOnly;
		ar & attacker;
		ar & destructible;
		ar & ai;
		ar & pickable;
		ar & container;
    }   
};
