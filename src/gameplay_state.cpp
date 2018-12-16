#include "ai.hpp"
#include "attacker.hpp"
#include "body.hpp"
#include "constants.hpp"
#include "container.hpp"
#include "gameplay_state.hpp"
#include "gameover_state.hpp"
#include "level_up_menu_state.hpp"
#include "destructible.hpp"
#include "effect.hpp"
#include "engine.hpp"
#include "event.hpp"
#include "map_utils.hpp"
#include "pickable.hpp"
#include "transporter.hpp"
#include "world.hpp"
#include <chrono>
#include <SFML/Window/Mouse.hpp>
#include <boost/optional/optional_io.hpp>

GameplayState::GameplayState(Engine* engine, sf::RenderWindow* window) :
State(engine, window) {
	std::unique_ptr<World> w = std::make_unique<World>(120, 72, 1, this);
	levels.push_back(std::move(w));
	world = levels.front().get();

	gui.setState(this);
	renderer.setState(this);
	newGame(engine);
}

void GameplayState::initLoaded(Engine* engine) {
	gui.setState(this);
	renderer.setState(this);
}

void GameplayState::newGame(Engine* engine) {
	map_utils::addPlayer(world, &world->map);
	map_utils::addStairs(world, &world->map);

	world->sortActors();

	world->movePlayerFrom(world); // TODO

	gui.message(sf::Color::Green, "Welcome to year 20XXAD, you strange rascal!\nPrepare to fight or die!");
}

void GameplayState::update() {
	auto previous = std::chrono::system_clock::now();
	double frameLength = 1.0 / 60.0; // 60 Hz, in seconds
	while(true) { // TODO events are not polled correctly
		world->update();

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
	//window->clear(sf::Color::Black);

	renderer.render(world, window);
	gui.render(world, window);

	window->display();
}

void GameplayState::notify(Event& event) {
	//TODO proper filtering
	if(auto e = dynamic_cast<PlayerDeathEvent*>(&event)) {
		std::unique_ptr<State> gameOverState = std::make_unique<GameOverState>(engine, e->actor, player.get());
		engine->pushState(std::move(gameOverState));
	}
	if(auto e = dynamic_cast<PlayerStatChangeEvent*>(&event)) {
		std::unique_ptr<State> levelUpMenuState = std::make_unique<LevelUpMenuState>(engine, e->actor);
		engine->pushState(std::move(levelUpMenuState));
	}
	gui.notify(event);
	player->notify(event);
	renderer.notify(event, world); // TODO nasty
	//audioSystem.notify(event);
}

void GameplayState::message(sf::Color col, std::string text, ...) {
	va_list args;
	va_start(args, text);
	gui.message(col, text, args);
	va_end(args);
}

// Bulk: map helper?
// FIXME ugly and brittle
void GameplayState::nextLevel() {
	Actor* downstairs = nullptr;
	for(auto& a : world->actors) {
		if(a->name == "stairs (up)") {
			downstairs = a.get();
		}
	}
	int downstairsX = 0;
	int downstairsY = 0;
	if(downstairs) {
		downstairsX = downstairs->x;
		downstairsY = downstairs->y;
	}

	if(levels.size() > world->level) {
		std::unique_ptr<Actor> player;
		auto it = world->actors.begin();
		while (it != world->actors.end()) {
			if ((*it)->isPlayer()) {
				player = std::move(*it); // i want to move(*it) move(*it)
				it = world->actors.erase(it);
			}
			else ++it;
		}
		world = levels.at(world->level).get();
		for (auto& a : player->container->inventory) a->world = world;
		world->addActor(std::move(player));
		world->sortActors();
		return;
	}

	int newLevel = world->level + 1;

	if(newLevel > 5) {
		std::unique_ptr<State> victoryState = std::make_unique<GameOverState>(engine, world->getPlayer(), player.get(), true);
		engine->pushState(std::move(victoryState));
	}

	std::unique_ptr<World> w = std::make_unique<World>(120, 72, newLevel, this);
	w->time = world->time;

	w->movePlayerFrom(world);

	gui.message(sf::Color::Magenta, "You take a moment to rest, and recover your strength.");
	world->getPlayer()->destructible->heal(world->getPlayer()->destructible->maxHp/2);
	gui.message(sf::Color::Red, "After a rare moment of peace, you climb\nhigher. You will escape this hellhole.");

	std::unique_ptr<Actor> player;
	auto it = world->actors.begin();
	while (it != world->actors.end()) {
		if ((*it)->isPlayer()) {
			player = std::move(*it); // i want to move(*it) move(*it)
			it = world->actors.erase(it);
		}
		else ++it;
	}

	levels.push_back(std::move(w));
	world = levels.back().get();

	if(downstairs) {
		map_utils::addStairs(world, &world->map, downstairsX, downstairsY);
	} else {
		map_utils::addStairs(world, &world->map);
	}

	for (auto& a : player->container->inventory) a->world = world;
	world->addActor(std::move(player));

	world->sortActors();
}

void GameplayState::previousLevel() {
	if(world->level <= 1) return;

	std::unique_ptr<Actor> player;
	auto it = world->actors.begin();
	while (it != world->actors.end()) {
		if ((*it)->isPlayer()) {
			player = std::move(*it); // i want to move(*it) move(*it)
			it = world->actors.erase(it);
		}
		else ++it;
	}

	world = levels.at(world->level-2).get();
	for (auto& a : player->container->inventory) a->world = world;
	world->addActor(std::move(player));
	world->sortActors();
}

BOOST_CLASS_EXPORT(GameplayState)
