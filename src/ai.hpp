class Ai : public Persistent {
public:
	virtual void update(Actor* owner) = 0;
	virtual ~Ai() {};
	static Ai* create (TCODZip& zip);

	virtual void save(TCODZip& zip) = 0;
	virtual void load(TCODZip& zip) = 0;
protected :
	enum AiType {
		MONSTER, CONFUSED_MONSTER, PLAYER
	};
};

class PlayerAi : public Ai {
public:
	void update(Actor* owner);
	void save(TCODZip& zip);
	void load(TCODZip& zip);

protected:
	bool moveOrAttack(Actor* owner, int targetX, int targetY);
	Actor* chooseFromInventory(Actor* owner);

private:
	void handleActionKey(Actor* owner, int ascii);
};

class MonsterAi : public Ai {
public:
	void update(Actor* owner);
	void save(TCODZip& zip);
	void load(TCODZip& zip);

protected:
	int moveCount;

	void moveOrAttack(Actor* owner, int targetX, int targetY);
};

class ConfusedMonsterAi : public Ai {
public:
	ConfusedMonsterAi(int turns, Ai* oldAi);
	void update(Actor* owner);
	void save(TCODZip& zip);
	void load(TCODZip& zip);

protected:
	int turns;
	Ai* oldAi;
};
