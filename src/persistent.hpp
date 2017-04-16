class Persistent {
public:
	virtual void save(TCODZip& zip) = 0;
	virtual void load(TCODZip& zip) = 0;
};
