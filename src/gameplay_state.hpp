#ifndef GAMEPLAY_STATE_HPP
#define GAMEPLAY_STATE_HPP
#include "state.hpp"
class Actor;
class Engine;
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include "gui.hpp"
#include "map.hpp"
#include "point.hpp"
#include "renderer.hpp"
#include "persistent.hpp"
#include "world.hpp"
#include <SFML/Graphics.hpp>
#include <boost/serialization/vector.hpp>

class GameplayState : public State {
public:
	GameplayState(Engine* engine, sf::RenderWindow* window);

	void handleEvents() override;
	void update() override;
	void render() override;

	void initLoaded(Engine* engine);
	void newGame(Engine* engine);

	void nextLevel();
	bool isInFov(int x, int y) { return world.map.isInFov(x, y); }
	Point getWorldCoordsFromScreenCoords(Point& point) { return renderer.getWorldCoordsFromScreenCoords(point); }
	void message(sf::Color col, std::string text, ...);
	Engine* getEngine() { return engine; }
	void setEngine(Engine* e) { engine = e; }

	Actor* getPlayer() const { return world.getPlayer(); }
	Actor* getClosestMonster(int x, int y, float range) const;
	Actor* getLiveActor(int x, int y) const;
	std::vector<std::unique_ptr<Actor>>& getActors() { return world.actors; }
	void setWindow(sf::RenderWindow* w) { window = w; }

	void addActor(std::unique_ptr<Actor> actor) { world.actors.push_back(std::move(actor)); }
private:
	Gui gui;
	Renderer renderer;
	World world;

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
	}

	template<class Archive>
	void load_construct_data(Archive & ar, GameplayState* s, const unsigned int version)
	{
		boost::serialization::void_cast_register<GameplayState, State>();
		Engine* engine = nullptr;
		sf::RenderWindow* window = nullptr;
		::new(s)GameplayState(engine, window);
		ar & s->gui;
		ar & s-> world;
	}
} // namespace boost
} // namespace serialization
#endif /* GAMEPLAY_STATE_HPP */
