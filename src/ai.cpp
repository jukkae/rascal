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

std::vector<std::unique_ptr<Action>> PlayerAi::getNextAction(Actor* actor) {
	std::vector<std::unique_ptr<Action>> actions;
	Direction dir = Direction::NONE;

	Engine* engine = io::engine;
	sf::Event event;
	while(engine->pollEvent(event)) {
		if(event.type == sf::Event::KeyPressed) {
			if(event.key.shift) {
				using k = sf::Keyboard::Key;
				switch(event.key.code) {
					case 56: { // < key
						actions.push_back(std::make_unique<TraverseStairsAction>(actor, true));
						break;
					}
					default: {
						actions.push_back(std::make_unique<EmptyAction>(actor));
						break;
					}
				}
			} else {
				using k = sf::Keyboard::Key;
				switch(event.key.code) {
					case k::Q:
						dir = Direction::NW;
						actions.push_back(std::make_unique<MoveAction>(actor, dir));
						break;
					case k::W:
						dir = Direction::N;
						actions.push_back(std::make_unique<MoveAction>(actor, dir));
						break;
					case k::E:
						dir = Direction::NE;
						actions.push_back(std::make_unique<MoveAction>(actor, dir));
						break;
					case k::A:
						dir = Direction::W;
						actions.push_back(std::make_unique<MoveAction>(actor, dir));
						break;
					case k::S:
						actions.push_back(std::make_unique<WaitAction>(actor));
						break;
					case k::D:
						dir = Direction::E;
						actions.push_back(std::make_unique<MoveAction>(actor, dir));
						break;
					case k::Z:
						dir = Direction::SW;
						actions.push_back(std::make_unique<MoveAction>(actor, dir));
						break;
					case k::X:
						dir = Direction::S;
						actions.push_back(std::make_unique<MoveAction>(actor, dir));
						break;
					case k::C:
						dir = Direction::SE;
						actions.push_back(std::make_unique<MoveAction>(actor, dir));
						break;
					case k::I: {
						std::unique_ptr<State> inventoryMenuState = std::make_unique<InventoryMenuState>(engine, actor);
						engine->pushState(std::move(inventoryMenuState));
						break;
					}
					// TODO select direction
					case k::O: {
						actions.push_back(std::make_unique<OpenAction>(actor));
						break;
					}
					// TODO select direction
					case k::T: {
						actions.push_back(std::make_unique<TalkAction>(actor));
						break;
					}
					case k::Comma: {
						actions.push_back(std::make_unique<PickupAction>(actor));
						break;
					}
					case k::Period:
						actions.push_back(std::make_unique<LookAction>(actor));
						break;
					case 56: { // < key
						actions.push_back(std::make_unique<TraverseStairsAction>(actor, false));
						break;
					}
					case k::Escape: {
						engine->save();
						std::unique_ptr<State> mainMenuState = std::make_unique<MainMenuState>(engine);
						engine->pushState(std::move(mainMenuState));
					}
					default: {
						actions.push_back(std::make_unique<EmptyAction>(actor));
						break;
					}
				}
			}
		} else if(event.type == sf::Event::MouseButtonPressed) {
			if(event.mouseButton.button == sf::Mouse::Left) {
				std::vector<Point> path = findPath(actor->world,
								 Point(actor->x, actor->y),
								 io::mousePosition);
				if(!(path.size() <= 1)) {
					for(int i = 0; i < path.size() - 1; ++i) {
						Point from = path.at(i);
						Point to = path.at(i + 1);
						int stepDx = to.x - from.x;
						int stepDy = to.y - from.y;
						Direction stepDir;
						if (stepDx ==  0 && stepDy == -1) stepDir = Direction::N;
						if (stepDx ==  1 && stepDy == -1) stepDir = Direction::NE;
						if (stepDx ==  1 && stepDy ==  0) stepDir = Direction::E;
						if (stepDx ==  1 && stepDy ==  1) stepDir = Direction::SE;
						if (stepDx ==  0 && stepDy ==  1) stepDir = Direction::S;
						if (stepDx == -1 && stepDy ==  1) stepDir = Direction::SW;
						if (stepDx == -1 && stepDy ==  0) stepDir = Direction::W;
						if (stepDx == -1 && stepDy == -1) stepDir = Direction::NW;
						actions.push_back(std::make_unique<MoveAction>(MoveAction(actor, stepDir)));
					}
				}
			}
			if(event.mouseButton.button == sf::Mouse::Right) {
				actions.push_back(std::make_unique<ShootAction>(actor, io::mousePosition));
			}
		}
	}
	if(actions.size() == 0) actions.push_back(std::make_unique<EmptyAction>(actor));
	return actions;
}

// TODO this belongs elsewhere, but here now for development
std::vector<Point> PlayerAi::findPath(World* world, Point from, Point to) {

	Mat2d<Tile> tiles = world->map.tiles;
	std::vector<Point> path;
	if(to.x < 0 || to.x >= tiles.w || to.y < 0 || to.y >= tiles.h ||
		 !tiles.at(to.x, to.y).walkable) return path; // TODO figure out more robust handling

	PriorityQueue<Point, float> frontier;
	frontier.put(from, 0.0f);

	std::unordered_map<Point, Point, PointHasher> came_from;
	came_from[from] = from;

	std::unordered_map<Point, float, PointHasher> cost_thus_far;
	cost_thus_far[from] = 0.0f;

	while(!frontier.empty()) {
		Point current = frontier.get();

		if (current == to) {
			break;
		}
		//std::cout << "Visiting (" << current.x << ", " << current.y << ")\n";
		for(int i = -1; i <= 1; ++i) {
			for(int j = -1; j <= 1; ++j) {
				if(i == 0 && j == 0) continue;
				Point next = Point(current.x + i, current.y + j);
				if(next.x < 0 || next.y < 0 || next.x >= tiles.w || next.y >= tiles.h) continue;
				float nextDeltaCost = (tiles.at(current.x, current.y).movementCost +
															 tiles.at(next.x, next.y).movementCost) / 2.0f;
				if((i != 0) && (j != 0)) nextDeltaCost *= sqrtf(2.0);
				float nextCost = cost_thus_far[current] + nextDeltaCost;
				if(cost_thus_far.find(next) == cost_thus_far.end() ||
					 nextCost < cost_thus_far[next]) {
			 		if(tiles.at(next.x, next.y).walkable) {
						cost_thus_far[next] = nextCost;
						float heuristic = sqrtf(pow(current.x - next.x, 2) +
						                        pow(current.y - next.y, 2));
						float priority = nextCost + heuristic;
						came_from[next] = current;
						frontier.put(next, priority);
					}
				}
			}
		}
	}

	Point current = to;

	while(!(current == from)) {
		path.push_back(current);
		current = came_from[current];
	}
	path.push_back(from);
	std::reverse(path.begin(), path.end());

	return path;
}

std::vector<std::unique_ptr<Action>> MonsterAi::getNextAction(Actor* actor) {
	std::vector<std::unique_ptr<Action>> actions;
	World* world = actor->world;
	Direction direction = Direction::NONE;
	Actor* player = world->getPlayer();
	/*if (los::is_visible(Point(actor->x, actor->y), Point(player->x, player->y), &world->map, world->getActorsAsPtrs(), constants::DEFAULT_ENEMY_FOV_RADIUS)) {
		actor->col = colors::red;
	} else  { actor->col = colors::black; }*/

	if (actor->destructible && actor->destructible->isDead()) {
		actions.push_back(std::make_unique<WaitAction>(WaitAction(actor)));
		return actions;
	}

	if (aiState == AiState::FRIENDLY) {
		actions.push_back(std::make_unique<WaitAction>(WaitAction(actor)));
		return actions;
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
					if (stepDx ==  0 && stepDy ==  0) {
						actions.push_back(std::make_unique<WaitAction>(WaitAction(actor)));
						return actions;
					}
					if (stepDx ==  0 && stepDy == -1) direction = Direction::N;
					if (stepDx ==  1 && stepDy == -1) direction = Direction::NE;
					if (stepDx ==  1 && stepDy ==  0) direction = Direction::E;
					if (stepDx ==  1 && stepDy ==  1) direction = Direction::SE;
					if (stepDx ==  0 && stepDy ==  1) direction = Direction::S;
					if (stepDx == -1 && stepDy ==  1) direction = Direction::SW;
					if (stepDx == -1 && stepDy ==  0) direction = Direction::W;
					if (stepDx == -1 && stepDy == -1) direction = Direction::NW;
					actions.push_back(std::make_unique<MoveAction>(MoveAction(actor, direction)));
					return actions;
				} else if (world->canWalk(actor->x + stepDx, actor->y)) { // Wall sliding
					if (stepDx ==  0 && stepDy ==  0) {
						actions.push_back(std::make_unique<WaitAction>(WaitAction(actor)));
						return actions;
					}
					if (stepDx ==  1 && stepDy ==  0) direction = Direction::E;
					if (stepDx == -1 && stepDy ==  0) direction = Direction::W;
					actions.push_back(std::make_unique<MoveAction>(MoveAction(actor, direction)));
					return actions;
				} else if (world->canWalk(actor->x, actor->y + stepDy)) {
					if (stepDx ==  0 && stepDy ==  0) {
						actions.push_back(std::make_unique<WaitAction>(WaitAction(actor)));
						return actions;
					}
					if (stepDx ==  0 && stepDy == -1) direction = Direction::N;
					if (stepDx ==  0 && stepDy ==  1) direction = Direction::S;
					actions.push_back(std::make_unique<MoveAction>(MoveAction(actor, direction)));
					return actions;
				}
			} else { // Melee range
				if (stepDx ==  0 && stepDy ==  0) {
					actions.push_back(std::make_unique<WaitAction>(WaitAction(actor)));
					return actions;
				}

				if (stepDx ==  0 && stepDy == -1) direction = Direction::N;
				if (stepDx ==  1 && stepDy == -1) direction = Direction::NE;
				if (stepDx ==  1 && stepDy ==  0) direction = Direction::E;
				if (stepDx ==  1 && stepDy ==  1) direction = Direction::SE;
				if (stepDx ==  0 && stepDy ==  1) direction = Direction::S;
				if (stepDx == -1 && stepDy ==  1) direction = Direction::SW;
				if (stepDx == -1 && stepDy ==  0) direction = Direction::W;
				if (stepDx == -1 && stepDy == -1) direction = Direction::NW;
				actions.push_back(std::make_unique<MoveAction>(MoveAction(actor, direction)));
				return actions;
			}
		}
		actions.push_back(std::make_unique<WaitAction>(WaitAction(actor)));
		return actions;
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
		actions.push_back(std::make_unique<MoveAction>(MoveAction(actor, direction)));
		return actions;
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

std::vector<std::unique_ptr<Action>> ConfusedMonsterAi::getNextAction(Actor* owner) {
	decreaseTurns(owner);
	std::vector<std::unique_ptr<Action>> actions;

	int stepDx = d3() - 2; // -1, 0, 1
	int stepDy = d3() - 2;

	Direction direction;
	if (stepDx ==  0 && stepDy ==  0) {
		actions.push_back(std::make_unique<WaitAction>(WaitAction(owner)));
		return actions;
	}

	if (stepDx ==  0 && stepDy == -1) direction = Direction::N;
	if (stepDx ==  1 && stepDy == -1) direction = Direction::NE;
	if (stepDx ==  1 && stepDy ==  0) direction = Direction::E;
	if (stepDx ==  1 && stepDy ==  1) direction = Direction::SE;
	if (stepDx ==  0 && stepDy ==  1) direction = Direction::S;
	if (stepDx == -1 && stepDy ==  1) direction = Direction::SW;
	if (stepDx == -1 && stepDy ==  0) direction = Direction::W;
	if (stepDx == -1 && stepDy == -1) direction = Direction::NW;
	actions.push_back(std::make_unique<MoveAction>(MoveAction(owner, direction)));
	return actions;
}

BOOST_CLASS_EXPORT(PlayerAi)
BOOST_CLASS_EXPORT(MonsterAi)
BOOST_CLASS_EXPORT(TemporaryAi)
BOOST_CLASS_EXPORT(ConfusedMonsterAi)
