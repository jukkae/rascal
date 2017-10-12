#ifndef STATE_HPP
#define STATE_HPP
#include "boost/serialization/base_object.hpp"
#include "boost/serialization/assume_abstract.hpp"
#include "engine.hpp"
class State {
public:
	virtual void init(Engine* engine) = 0;
	virtual void cleanup() = 0;

	virtual void handleEvents(Engine* engine) = 0;
	virtual void update(Engine* engine) = 0;
	virtual void render(Engine* engine) = 0; // TODO should be const

	void changeState(Engine* engine, State* state) { engine->changeState(state); } // TODO not sure if necessary

// protected: State() { } TODO for implementing states as singletons
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {

	}

};
BOOST_SERIALIZATION_ASSUME_ABSTRACT(State)
#endif /* STATE_HPP */
