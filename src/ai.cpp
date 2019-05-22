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
#include "pathfinding.hpp"
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

static const int TRACKING_TURNS = 3;

const int LEVEL_UP_BASE = 50;
const int LEVEL_UP_FACTOR = 150;

void Ai::updateFov(Actor* actor) {
	float fovRadius = constants::DEFAULT_FOV_RADIUS * (actor->body ? actor->body->perception / 10.0 : 1.0);
	if(actor->destructible->isDead()) {
		fov = {};
		return;
	}
	if(isPlayer()) {
		fov = fov::computeFov(&(actor->world->map), actor->x, actor->y, fovRadius, FovType::CIRCLE, actor->world->getActorsAsPtrs());
	}
	else {
		fov = fov::computeEnemyFov(&(actor->world->map), actor->x, actor->y, currentDirection, fovRadius, FovType::CIRCLE, actor->world->getActorsAsPtrs());
	}
}

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
				// using k = sf::Keyboard::Key;
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
			actor->actionsQueue.clear(); // TODO this should work!
			if(event.mouseButton.button == sf::Mouse::Left) {
				std::unique_ptr<Action> lastAction = nullptr;
				auto actorsAtTarget = actor->world->getActorsAt(io::mousePosition.x, io::mousePosition.y);
				for(auto& target : actorsAtTarget) {
					// TODO other default actions
					if(target->destructible && !target->destructible->isDead()) {
						lastAction = std::make_unique<HitAction>(actor, io::mousePosition);
					}
					if(target->openable && !target->openable->open) {
						lastAction = std::make_unique<OpenAction>(actor, target);
					}
					else if(target->openable && target->openable->open) {
						lastAction = std::make_unique<OpenAction>(actor, target);
					}
					if(target->pickable) {
						lastAction = std::make_unique<PickupAction>(actor);
					}
				}

				// Find path
				std::vector<Point> path = pathfinding::findPath(actor->world->map,
								 Point(actor->x, actor->y),
								 io::mousePosition);
				if(!(path.size() <= 1)) {
					for(int i = 0; i < path.size() - 1; ++i) {
						Point from = path.at(i);
						Point to = path.at(i + 1);
						int stepDx = to.x - from.x;
						int stepDy = to.y - from.y;
						Direction stepDir = direction::getDirectionFromDeltas(stepDx, stepDy);
						if(i != path.size() - 2) {
							actions.push_back(std::make_unique<MoveAction>(MoveAction(actor, stepDir)));
						} else { // for the last step of the path...
							if(lastAction == nullptr) { // ... if no alternate default, push move
								actions.push_back(std::make_unique<MoveAction>(MoveAction(actor, stepDir)));
							} else if(lastAction->actionRange.index() == 0) { // ActionRange enum
								ActionRange actionRange = std::get<ActionRange>(lastAction->actionRange);
								switch(actionRange) {
									case ActionRange::ON_TOP:
										actions.push_back(std::make_unique<MoveAction>(MoveAction(actor, stepDir)));
										actions.push_back(std::move(lastAction));
										break;
									case ActionRange::NEXT_TO:
										actions.push_back(std::move(lastAction));
										break;
									case ActionRange::ANYWHERE:
										// TODO I suppose??
										throw std::logic_error("Make sure this does what you think it does");
										actions.push_back(std::move(lastAction));
										break;
									default:
										throw std::logic_error("Unknown action range");
								}

							} else if(lastAction->actionRange.index() == 1) { // float for range
								throw std::logic_error("Action ranges not implemented");
							}
						}

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

void MonsterAi::setCurrentTargetIndex(int index) {
	currentTargetIndex = index % patrolPoints.size();
	currentTarget = &patrolPoints.at(currentTargetIndex);
}

std::vector<std::unique_ptr<Action>> MonsterAi::getNextAction(Actor* actor) {
	std::vector<std::unique_ptr<Action>> actions;
	World* world = actor->world;
	Direction direction = Direction::NONE;
	Actor* player = world->getPlayer();

	if (actor->destructible && actor->destructible->isDead()) {
		actions.push_back(std::make_unique<WaitAction>(WaitAction(actor)));
		return actions;
	}

	// State machine: Check conditions for moving from one state to another
	switch(aiState) {
		case AiState::NORMAL_WANDER: { // TODO fix wander state
			if (isInFov(player->x, player->y)) {
				aiState = AiState::SEEN_PLAYER_HOSTILE;
				moveCount = TRACKING_TURNS;
				if(!hasSeenPlayer) {
					hasSeenPlayer = true;
					EnemyHasSeenPlayerEvent e(actor);
					world->notify(e);
				}
			}
			break;
		}
		case AiState::NORMAL_PATROL: {
			if (isInFov(player->x, player->y)) {
				aiState = AiState::SEEN_PLAYER_HOSTILE;
				moveCount = TRACKING_TURNS;
				if(!hasSeenPlayer) {
					hasSeenPlayer = true;
					EnemyHasSeenPlayerEvent e(actor);
					world->notify(e);
				}
			}
			break;
		}
		case AiState::NORMAL_IDLE: {
			break;
		}
		case AiState::CURIOUS: {
			break;
		}
		case AiState::SEEN_PLAYER_FRIENDLY: {
			break;
		}
		case AiState::SEEN_PLAYER_HOSTILE: {
			if(moveCount <= 0) {
				// TODO go to previous state instead
				aiState = AiState::NORMAL_PATROL;
			}
			break;
		}
		default:
		throw "AI state not implemented";
		break;
	}

	// Select actions
	switch(aiState) {
		case AiState::NORMAL_WANDER:
			actions.push_back(std::make_unique<WaitAction>(WaitAction(actor)));
			return actions;
		break;
		case AiState::NORMAL_PATROL:
		{
			if(patrolPoints.empty()) {
				actions.push_back(std::make_unique<WaitAction>(WaitAction(actor)));
				return actions;
			}
			if(currentTarget == nullptr) {
				currentTargetIndex = 0;
				currentTarget = &patrolPoints.at(0);
			}
			std::unique_ptr<Action> lastAction = nullptr;
			auto actorsAtTarget = actor->world->getActorsAt(currentTarget->x, currentTarget->y);
			for(auto& target : actorsAtTarget) {
				// TODO other default actions
				if(target->openable && !target->openable->open) {
					lastAction = std::make_unique<OpenAction>(actor, target);
				}
				else if(target->openable && target->openable->open) {
					lastAction = std::make_unique<OpenAction>(actor, target);
				}
				if(target->pickable) {
					lastAction = std::make_unique<PickupAction>(actor);
				}
			}

			// Find path
			// TODO:
			// Add internal action queue,
			// pop first action from there
			// and only find path if queue is empty
			if(plannedActions.empty()) {
				std::vector<Point> path = pathfinding::findPath(actor->world->map,
								 Point(actor->x, actor->y),
								 *currentTarget);
				if(!(path.size() <= 1)) {
					for(int i = 0; i < path.size() - 1; ++i) {
						Point from = path.at(i);
						Point to = path.at(i + 1);
						int stepDx = to.x - from.x;
						int stepDy = to.y - from.y;
						Direction stepDir = direction::getDirectionFromDeltas(stepDx, stepDy);
						if(i != path.size() - 2) {
							plannedActions.push_back(std::make_unique<MoveAction>(MoveAction(actor, stepDir)));
						} else { // for the last step of the path...
							if(lastAction == nullptr) { // ... if no alternate default, push move
								plannedActions.push_back(std::make_unique<MoveAction>(MoveAction(actor, stepDir)));
							} else if(lastAction->actionRange.index() == 0) { // ActionRange enum
								ActionRange actionRange = std::get<ActionRange>(lastAction->actionRange);
								switch(actionRange) {
									case ActionRange::ON_TOP:
										plannedActions.push_back(std::make_unique<MoveAction>(MoveAction(actor, stepDir)));
										plannedActions.push_back(std::move(lastAction));
										break;
									case ActionRange::NEXT_TO:
										plannedActions.push_back(std::move(lastAction));
										break;
									case ActionRange::ANYWHERE:
										// TODO I suppose??
										throw std::logic_error("Make sure this does what you think it does");
										plannedActions.push_back(std::move(lastAction));
										break;
									default:
										throw std::logic_error("Unknown action range");
								}

							} else if(lastAction->actionRange.index() == 1) { // float for range
								throw std::logic_error("Action ranges not implemented");
							}
						}

					}
				} else {
					++currentTargetIndex;
					if(currentTargetIndex >= patrolPoints.size()) currentTargetIndex = 0;
					currentTarget = &patrolPoints.at(currentTargetIndex);
					actor->actionsQueue.clear();
					plannedActions.push_back(std::make_unique<WaitAction>(WaitAction(actor)));
					std::move(plannedActions.begin(), plannedActions.begin() + 1, back_inserter(actions));
					plannedActions.erase(plannedActions.begin(), plannedActions.begin() + 1);
					return actions;
				}
				plannedActions.push_back(std::make_unique<WaitAction>(WaitAction(actor)));
				std::move(plannedActions.begin(), plannedActions.begin() + 1, back_inserter(actions));
				plannedActions.erase(plannedActions.begin(), plannedActions.begin() + 1);
				return actions;
			} else { // plannedActions NOT empty
				std::move(plannedActions.begin(), plannedActions.begin() + 1, back_inserter(actions));
				plannedActions.erase(plannedActions.begin(), plannedActions.begin() + 1);
				return actions;
			}
		break;
		}
		case AiState::NORMAL_IDLE:
			actions.push_back(std::make_unique<WaitAction>(WaitAction(actor)));
			return actions;
		break;
		case AiState::CURIOUS:
			actions.push_back(std::make_unique<WaitAction>(WaitAction(actor)));
			return actions;
		break;
		case AiState::SEEN_PLAYER_FRIENDLY:
			actions.push_back(std::make_unique<WaitAction>(WaitAction(actor)));
			return actions;
		break;
		case AiState::SEEN_PLAYER_HOSTILE:
		{
			if (isInFov(player->x, player->y)) {
				moveCount = TRACKING_TURNS; //TODO also track if was in FOV, and fire event when first seeing player
				if(!hasSeenPlayer) {
					hasSeenPlayer = true;
					EnemyHasSeenPlayerEvent e(actor);
					world->notify(e);
				}
			} else {
				hasSeenPlayer = false;
				--moveCount;
			}

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
						direction = direction::getDirectionFromDeltas(stepDx, stepDy);
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

					direction = direction::getDirectionFromDeltas(stepDx, stepDy);
					actions.push_back(std::make_unique<HitAction>(HitAction(actor, Point{actor->x + stepDx, actor->y + stepDy})));
					return actions;
				}
			}
			actions.push_back(std::make_unique<WaitAction>(WaitAction(actor)));
			return actions;
		break;
		}
		default:
		throw "AI state not implemented";
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

	direction = direction::getDirectionFromDeltas(stepDx, stepDy);
	actions.push_back(std::make_unique<MoveAction>(MoveAction(owner, direction)));
	return actions;
}

BOOST_CLASS_EXPORT(PlayerAi)
BOOST_CLASS_EXPORT(MonsterAi)
BOOST_CLASS_EXPORT(TemporaryAi)
BOOST_CLASS_EXPORT(ConfusedMonsterAi)
