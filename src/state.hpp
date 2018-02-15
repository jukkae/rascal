#ifndef STATE_HPP
#define STATE_HPP
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/assume_abstract.hpp>
#include "engine.hpp"
#include <SFML/Window.hpp>
#include "console.hpp"
class State {
public:
	State(Engine* engine, sf::RenderWindow* window): engine(engine), window(window), console() {;}
	virtual ~State() {}

	virtual void handleEvents() = 0;
	virtual void update() = 0;
	virtual void render() = 0;

	void changeState(std::unique_ptr<State> state) { engine->changeState(std::move(state)); }

protected:
	Engine* engine;
	sf::RenderWindow* window;
	Console console;

private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) { }
};
BOOST_SERIALIZATION_ASSUME_ABSTRACT(State)
#endif /* STATE_HPP */
