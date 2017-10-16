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
#include "input_handler.hpp"
#include "persistent.hpp"

class GameplayState : public State {
public:
	void init(Engine* engine) override;
	void cleanup() override;

	void handleEvents(Engine* engine) override;
	void update(Engine* engine) override;
	void render(Engine* engine) override;

	void initLoaded(Engine* engine);
	// TEMPORARY FUNCTIONS FOR REFACTORING PROCESS
	int getLevel() { return level; }
	void nextLevel();
	int getTime() { return time; }
	bool isInFov(int x, int y) { return map->isInFov(x, y); }
	Point getWorldCoordsFromScreenCoords(Point& point) { return renderer->getWorldCoordsFromScreenCoords(point); }
	Point getMouseLocation();
	void message(const TCODColor& col, std::string text, ...);
	void computeFov() { map->computeFov(); }
	bool isWall(int x, int y) { return map->isWall(x, y); }
	bool canWalk(int x, int y);
	void markExploredTiles() { map->markExploredTiles(); }
	Engine* getEngine() { return e; }

	Actor* getNextActor() const { return actors.front(); }
	Actor* getPlayer() const;
	Actor* getStairs() const;
	Actor* getClosestMonster(int x, int y, float range) const;
	Actor* getLiveActor(int x, int y) const;
	std::vector<Actor*>* getActors() { return &actors; } // TODO ultimately get rid of pointers

	void addActor(Actor* actor) { actors.push_back(actor); }

	bool pickTile(int* x, int* y, float maxRange = 0.0f);

	std::unique_ptr<InputHandler> inputHandler; // TODO private? singleton? something else?
private:
	Engine* e; // TODO
	int time = 0;
	int level = 1;
	std::vector<Actor*> actors;
	std::unique_ptr<Map> map;
	std::unique_ptr<Gui> gui;
	std::unique_ptr<Renderer> renderer;

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
