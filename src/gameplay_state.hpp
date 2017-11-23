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
#include <SFML/Graphics.hpp>
#include <boost/serialization/vector.hpp>

class GameplayState : public State {
public:
	GameplayState(Engine* engine);

	void handleEvents(Engine* engine) override;
	void update(Engine* engine, sf::RenderWindow& window) override;
	void render(Engine* engine, sf::RenderWindow& window) override;

	void initLoaded(Engine* engine);
	void newGame(Engine* engine);
	// TEMPORARY FUNCTIONS FOR REFACTORING PROCESS
	int getLevel() { return level; }
	void nextLevel();
	int getTime() { return time; }
	bool isInFov(int x, int y) { return map.isInFov(x, y); }
	Point getWorldCoordsFromScreenCoords(Point& point) { return renderer.getWorldCoordsFromScreenCoords(point); }
	void message(sf::Color col, std::string text, ...);
	void computeFov() { map.computeFov(); }
	bool isWall(int x, int y) { return map.isWall(x, y); }
	bool canWalk(int x, int y);
	Engine* getEngine() { return engine; }
	void setEngine(Engine* e) { engine = e; }

	Actor* getNextActor() const { return actors.front().get(); }
	Actor* getPlayer() const;
	Actor* getStairs() const;
	Actor* getClosestMonster(int x, int y, float range) const;
	Actor* getLiveActor(int x, int y) const;
	std::vector<std::unique_ptr<Actor>>& getActors() { return actors; }

	void addActor(std::unique_ptr<Actor> actor) { actors.push_back(std::move(actor)); }

	bool pickTile(int* x, int* y, float maxRange = 0.0f);
private:
	int time = 0;
	int level = 1;
	std::vector<std::unique_ptr<Actor>> actors;
	Map map;
	Gui gui;
	Renderer renderer;
	sf::RenderWindow* window_ = nullptr; // TODO

	void updateTime();
	void updateNextActor();
	void sortActors();

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
		ar & s->level;
		ar & s->time;
		ar & s->map;
		ar & s->actors;
		ar & s->gui;
	}

	template<class Archive>
	void load_construct_data(Archive & ar, GameplayState* s, const unsigned int version)
	{
		boost::serialization::void_cast_register<GameplayState, State>();
		Engine* engine = nullptr; // TODO how to get this?
		::new(s)GameplayState(engine);
		ar & s->level;
		ar & s->time;
		ar & s->map;
		ar & s->actors;
		ar & s->gui;
	}
} // namespace boost
} // namespace serialization
#endif /* GAMEPLAY_STATE_HPP */
