#include "ai.hpp"
#include "action.hpp"
#include "actor.hpp"
#include "attacker.hpp"
#include "body.hpp"
#include "colors.hpp"
#include "constants.hpp"
#include "container.hpp"
#include "destructible.hpp"
#include "dice.hpp"
#include "effect.hpp"
#include "engine.hpp"
#include "event.hpp"
#include "gameplay_state.hpp"
#include "gui.hpp"
#include "io.hpp"
#include "inventory_menu_state.hpp"
#include "los.hpp"
#include "map.hpp"
#include "pickable.hpp"
#include "persistent.hpp"
#include "point.hpp"
#include "state.hpp"
#include "level_up_menu_state.hpp"
#include "main_menu_state.hpp"
#include "world.hpp"
#include <cmath>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/optional.hpp>
#include <SFML/Window.hpp>

#include <unordered_map>

static const int TRACKING_TURNS = 3;

const int LEVEL_UP_BASE = 50;
const int LEVEL_UP_FACTOR = 150;

int PlayerAi::getNextLevelXp() const {
	return LEVEL_UP_BASE + xpLevel * LEVEL_UP_FACTOR;
}

void PlayerAi::increaseXp(Actor* owner, int xp) {
	experience += xp;
	if(experience >= getNextLevelXp()) {
		experience -= getNextLevelXp();
		++xpLevel;
		PlayerStatChangeEvent e(owner, true, xpLevel);
		owner->world->notify(e);
	}
}

std::unique_ptr<Action> PlayerAi::getNextAction(Actor* actor) {
	Direction dir = Direction::NONE;

	Engine* engine = io::engine;
	sf::Event event;
	while(engine->pollEvent(event)) {
		if(event.type == sf::Event::KeyPressed) {
			if(event.key.shift) {
				using k = sf::Keyboard::Key;
				switch(event.key.code) {
					case 56: { // < key
						return std::make_unique<TraverseStairsAction>(actor, true);
					}
					default: return std::make_unique<EmptyAction>(actor);
				}
			} else {
				using k = sf::Keyboard::Key;
				switch(event.key.code) {
					case k::Q:
						dir = Direction::NW;
						return std::make_unique<MoveAction>(actor, dir);
					case k::W:
						dir = Direction::N;
						return std::make_unique<MoveAction>(actor, dir);
					case k::E:
						dir = Direction::NE;
						return std::make_unique<MoveAction>(actor, dir);
					case k::A:
						dir = Direction::W;
						return std::make_unique<MoveAction>(actor, dir);
					case k::S:
						return std::make_unique<WaitAction>(actor);
					case k::D:
						dir = Direction::E;
						return std::make_unique<MoveAction>(actor, dir);
					case k::Z:
						dir = Direction::SW;
						return std::make_unique<MoveAction>(actor, dir);
					case k::X:
						dir = Direction::S;
						return std::make_unique<MoveAction>(actor, dir);
					case k::C:
						dir = Direction::SE;
						return std::make_unique<MoveAction>(actor, dir);
					case k::I: {
						std::unique_ptr<State> inventoryMenuState = std::make_unique<InventoryMenuState>(engine, actor);
						engine->pushState(std::move(inventoryMenuState));
						break;
					}
					// TODO select direction
					case k::O: {
						return std::make_unique<OpenAction>(actor);
					}
					case k::P: {
						findPath(actor->world,
										 Point(actor->x, actor->y),
										 io::mousePosition);
						break;
					}
					// TODO select direction
					case k::T: {
						return std::make_unique<TalkAction>(actor);
					}
					case k::Comma: {
						return std::make_unique<PickupAction>(actor);
					}
					case k::Period:
						return std::make_unique<LookAction>(actor);
					case 56: { // < key
						return std::make_unique<TraverseStairsAction>(actor, false);
					}
					case k::Escape: {
						engine->save();
						std::unique_ptr<State> mainMenuState = std::make_unique<MainMenuState>(engine);
						engine->pushState(std::move(mainMenuState));
					}
					default: return std::make_unique<EmptyAction>(actor);
				}
			}
		} else if(event.type == sf::Event::MouseButtonPressed) {
			if(event.mouseButton.button == sf::Mouse::Left) {
				return std::make_unique<ShootAction>(actor, io::mousePosition);
			}
			if(event.mouseButton.button == sf::Mouse::Right) {

			}
		}
	}
	return std::make_unique<EmptyAction>(actor);
}

// TODO this belongs elsewhere, but here now for development
void PlayerAi::findPath(World* world, Point from, Point to) {
	std::cout << "Finding path from (" << from.x << ", " << from.y
	<< ") to (" << to.x << ", " << to.y <<")...\n";

	Mat2d<Tile> tiles = world->map.tiles;
	std::queue<Point> frontier;
	frontier.push(from);

	std::unordered_map<Point, Point, PointHasher> came_from;
	came_from[from] = from;

	while(!frontier.empty()) {
		Point current = frontier.front();
		frontier.pop();

		if (current == to) {
			break;
		}
		//std::cout << "Visiting (" << current.x << ", " << current.y << ")\n";
		for(int i = -1; i <= 1; ++i) {
			for(int j = -1; j <= 1; ++j) {
				if(i == 0 && j == 0) continue;
				Point next = Point(current.x + i, current.y + j);
				if(next.x < 0 || next.y < 0 || next.x >= tiles.w || next.y >= tiles.h) continue;
				if(came_from.find(next) == came_from.end()) {
					//std::cout << "  Next: (" << next.x << ", " << next.y << ")\n";
					if(tiles.at(next.x, next.y).walkable) {
						// std::cout << "    Walkable!\n";
						frontier.push(next);
						came_from[next] = current;
					}
					// else std::cout << "    Not walkable!\n";
				}
			}
		}
	}

	std::cout << "Done!\n";
	for(auto& a : came_from) {
		std::cout << a.first.x << ", " << a.first.y << " <- "
			<< a.second.x << ", " << a.second.y << "\n";
	}

	Point current = to;
	std::vector<Point> path;
	while(!(current == from)) {
		path.push_back(current);
		current = came_from[current];
	}
	path.push_back(from);

	std::cout << "Path:\n";
	for(auto& a : path) {
		std::cout << a.x << ", " << a.y << "\n";
	}
}

std::unique_ptr<Action> MonsterAi::getNextAction(Actor* actor) {
	World* world = actor->world;
	Direction direction = Direction::NONE;
	Actor* player = world->getPlayer();
	/*if (los::is_visible(Point(actor->x, actor->y), Point(player->x, player->y), &world->map, world->getActorsAsPtrs(), constants::DEFAULT_ENEMY_FOV_RADIUS)) {
		actor->col = colors::red;
	} else  { actor->col = colors::black; }*/

	if (actor->destructible && actor->destructible->isDead()) return std::make_unique<WaitAction>(WaitAction(actor));

	if (aiState == AiState::FRIENDLY) {
		return std::make_unique<WaitAction>(WaitAction(actor));
	}

	if(actor->destructible->hp <= actor->destructible->maxHp * 0.3 + (0.1 * player->body->getModifier(player->body->charisma))) {
		aiState = AiState::FRIGHTENED; //TODO implement in terms of morale
	}

	if(actor->destructible->hp >= actor->destructible->maxHp * 0.6) {
		aiState = AiState::NORMAL;
	}

	if(aiState == AiState::NORMAL) {
		//if (actor->world->isInFov(actor->x, actor->y)) {
		if (los::is_visible(Point(actor->x, actor->y), Point(player->x, player->y), &world->map, world->getActorsAsPtrs(), constants::DEFAULT_ENEMY_FOV_RADIUS)) {
			moveCount = TRACKING_TURNS; //TODO also track if was in FOV, and fire event when first seeing player
		} else { --moveCount; }

		if (moveCount > 0) {
			int targetX = player->x;
			int targetY = player->y;
			int dx = targetX - actor->x;
			int dy = targetY - actor->y;
			int stepDx = (dx == 0 ? 0 : (dx > 0 ? 1 : -1));
			int stepDy = (dy == 0 ? 0 : (dy > 0 ? 1 : -1));
			float distance = sqrtf(dx*dx + dy*dy);

			if(distance >= 2) {
				dx = (int) (round(dx / distance));
				dy = (int) (round(dy / distance));
				if(world->canWalk(actor->x + stepDx, actor->y + stepDy)) { // uhh
					if (stepDx ==  0 && stepDy ==  0) return std::make_unique<WaitAction>(WaitAction(actor));
					if (stepDx ==  0 && stepDy == -1) direction = Direction::N;
					if (stepDx ==  1 && stepDy == -1) direction = Direction::NE;
					if (stepDx ==  1 && stepDy ==  0) direction = Direction::E;
					if (stepDx ==  1 && stepDy ==  1) direction = Direction::SE;
					if (stepDx ==  0 && stepDy ==  1) direction = Direction::S;
					if (stepDx == -1 && stepDy ==  1) direction = Direction::SW;
					if (stepDx == -1 && stepDy ==  0) direction = Direction::W;
					if (stepDx == -1 && stepDy == -1) direction = Direction::NW;
					return std::make_unique<MoveAction>(MoveAction(actor, direction));
				} else if (world->canWalk(actor->x + stepDx, actor->y)) { // Wall sliding
					if (stepDx ==  0 && stepDy ==  0) return std::make_unique<WaitAction>(WaitAction(actor));
					if (stepDx ==  1 && stepDy ==  0) direction = Direction::E;
					if (stepDx == -1 && stepDy ==  0) direction = Direction::W;
					return std::make_unique<MoveAction>(MoveAction(actor, direction));
				} else if (world->canWalk(actor->x, actor->y + stepDy)) {
					if (stepDx ==  0 && stepDy ==  0) return std::make_unique<WaitAction>(WaitAction(actor));
					if (stepDx ==  0 && stepDy == -1) direction = Direction::N;
					if (stepDx ==  0 && stepDy ==  1) direction = Direction::S;
					return std::make_unique<MoveAction>(MoveAction(actor, direction));
				}
			} else { // Melee range
				if (stepDx ==  0 && stepDy ==  0) return std::make_unique<WaitAction>(WaitAction(actor));

				if (stepDx ==  0 && stepDy == -1) direction = Direction::N;
				if (stepDx ==  1 && stepDy == -1) direction = Direction::NE;
				if (stepDx ==  1 && stepDy ==  0) direction = Direction::E;
				if (stepDx ==  1 && stepDy ==  1) direction = Direction::SE;
				if (stepDx ==  0 && stepDy ==  1) direction = Direction::S;
				if (stepDx == -1 && stepDy ==  1) direction = Direction::SW;
				if (stepDx == -1 && stepDy ==  0) direction = Direction::W;
				if (stepDx == -1 && stepDy == -1) direction = Direction::NW;
				return std::make_unique<MoveAction>(MoveAction(actor, direction));
			}
		}
		return std::make_unique<WaitAction>(WaitAction(actor));
	} else {
		Actor* player = world->getPlayer();
		int targetX = player->x;
		int targetY = player->y;
		int dx = targetX - actor->x;
		int dy = targetY - actor->y;
		int stepDx = (dx == 0 ? 0 : (dx > 0 ? -1 : 1));
		int stepDy = (dy == 0 ? 0 : (dy > 0 ? -1 : 1));
		if (stepDx ==  0 && stepDy == -1) direction = Direction::N;
		if (stepDx ==  1 && stepDy == -1) direction = Direction::NE;
		if (stepDx ==  1 && stepDy ==  0) direction = Direction::E;
		if (stepDx ==  1 && stepDy ==  1) direction = Direction::SE;
		if (stepDx ==  0 && stepDy ==  1) direction = Direction::S;
		if (stepDx == -1 && stepDy ==  1) direction = Direction::SW;
		if (stepDx == -1 && stepDy ==  0) direction = Direction::W;
		if (stepDx == -1 && stepDy == -1) direction = Direction::NW;
		return std::make_unique<MoveAction>(MoveAction(actor, direction));
	}
}

void TemporaryAi::decreaseTurns(Actor* owner) {
	--turns;
	if(turns <= 0) {
		owner->ai = std::move(oldAi);
	}
}

void TemporaryAi::applyTo(Actor* actor) {
	oldAi = std::move(actor->ai);
	this->faction = oldAi->faction;
	actor->ai.reset(this);
}

std::unique_ptr<Action> ConfusedMonsterAi::getNextAction(Actor* owner) {
	decreaseTurns(owner);

	int stepDx = d3() - 2; // -1, 0, 1
	int stepDy = d3() - 2;

	Direction direction;
	if (stepDx ==  0 && stepDy ==  0) return std::make_unique<WaitAction>(WaitAction(owner));

	if (stepDx ==  0 && stepDy == -1) direction = Direction::N;
	if (stepDx ==  1 && stepDy == -1) direction = Direction::NE;
	if (stepDx ==  1 && stepDy ==  0) direction = Direction::E;
	if (stepDx ==  1 && stepDy ==  1) direction = Direction::SE;
	if (stepDx ==  0 && stepDy ==  1) direction = Direction::S;
	if (stepDx == -1 && stepDy ==  1) direction = Direction::SW;
	if (stepDx == -1 && stepDy ==  0) direction = Direction::W;
	if (stepDx == -1 && stepDy == -1) direction = Direction::NW;
	return std::make_unique<MoveAction>(MoveAction(owner, direction));
}

BOOST_CLASS_EXPORT(PlayerAi)
BOOST_CLASS_EXPORT(MonsterAi)
BOOST_CLASS_EXPORT(TemporaryAi)
BOOST_CLASS_EXPORT(ConfusedMonsterAi)
