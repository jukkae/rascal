#ifndef STATE_HPP
#define STATE_HPP
#include "boost/serialization/base_object.hpp"
#include "boost/serialization/assume_abstract.hpp"
#include "engine.hpp"
#include <SFML/Window.hpp>
class State {
public:
	State(Engine* engine): engine(engine) {;}

	virtual void handleEvents() = 0;
	virtual void update(sf::RenderWindow& window) = 0;
	virtual void render(sf::RenderWindow& window) = 0; // TODO should be const

	void changeState(std::unique_ptr<State> state) { engine->changeState(std::move(state)); }

protected:
	Engine* engine;

private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {

	}

};
BOOST_SERIALIZATION_ASSUME_ABSTRACT(State)
#endif /* STATE_HPP */
