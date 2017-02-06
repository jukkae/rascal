class Ai {
public:
	virtual void update(Actor* owner) = 0;
	virtual ~Ai() {};
};

class PlayerAi : public Ai {
public:
	void update(Actor* owner);

protected:
	bool moveOrAttack(Actor* owner, int targetX, int targetY);
	Actor* chooseFromInventory(Actor* owner);

private:
	void handleActionKey(Actor* owner, int ascii);
};

class MonsterAi : public Ai {
public:
	void update(Actor* owner);

protected:
	int moveCount;

	void moveOrAttack(Actor* owner, int targetX, int targetY);
};
