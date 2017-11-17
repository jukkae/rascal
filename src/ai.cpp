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
#include "inventory_menu_state.hpp"
#include "map.hpp"
#include "pickable.hpp"
#include "persistent.hpp"
#include "state.hpp"
#include "level_up_menu_state.hpp"
#include "main_menu_state.hpp"
#include <math.h>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/vector.hpp>
#include "boost/optional.hpp"
#include <SFML/Window.hpp>

static const int TRACKING_TURNS = 3;

const int LEVEL_UP_BASE = 50;
const int LEVEL_UP_FACTOR = 150;

int PlayerAi::getNextLevelXp() const {
	return LEVEL_UP_BASE + xpLevel * LEVEL_UP_FACTOR;
}

void PlayerAi::increaseXp(int xp, GameplayState* state) {
	experience += xp;
	if(experience >= getNextLevelXp()) {
		experience -= getNextLevelXp();
		++xpLevel;
		state->message(colors::yellow, "You've reached level %d!", xpLevel);
		Engine* engine = state->getEngine();
		Actor* player = state->getPlayer();
		State* levelUpMenuState = new LevelUpMenuState(player);
		levelUpMenuState->init(engine);
		engine->pushState(levelUpMenuState);
	}
}

Action* PlayerAi::getNextAction(Actor* actor) {
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
				case k::G: {
					return new PickupAction(actor);
				}
				case k::I: { // TODO is this really the right place for this?
					State* inventoryMenuState = new InventoryMenuState(actor);
					inventoryMenuState->init(engine);
					engine->pushState(inventoryMenuState);
				}
				case 56: { // < key
					if(event.key.shift) {
						return new TraverseStairsAction(actor, true);
					}
					else {
						return new TraverseStairsAction(actor, false);
					}
				}
				case k::Escape: { // TODO this would need to be captured earlier
					engine->save();
					State* mainMenuState = new MainMenuState();
					mainMenuState->init(engine);
					engine->pushState(mainMenuState);
				}
				default: dir = Direction::NONE; break;
			}
		}
	}

	return new MoveAction(actor, dir);
}

Action* MonsterAi::getNextAction(Actor* actor) {
	GameplayState* state = actor->s;
	Direction direction = Direction::NONE;
	if (actor->destructible && actor->destructible->isDead()) return new WaitAction(actor);

	if (actor->s->isInFov(actor->x, actor->y)) {
		moveCount = TRACKING_TURNS;
	} else { --moveCount; }

	if (moveCount > 0) {
		Actor* player = state->getPlayer();
		int targetX = player->x;
		int targetY = player->y;
		int dx = targetX - actor->x;
		int dy = targetY - actor->y;
		int stepDx = (dx == 0 ? 0 : (dx > 0 ? 1 : -1));
		int stepDy = (dy == 0 ? 0 : (dy > 0 ? 1 : -1));
		float distance = sqrtf(dx*dx + dy*dy);

		if(distance >= 2) {
			state->message(colors::white, "The %s threatens you!", actor->name.c_str());
			dx = (int) (round(dx / distance));
			dy = (int) (round(dy / distance));
			if(state->canWalk(actor->x + stepDx, actor->y + stepDy)) { // uhh
				if (stepDx ==  0 && stepDy ==  0) return new WaitAction(actor);

				if (stepDx ==  0 && stepDy == -1) direction = Direction::N;
				if (stepDx ==  1 && stepDy == -1) direction = Direction::NE;
				if (stepDx ==  1 && stepDy ==  0) direction = Direction::E;
				if (stepDx ==  1 && stepDy ==  1) direction = Direction::SE;
				if (stepDx ==  0 && stepDy ==  1) direction = Direction::S;
				if (stepDx == -1 && stepDy ==  1) direction = Direction::SW;
				if (stepDx == -1 && stepDy ==  0) direction = Direction::W;
				if (stepDx == -1 && stepDy == -1) direction = Direction::NW;
				return new MoveAction(actor, direction);
			} else if (state->canWalk(actor->x + stepDx, actor->y)) { // Wall sliding
				if (stepDx ==  0 && stepDy ==  0) return new WaitAction(actor);
				if (stepDx ==  1 && stepDy ==  0) direction = Direction::E;
				if (stepDx == -1 && stepDy ==  0) direction = Direction::W;
				return new MoveAction(actor, direction);
			} else if (state->canWalk(actor->x, actor->y + stepDy)) {
				if (stepDx ==  0 && stepDy ==  0) return new WaitAction(actor);
				if (stepDx ==  0 && stepDy == -1) direction = Direction::N;
				if (stepDx ==  0 && stepDy ==  1) direction = Direction::S;
				return new MoveAction(actor, direction);
			}
		} else { // Melee range
			if (stepDx ==  0 && stepDy ==  0) return new WaitAction(actor);

			if (stepDx ==  0 && stepDy == -1) direction = Direction::N;
			if (stepDx ==  1 && stepDy == -1) direction = Direction::NE;
			if (stepDx ==  1 && stepDy ==  0) direction = Direction::E;
			if (stepDx ==  1 && stepDy ==  1) direction = Direction::SE;
			if (stepDx ==  0 && stepDy ==  1) direction = Direction::S;
			if (stepDx == -1 && stepDy ==  1) direction = Direction::SW;
			if (stepDx == -1 && stepDy ==  0) direction = Direction::W;
			if (stepDx == -1 && stepDy == -1) direction = Direction::NW;
			return new MoveAction(actor, direction);
		}
	}
	return new WaitAction(actor);
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

Action* ConfusedMonsterAi::getNextAction(Actor* owner) {
	decreaseTurns(owner);

	int stepDx = d3() - 2; // -1, 0, 1
	int stepDy = d3() - 2;

	Direction direction;
	if (stepDx ==  0 && stepDy ==  0) return new WaitAction(owner);

	if (stepDx ==  0 && stepDy == -1) direction = Direction::N;
	if (stepDx ==  1 && stepDy == -1) direction = Direction::NE;
	if (stepDx ==  1 && stepDy ==  0) direction = Direction::E;
	if (stepDx ==  1 && stepDy ==  1) direction = Direction::SE;
	if (stepDx ==  0 && stepDy ==  1) direction = Direction::S;
	if (stepDx == -1 && stepDy ==  1) direction = Direction::SW;
	if (stepDx == -1 && stepDy ==  0) direction = Direction::W;
	if (stepDx == -1 && stepDy == -1) direction = Direction::NW;
	return new MoveAction(owner, direction);
}

BOOST_CLASS_EXPORT(PlayerAi)
BOOST_CLASS_EXPORT(MonsterAi)
BOOST_CLASS_EXPORT(TemporaryAi)
BOOST_CLASS_EXPORT(ConfusedMonsterAi)
