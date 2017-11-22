#include "constants.hpp"
#include "gameplay_state.hpp"
#include "engine.hpp"
#include "map_utils.hpp"
#include "victory_state.hpp"
#include <SFML/Window/Mouse.hpp>
#include <boost/optional/optional_io.hpp>

GameplayState::GameplayState() {

}

void GameplayState::init(Engine* engine) {
	e = engine;
	gui.setState(this);
	renderer.setState(this);

	newGame(engine);

	map = Map(120, 72);
	map.setState(this);
	map.generateMap();

	map_utils::addItems(this, &map);
	map_utils::addMonsters(this, &map);

	// not really the correct place for following, but w/e
	for (auto& a : actors) a->setState(this);
	sortActors();
}

void GameplayState::initLoaded(Engine* engine) {
	e = engine;
	gui.setState(this);
	renderer.setState(this);
	map.setState(this);
	for (auto& a : actors) a->setState(this);
}

void GameplayState::newGame(Engine* engine) {
	std::unique_ptr<Actor> player = std::make_unique<Actor>(40, 25, '@', "you", sf::Color::White, 2);
	player->destructible = std::make_unique<PlayerDestructible>(100, 2, 0, "your corpse", 11);
	player->attacker     = std::make_unique<Attacker>(5);
	player->ai           = std::make_unique<PlayerAi>();
	player->container    = std::make_unique<Container>(26);
	addActor(std::move(player));

	std::unique_ptr<Actor> stairs = std::make_unique<Actor>(1, 1, '>', "stairs", sf::Color::White, 0, true);
    stairs->blocks = false;
    stairs->fovOnly = false;
	addActor(std::move(stairs));

	gui.message(sf::Color::Green, "Welcome to year 20XXAD, you strange rascal!\nPrepare to fight or die!");
}

void GameplayState::update(Engine* engine, sf::RenderWindow& window) {
	//window.clear(sf::Color::Black);
	if(!window_) window_ = &window; // TODO
	Actor* activeActor = getNextActor();
	if(activeActor->isPlayer()) {
		handleEvents(engine);
	}
	updateNextActor();
	if(activeActor->isPlayer()) {
		computeFov();
		render(engine, window);
	}
	//window.display();
}

void GameplayState::handleEvents(Engine* engine) {
	//inputHandler->handleEvents();
}

void GameplayState::render(Engine* engine, sf::RenderWindow& window) {
	window.clear(sf::Color::Black);

	renderer.render(&map, actors, window);
	gui.render(window);

	window.display();
}

void GameplayState::updateNextActor() {
	Actor* activeActor = getNextActor();

	float actionTime = activeActor->update(this);
	*activeActor->energy -= actionTime;

    /*actors.erase(actors.begin());
    auto it = std::lower_bound(actors.begin(), actors.end(), activeActor, [](const auto& lhs, const auto& rhs) { return lhs->energy > rhs->energy; });
    actors.insert(it, std::move(activeActor));*/

	sortActors();
	updateTime();
}

void GameplayState::updateTime() {
	if(!actors.front()->energy) return;
	if(actors.front()->energy.get() > 0) return;
	else {
		Actor* next = std::find_if(actors.begin(), actors.end(), [](const auto& a) { return a->ai != nullptr; })->get();

		float tuna = next->energy.get() * -1;
		time += tuna;

		for(auto& a : actors) {
			if(a->ai != nullptr) *a->energy += tuna;
		}
	}
}

void GameplayState::sortActors() {
    std::sort(actors.begin(), actors.end(), [](const auto& lhs, const auto& rhs)
    {
        return lhs->energy > rhs->energy;
    });
}

Actor* GameplayState::getPlayer() const {
	for(auto& actor : actors) {
        if(actor->isPlayer()) return actor.get();
    }
    return nullptr;
}

Actor* GameplayState::getStairs() const {
	for(auto& actor : actors) {
        if(actor->isStairs()) return actor.get();
    }
    return nullptr;
}

bool GameplayState::canWalk(int x, int y) {
	for(auto& actor : actors) {
		if(actor->blocks && actor->x == x && actor->y == y) {
			return false;
		}
	}
	return true;
}

Actor* GameplayState::getClosestMonster(int x, int y, float range) const {
	Actor* closest = nullptr;
	float bestDistance = std::numeric_limits<float>::max();
	for (auto& actor : actors) {
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
	for(auto& actor : actors) {
		if(actor->x == x && actor->y == y && actor->destructible && !actor->destructible->isDead()) return actor.get();
	}
	return nullptr;
}

void GameplayState::message(sf::Color col, std::string text, ...) {
	va_list args;
	va_start(args, text);
	gui.message(col, text, args);
	va_end(args);
}

void GameplayState::nextLevel() {
	++level;
	if(level > 5) {
		std::unique_ptr<State> victoryState = std::make_unique<VictoryState>(getPlayer());
		victoryState->init(e);
		e->pushState(std::move(victoryState));
	}
	gui.message(sf::Color::Magenta, "You take a moment to rest, and recover your strength.");
	getPlayer()->destructible->heal(getPlayer()->destructible->maxHp/2);
	gui.message(sf::Color::Red, "After a rare moment of peace, you descend\ndeeper into the heart of the dungeon...");

	// Clunky, not idiomatic
	auto it = actors.begin();
	while (it != actors.end()) {
		if (!((*it)->isPlayer()) && !((*it)->isStairs())) {
			it = actors.erase(it);
		}
		else ++it;
	}

	// gameplayState.initMap() or something like that, remember to init actors
	map = Map(120, 72);
	map.setState(this);
	if(level == 3) map.generateMap(MapType::PILLARS);
	else map.generateMap(MapType::BUILDING);

	map_utils::addItems(this, &map);
	map_utils::addMonsters(this, &map);

	sortActors();
	for (auto& a : actors) a->setState(this);
}

bool GameplayState::pickTile(int* x, int* y, float maxRange) {
	while(true) { // TODO this function hangs (doesn't freeze)
		for(int cx = 0; cx < constants::SCREEN_WIDTH; ++cx) {
			for(int cy = 0; cy < constants::SCREEN_HEIGHT; ++cy) {
				Point location = renderer.getWorldCoordsFromScreenCoords(Point(cx, cy));
				int realX = location.x;
				int realY = location.y;
				if(isInFov(realX, realY) && (maxRange == 0 || getPlayer()->getDistance(realX, realY) <= maxRange)) {
					// TODO Highlight background for tiles in range
				}
			}
		}
		sf::RenderWindow& window = *window_; // TODO
		render(nullptr, window); // TODO
		int mouseXPix = sf::Mouse::getPosition(window).x;
		int mouseYPix = sf::Mouse::getPosition(window).y;
		int xCells = mouseXPix / constants::CELL_WIDTH;
		int yCells = mouseYPix / constants::CELL_HEIGHT;
		Point mouseLocationScreen(xCells, yCells);
		Point mouseLocation = renderer.getWorldCoordsFromScreenCoords(Point(mouseLocationScreen.x, mouseLocationScreen.y));
		int realX = mouseLocation.x;
		int realY = mouseLocation.y;
		if(isInFov(realX, realY) && (maxRange == 0 || getPlayer()->getDistance(realX, realY) <= maxRange)) {
			// TODO Highlight background for tile under cursor
			if(sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
				*x = realX;
				*y = realY;
				return true;
			}
		}
		if(sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
			return false;
		}
	}
}

BOOST_CLASS_EXPORT(GameplayState)
