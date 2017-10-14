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
	int getLevel() { return level; } // temporary!
	void increaseLevel() { ++level; } // temporary!
	void nextLevel();
	int getTime() { return time; } // temporary!
	void updateTime();
	void increaseTime(int amount) { time += amount; } // temporary!
	bool isInFov(int x, int y) { return map->isInFov(x, y); }
	Point getWorldCoordsFromScreenCoords(Point& point) { return renderer->getWorldCoordsFromScreenCoords(point); }
	Point getMouseLocation();
	void message(const TCODColor& col, std::string text, ...);
	void computeFov() { map->computeFov(); }
	bool isWall(int x, int y) { return map->isWall(x, y); }
	bool canWalk(int x, int y);
	void markExploredTiles() { map->markExploredTiles(); }
	void showLevelUpMenu();
	Menu::MenuItemCode pickFromMenu(Menu::DisplayMode mode = Menu::DisplayMode::PAUSE) { return gui->menu.pick(mode); }
	Engine* getEngine() { return e; }

	Actor* getNextActor() const { return actors->at(0); }
	void updateNextActor();
	Actor* getPlayer() const;
	Actor* getStairs() const { return stairs; }
	Actor* getClosestMonster(int x, int y, float range) const;
	Actor* getLiveActor(int x, int y) const;
	std::vector<Actor*>* getActors() { return actors.get(); }

	void addActor(Actor* actor) { actors->push_back(actor); }

	bool pickTile(int* x, int* y, float maxRange = 0.0f);

	std::unique_ptr<InputHandler> inputHandler; // TODO private? singleton? something else?
private:
	Engine* e; // TODO
	int time = 0;
	int level = 1;
	std::shared_ptr<std::vector<Actor*>> actors = std::make_shared<std::vector<Actor*>>(); // s.b. unique_ptr
	Actor* player; // TODO fix reliance on explicitly pointing to player
	Actor* stairs; // likewise, this feels bad
	std::unique_ptr<Map> map;
	std::unique_ptr<Gui> gui;
	std::unique_ptr<Renderer> renderer;

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(State);
		ar & level;
		ar & time;
		ar & map;
		ar & stairs;
		ar & player;
		ar & actors;
		ar & gui;
	}
};
#endif /* GAMEPLAY_STATE_HPP */
