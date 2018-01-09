#ifndef ACTION_HPP
#define ACTION_HPP

struct ActionResult;

#include "actor.hpp"
#include "direction.hpp"
#include "point.hpp"
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/optional.hpp>


class Action {
public:
	Action(Actor* actor, float length = 100.0f) : actor(actor), length(length) {;}
	virtual ~Action() {}
	virtual bool execute() = 0;
	float getLength() { return length; }
protected:
	Actor* actor;
	float length;
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & actor;
		ar & length;
	}
};

class EmptyAction : public Action {
public:
	EmptyAction(Actor* actor) : Action(actor, 0.0f) {;}
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
	WaitAction(Actor* actor) : Action(actor, 100.0f) {;}
	bool execute() { return true; }
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Action);
	}
};

class MoveAction : public Action {
public:
	MoveAction(Actor* actor, Direction direction) : Action(actor, 100.0f), direction(direction) {;}
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

class TraverseStairsAction : public Action {
public:
	TraverseStairsAction(Actor* actor, bool down) : Action(actor, 100.0f), down(down) {;}
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
	PickupAction(Actor* actor) : Action(actor, 100.0f) {;}
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
	UseItemAction(Actor* actor, Actor* item) : Action(actor, 100.0f), item(item) {;}
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
	DropItemAction(Actor* actor, Actor* item) : Action(actor, 50.0f), item(item) {;}
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
	WieldItemAction(Actor* actor, Actor* item) : Action(actor, 50.0f), item(item) {;}
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
	UnWieldItemAction(Actor* actor) : Action(actor, 50.0f) {;}
	bool execute();
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Action);
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

struct ActionResult {
	bool succeeded;
	boost::optional<Action> alternativeAction;
};


#endif /* ACTION_HPP */
