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

class GameplayState : public State {
public:
	void init(Engine* engine) override;
	void cleanup() override;

	void handleEvents(Engine* engine) override;
	void update(Engine* engine, sf::RenderWindow& window) override;
	void render(Engine* engine, sf::RenderWindow& window) override;

	void initLoaded(Engine* engine);
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
	Engine* getEngine() { return e; }

	Actor* getNextActor() const { return actors.front().get(); }
	Actor* getPlayer() const;
	Actor* getStairs() const;
	Actor* getClosestMonster(int x, int y, float range) const;
	Actor* getLiveActor(int x, int y) const;
	std::vector<std::unique_ptr<Actor>>& getActors() { return actors; }

	void addActor(std::unique_ptr<Actor> actor) { actors.push_back(std::move(actor)); }

	bool pickTile(int* x, int* y, float maxRange = 0.0f);
private:
	Engine* e; // TODO
	int time = 0;
	int level = 1;
	std::vector<std::unique_ptr<Actor>> actors;
	Map map;
	Gui gui;
	Renderer renderer;
	sf::RenderWindow* window_ = nullptr; // TODO

	void updateTime();
	void updateNextActor();

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(State);
		ar & level;
		ar & time;
		ar & map;
		ar & actors;
		ar & gui;
	}
};
#endif /* GAMEPLAY_STATE_HPP */
