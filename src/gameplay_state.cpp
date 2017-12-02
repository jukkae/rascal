#include "constants.hpp"
#include "gameplay_state.hpp"
#include "engine.hpp"
#include "map_utils.hpp"
#include "victory_state.hpp"
#include <chrono>
#include <SFML/Window/Mouse.hpp>
#include <boost/optional/optional_io.hpp>

GameplayState::GameplayState(Engine* engine, sf::RenderWindow* window) :
State(engine, window) {
	world = World(120, 72);
	world.state = this;

	gui.setState(this);
	renderer.setState(this);
	world.map.setState(this);
	world.map.setWorld(&world);
	world.map.generateMap();
	newGame(engine);

	// not really the correct place for following, but w/e
	for (auto& a : world.actors) a->setState(this);
	for (auto& a : world.actors) a->world = &world;
	world.sortActors();
}

void GameplayState::initLoaded(Engine* engine) {
	gui.setState(this);
	renderer.setState(this);
	world.map.setState(this);
	for (auto& a : world.actors) a->setState(this);
}

void GameplayState::newGame(Engine* engine) {
	map_utils::addPlayer(this, &world.map);
	map_utils::addStairs(this, &world.map);
	map_utils::addMcGuffin(this, &world.map, world.level);
	map_utils::addItems(this, &world.map);
	map_utils::addMonsters(this, &world.map);
	gui.message(sf::Color::Green, "Welcome to year 20XXAD, you strange rascal!\nPrepare to fight or die!");
}

void GameplayState::update() {
	auto previous = std::chrono::system_clock::now();
	double frameLength = 1.0 / 60.0; // 60 Hz, in seconds
	while(true) { // TODO events are not polled correctly
		world.update();

		auto now = std::chrono::system_clock::now();
		std::chrono::duration<double> elapsed = now - previous;
		if(elapsed.count() > frameLength) break;
	}
	render();
}

void GameplayState::handleEvents() {
	// TODO poll events from here eventually?
}

void GameplayState::render() {
	window->clear(sf::Color::Black);

	renderer.render(&world, window);
	gui.render(&world, window);

	window->display();
}

Actor* GameplayState::getClosestMonster(int x, int y, float range) const {
	Actor* closest = nullptr;
	float bestDistance = std::numeric_limits<float>::max();
	for (auto& actor : world.actors) {
		if(!actor->isPlayer() && actor->destructible && !actor->destructible->isDead()) {
			float distance = actor->getDistance(x,y);
			if(distance < bestDistance && (distance <= range || range == 0.0f)) {
				bestDistance = distance;
				closest = actor.get();
			}
		}
	}
	return closest;
}

Actor* GameplayState::getLiveActor(int x, int y) const {
	for(auto& actor : world.actors) {
		if(actor->x == x && actor->y == y && actor->destructible && !actor->destructible->isDead()) return actor.get();
	}
	return nullptr;
}

// Messaging system
void GameplayState::message(sf::Color col, std::string text, ...) {
	va_list args;
	va_start(args, text);
	gui.message(col, text, args);
	va_end(args);
}

// Bulk: map helper?
void GameplayState::nextLevel() {
	++world.level;
	if(world.level > 5) {
		std::unique_ptr<State> victoryState = std::make_unique<VictoryState>(engine, world.getPlayer());
		engine->pushState(std::move(victoryState));
	}
	gui.message(sf::Color::Magenta, "You take a moment to rest, and recover your strength.");
	world.getPlayer()->destructible->heal(world.getPlayer()->destructible->maxHp/2);
	gui.message(sf::Color::Red, "After a rare moment of peace, you climb\nhigher. You will escape this hellhole.");

	// Clunky, not idiomatic
	auto it = world.actors.begin();
	while (it != world.actors.end()) {
		if (!((*it)->isPlayer())) {
			it = world.actors.erase(it);
		}
		else ++it;
	}

	world.map = Map(120, 72);
	world.map.setState(this);
	if(world.level == 3) world.map.generateMap(MapType::PILLARS);
	else world.map.generateMap(MapType::BUILDING);

	map_utils::addItems(this, &world.map);
	map_utils::addMonsters(this, &world.map);
	map_utils::addStairs(this, &world.map);
	map_utils::addMcGuffin(this, &world.map, world.level);

	world.sortActors();
	for (auto& a : world.actors) a->setState(this);
}

BOOST_CLASS_EXPORT(GameplayState)
