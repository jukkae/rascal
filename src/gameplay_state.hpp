#ifndef GAMEPLAY_STATE_HPP
#define GAMEPLAY_STATE_HPP
#include "state.hpp"
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include "engine.hpp"
#include "gui.hpp"
#include "player.hpp"
#include "point.hpp"
#include "renderer.hpp"
#include <SFML/Graphics.hpp>
#include <boost/serialization/vector.hpp>

struct Event;
class World;
class GameplayState : public State {
public:
	GameplayState(Engine* engine, sf::RenderWindow* window);

	void handleEvents() override;
	void update() override;
	void render() override;

	void initLoaded(Engine* engine);
	void newGame(Engine* engine);

	void nextLevel();
	void previousLevel();
	Point getWorldCoordsFromScreenCoords(Point& point) { return renderer.getWorldCoordsFromScreenCoords(point); }

	void notify(Event& event);
	void message(sf::Color col, std::string text, ...);
	Engine* getEngine() { return engine; }
	void setEngine(Engine* e) { engine = e; }
	void setWindow(sf::RenderWindow* w) { window = w; }

	World* world = nullptr; // TODO should be private
	std::unique_ptr<Player> player = std::make_unique<Player>();
private:
	Gui gui;
	Renderer renderer;
	std::vector<std::unique_ptr<World>> levels;

	friend class boost::serialization::access;
	template<class Archive>
	friend void boost::serialization::save_construct_data(Archive & ar, const GameplayState* s, const unsigned int version);
	template<class Archive>
	friend void boost::serialization::load_construct_data(Archive & ar, GameplayState* s, const unsigned int version);

};

namespace boost {
namespace serialization {
	template<class Archive>
	void save_construct_data(Archive & ar, const GameplayState* s, const unsigned int version)
	{
		boost::serialization::void_cast_register<GameplayState, State>();
		ar & s->gui;
		ar & s->world;
		ar & s->levels;
		ar & s->player;
	}

	template<class Archive>
	void load_construct_data(Archive & ar, GameplayState* s, const unsigned int version)
	{
		boost::serialization::void_cast_register<GameplayState, State>();
		Engine* engine = nullptr;
		sf::RenderWindow* window = nullptr;
		::new(s)GameplayState(engine, window);
		ar & s->gui;
		ar & s->world;
		ar & s->levels;
		ar & s->player;
	}
} // namespace boost
} // namespace serialization
#endif /* GAMEPLAY_STATE_HPP */
