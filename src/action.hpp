#ifndef ACTION_HPP
#define ACTION_HPP

struct ActionResult;

#include "direction.hpp"
#include "point.hpp"
#include <variant>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/optional.hpp>

class Actor;

enum class ActionRange { ON_TOP, NEXT_TO, ANYWHERE };

class Action {
public:
	Action(Actor* actor, std::variant<ActionRange, float> actionRange, float length) : actionRange(actionRange), actor(actor), length(length) {;}
	virtual ~Action() {}
	virtual bool execute() = 0;
	float getLength() { return length; }
	std::variant<ActionRange, float> actionRange;
protected:
	Actor* actor;
	float length;
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & actor;
		ar & length;
		ar & actionRange;
	}
};

class EmptyAction : public Action {
public:
	EmptyAction(Actor* actor) : Action(actor, ActionRange::ANYWHERE, 0.0f) {;}
	bool execute() { return true; }
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Action);
	}
};

class WaitAction : public Action {
public:
	WaitAction(Actor* actor) : Action(actor, ActionRange::ANYWHERE, 100.0f) {;}
	bool execute();
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Action);
	}
};

class MoveAction : public Action {
public:
	MoveAction(Actor* actor, Direction direction) : Action(actor, ActionRange::ANYWHERE, 100.0f), direction(direction)
	{
		if(direction == Direction::NE || direction == Direction::SE ||
		   direction == Direction::SW || direction == Direction::NW) {
			this->length = 141; // sqrt(2) * 100
		}
	}
	bool execute();
private:
	Direction direction;

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Action);
		ar & direction;
	}
};

class HitAction : public Action {
public:
	HitAction(Actor* actor, Point target);
	bool execute();
	Point target;
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Action);
		ar & target;
	}
};

class TraverseStairsAction : public Action {
public:
	TraverseStairsAction(Actor* actor, bool down) : Action(actor, ActionRange::ON_TOP, 100.0f), down(down) {;}
	bool execute();
private:
	bool down;

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Action);
		ar & down;
	}
};

class PickupAction : public Action {
public:
	PickupAction(Actor* actor) : Action(actor, ActionRange::ON_TOP, 100.0f) {;}
	bool execute();
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Action);
	}
};

class UseItemAction : public Action {
public:
	UseItemAction(Actor* actor, Actor* item) : Action(actor, ActionRange::ANYWHERE, 100.0f), item(item) {;}
	bool execute();
private:
	Actor* item;
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Action);
		ar & item;
	}
};

class DropItemAction : public Action {
public:
	DropItemAction(Actor* actor, Actor* item) : Action(actor, ActionRange::ANYWHERE, 50.0f), item(item) {;}
	bool execute();
private:
	Actor* item;
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Action);
		ar & item;
	}
};

class ThrowItemAction : public Action {
public:
	ThrowItemAction(Actor* actor, Actor* item) : Action(actor, ActionRange::ANYWHERE, 50.0f), item(item) {;}
	bool execute();
private:
	Actor* item;
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Action);
		ar & item;
	}
};

class WieldItemAction : public Action {
public:
	WieldItemAction(Actor* actor, Actor* item) : Action(actor, ActionRange::ANYWHERE, 50.0f), item(item) {;}
	bool execute();
private:
	Actor* item;
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Action);
		ar & item;
	}
};

class UnWieldItemAction : public Action {
public:
	Actor* item;
	UnWieldItemAction(Actor* actor, Actor* item) : Action(actor, ActionRange::ANYWHERE, 50.0f), item(item) {;}
	bool execute();
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Action);
		ar & item;
	}
};

class LookAction : public Action {
public:
	LookAction(Actor* actor);
	LookAction(Actor* actor, Point location);
	bool execute();
	Point location;
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Action);
		ar & location;
	}
};

class ShootAction : public Action {
public:
	ShootAction(Actor* actor, Point target);
	bool execute();
	Point target;
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Action);
		ar & target;
	}
};

class EatAction : public Action {
public:
	EatAction(Actor* actor, Actor* item) : Action(actor, ActionRange::ANYWHERE, 200.0f), item(item) {;}
	bool execute();
private:
	Actor* item;
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Action);
		ar & item;
	}
};

class OpenAction : public Action {
public:
	OpenAction(Actor* actor, Actor* target) : Action(actor, ActionRange::NEXT_TO, 100.0f), target(target) {;}
	bool execute();
private:
	Actor* target;
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Action);
		ar & target;
	}
};

class TalkAction : public Action {
public:
	TalkAction(Actor* actor) : Action(actor, ActionRange::NEXT_TO, 100.0f) {;}
	bool execute();
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Action);
	}
};

struct ActionResult {
	bool succeeded;
	boost::optional<Action> alternativeAction; // TODO should use std::optional
};


#endif /* ACTION_HPP */
