#include "constants.hpp"
#include "gameplay_state.hpp"
#include "engine.hpp"
#include "map_utils.hpp"
#include "victory_state.hpp"
#include <SFML/Window/Mouse.hpp>
#include <boost/optional/optional_io.hpp>

GameplayState::GameplayState(Engine* engine, sf::RenderWindow* window) :
State(engine, window) {
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
	gui.setState(this);
	renderer.setState(this);
	map.setState(this);
	for (auto& a : actors) a->setState(this);
}

void GameplayState::newGame(Engine* engine) {
	map_utils::addPlayer(this, &map);
	map_utils::addStairs(this, &map);
	map_utils::addMcGuffin(this, &map, level);
	gui.message(sf::Color::Green, "Welcome to year 20XXAD, you strange rascal!\nPrepare to fight or die!");
}

void GameplayState::update() {
	Actor* activeActor = getNextActor();
	if(activeActor->isPlayer()) {
		handleEvents();
	}
	updateNextActor();
	if(activeActor->isPlayer()) {
		computeFov();
		render();
	}
}

void GameplayState::handleEvents() {
	//inputHandler->handleEvents();
}

void GameplayState::render() {
	window->clear(sf::Color::Black);

	renderer.render(&map, actors, window);
	gui.render(window);

	window->display();
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

// Messaging system
void GameplayState::message(sf::Color col, std::string text, ...) {
	va_list args;
	va_start(args, text);
	gui.message(col, text, args);
	va_end(args);
}

// Bulk: map helper?
void GameplayState::nextLevel() {
	++level;
	if(level > 5) {
		std::unique_ptr<State> victoryState = std::make_unique<VictoryState>(engine, getPlayer());
		engine->pushState(std::move(victoryState));
	}
	gui.message(sf::Color::Magenta, "You take a moment to rest, and recover your strength.");
	getPlayer()->destructible->heal(getPlayer()->destructible->maxHp/2);
	gui.message(sf::Color::Red, "After a rare moment of peace, you climb\nhigher. You will escape this hellhole.");

	// Clunky, not idiomatic
	auto it = actors.begin();
	while (it != actors.end()) {
		if (!((*it)->isPlayer())) {
			it = actors.erase(it);
		}
		else ++it;
	}

	map = Map(120, 72);
	map.setState(this);
	if(level == 3) map.generateMap(MapType::PILLARS);
	else map.generateMap(MapType::BUILDING);

	map_utils::addItems(this, &map);
	map_utils::addMonsters(this, &map);
	map_utils::addStairs(this, &map);
	map_utils::addMcGuffin(this, &map, level);

	sortActors();
	for (auto& a : actors) a->setState(this);
}

// TODO this doesn't belong to gameplay state
// --> UI / Renderer / IO
bool GameplayState::pickTile(int* x, int* y, float maxRange) { // Should be moved over to renderer
	while(true) {
		sf::Event event;
		while(window->pollEvent(event)) { // Dummy polling to keep macOS happy
			render();
			for(int cx = 0; cx < constants::SQUARE_SCREEN_WIDTH; ++cx) {
				for(int cy = 0; cy < constants::SCREEN_HEIGHT; ++cy) {
					Point location = renderer.getWorldCoordsFromScreenCoords(Point(cx, cy));
					int realX = location.x;
					int realY = location.y;
					if(isInFov(realX, realY) && (maxRange == 0 || getPlayer()->getDistance(realX, realY) <= maxRange)) {
						// TODO Highlight background for tiles in range
					}
				}
			}
			int mouseXPix = sf::Mouse::getPosition(*window).x;
			int mouseYPix = sf::Mouse::getPosition(*window).y;
			int xCells = mouseXPix / constants::SQUARE_CELL_WIDTH;
			int yCells = mouseYPix / constants::SQUARE_CELL_HEIGHT;
			Point mouseLocationScreen(xCells, yCells);
			Point mouseLocation = renderer.getWorldCoordsFromScreenCoords(Point(mouseLocationScreen.x, mouseLocationScreen.y));
			int realX = mouseLocation.x;
			int realY = mouseLocation.y;
			if(isInFov(realX, realY) && (maxRange == 0 || getPlayer()->getDistance(realX, realY) <= maxRange)) {
				// Highlight background for tile under cursor: Done globally
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
}

BOOST_CLASS_EXPORT(GameplayState)
