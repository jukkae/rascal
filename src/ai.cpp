#include "ai.hpp"
#include "action.hpp"
#include "actor.hpp"
#include "attacker.hpp"
#include "colors.hpp"
#include "constants.hpp"
#include "container.hpp"
#include "destructible.hpp"
#include "dice.hpp"
#include "engine.hpp"
#include "gameplay_state.hpp"
#include "gui.hpp"
#include "io.hpp"
#include "inventory_menu_state.hpp"
#include "map.hpp"
#include "pickable.hpp"
#include "persistent.hpp"
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

int PlayerAi::getNextLevelXp() const {
	return LEVEL_UP_BASE + xpLevel * LEVEL_UP_FACTOR;
}

void PlayerAi::increaseXp(Actor* owner, int xp) {
	experience += xp;
	if(experience >= getNextLevelXp()) {
		experience -= getNextLevelXp();
		++xpLevel;
		owner->s->message(colors::yellow, "You've reached level %d!", xpLevel);
		Engine* engine = owner->s->getEngine();
		std::unique_ptr<State> levelUpMenuState = std::make_unique<LevelUpMenuState>(engine, owner);
		engine->pushState(std::move(levelUpMenuState));
	}
}

std::unique_ptr<Action> PlayerAi::getNextAction(Actor* actor) {
	Direction dir = Direction::NONE;

	Engine* engine = actor->s->getEngine();
	sf::Event event;
	while(engine->pollEvent(event)) {
		if(event.type == sf::Event::KeyPressed) {
			using k = sf::Keyboard::Key;
			switch(event.key.code) {
				case k::Up:
					dir = Direction::N;
					break;
				case k::Down:
					dir = Direction::S;
					break;
				case k::Left:
					dir = Direction::W;
					break;
				case k::Right:
					dir = Direction::E;
					break;
				case k::I: {
					std::unique_ptr<State> inventoryMenuState = std::make_unique<InventoryMenuState>(engine, actor);
					engine->pushState(std::move(inventoryMenuState));
					break;
				}
				case k::Comma: {
					return std::make_unique<PickupAction>(PickupAction(actor));
				}
				case k::Period: {
					if(event.key.shift) {
						return std::make_unique<LookAction>(LookAction(actor));
					}
					else {
						return std::make_unique<WaitAction>(WaitAction(actor));
					}
				}
				case k::S: {
					return std::make_unique<ShootAction>(ShootAction(actor, io::mousePosition));
				}
				case 56: { // < key
					if(event.key.shift) {
						return std::make_unique<TraverseStairsAction>(TraverseStairsAction(actor, true));
					}
					else {
						return std::make_unique<TraverseStairsAction>(TraverseStairsAction(actor, false));
					}
				}
				case k::Escape: {
					engine->save();
					std::unique_ptr<State> mainMenuState = std::make_unique<MainMenuState>(engine);
					engine->pushState(std::move(mainMenuState));
				}
				default: dir = Direction::NONE; break;
			}
		}
	}

	return std::make_unique<MoveAction>(MoveAction(actor, dir));
}

std::unique_ptr<Action> MonsterAi::getNextAction(Actor* actor) {
	GameplayState* state = actor->s;
	World* world = actor->world;
	Direction direction = Direction::NONE;
	if (actor->destructible && actor->destructible->isDead()) return std::make_unique<WaitAction>(WaitAction(actor));

	if (actor->world->isInFov(actor->x, actor->y)) {
		moveCount = TRACKING_TURNS; //TODO also track if was in FOV, and fire event when first seeing player
	} else { --moveCount; }

	if (moveCount > 0) {
		Actor* player = world->getPlayer();
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
	actor->ai = std::unique_ptr<Ai>(this);
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
