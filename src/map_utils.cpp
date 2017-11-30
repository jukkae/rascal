#include "colors.hpp"
#include "dice.hpp"
#include "map_utils.hpp"
#include "map.hpp"
#include "gameplay_state.hpp"
#include "status_effect.hpp" // TODO for testing
#include <SFML/Graphics.hpp>

void map_utils::addItems(GameplayState* gameplayState, Map* map) {
	for(int x = 0; x < map->width; ++x) {
		for(int y = 0; y < map->height; ++y) {
			int r = d100();
			if (map->canWalk(x, y) && r <= 2) {
				map_utils::addItem(gameplayState, map, x, y);
			}
		}
	}
}

void map_utils::addMonsters(GameplayState* gameplayState, Map* map) {
	for(int x = 0; x < map->width; ++x) {
		for(int y = 0; y < map->height; ++y) {
			int r = d100();
			if (map->canWalk(x, y) && r == 1) {
				map_utils::addMonster(gameplayState, map, x, y);
			}
		}
	}
}

void map_utils::addPlayer(GameplayState* gameplayState, Map* map) {
	int x = 0;
	int y = 0;
	do {
		int r = d100();
		int s = d100();
		x = (map->width-1) * r / 100;
		y = (map->height-1) * s / 100;
	} while (map->isWall(x, y)); // should check for canWalk, but can't do that yet

	std::unique_ptr<Actor> player = std::make_unique<Actor>(x, y, '@', "you", sf::Color::White, 2);
	player->destructible = std::make_unique<PlayerDestructible>(100, 2, 0, "your corpse", 11);
	player->attacker     = std::make_unique<Attacker>(1, 2, 1);
	player->ai           = std::make_unique<PlayerAi>();
	player->container    = std::make_unique<Container>(100);
	player->addStatusEffect(std::make_unique<PoisonedStatusEffect>(10000));
	gameplayState->addActor(std::move(player));
}

void map_utils::addStairs(GameplayState* gameplayState, Map* map) {
	int x = 0;
	int y = 0;
	do {
		int r = d100();
		int s = d100();
		x = (map->width-1) * r / 100;
		y = (map->height-1) * s / 100;
	} while (map->isWall(x, y)); // should check for canWalk, but can't do that yet

	std::unique_ptr<Actor> stairs = std::make_unique<Actor>(x, y, '<', "stairs (up)", sf::Color::White, 0, true);
    stairs->blocks = false;
    stairs->fovOnly = false;
	gameplayState->addActor(std::move(stairs));
}

void map_utils::addMcGuffin(GameplayState* gameplayState, Map* map, int level) {
	int x = 0;
	int y = 0;
	do {
		int r = d100();
		int s = d100();
		x = (map->width-1) * r / 100;
		y = (map->height-1) * s / 100;
	} while (map->isWall(x, y)); // should check for canWalk, but can't do that yet

	std::unique_ptr<Actor> mcGuffin = std::make_unique<Actor>(x, y, 'q', "phlebotinum link", sf::Color::White, 0, true);
    mcGuffin->blocks = false;
	mcGuffin->pickable = std::make_unique<Pickable>(TargetSelector(TargetSelector::SelectorType::NONE));
	gameplayState->addActor(std::move(mcGuffin));
}

std::unique_ptr<Actor> map_utils::makeMonster(GameplayState* gameplayState, Map* map, int x, int y) {
	int r = d100();
	if(r < 60) {
		std::unique_ptr<Actor> punk = std::make_unique<Actor>(x, y, 'h', "punk", colors::desaturatedGreen, 1);
		punk->destructible = std::make_unique<MonsterDestructible>(10, 0, 50, "dead punk", 10);
		punk->attacker = std::make_unique<Attacker>(1, 3, 0);
		punk->ai = std::make_unique<MonsterAi>();
		return punk;
	} else if (r < 70) {
		std::unique_ptr<Actor> fighter = std::make_unique<Actor>(x, y, 'H', "fighter", colors::darkGreen, 1);
		fighter->destructible = std::make_unique<MonsterDestructible>(16, 1, 100, "fighter carcass", 12);
		fighter->attacker = std::make_unique<Attacker>(1, 6, 0);
		fighter->ai = std::make_unique<MonsterAi>();
		return fighter;
	} else if (r < 80) {
		std::unique_ptr<Actor> guard = std::make_unique<Actor>(x, y, 'h', "guard", colors::darkGreen, 1);
		guard->destructible = std::make_unique<MonsterDestructible>(6, 1, 100, "guard body", 17);
		guard->attacker = std::make_unique<Attacker>(1, 2, 0);
		guard->ai = std::make_unique<MonsterAi>(200);
		return guard;
	}
	else {
		std::unique_ptr<Actor> boxer = std::make_unique<Actor>(x, y, 'H', "boxer", colors::darkerGreen, 1);
		boxer->destructible = std::make_unique<MonsterDestructible>(4, 0, 70, "boxer carcass", 15);
		boxer->attacker = std::make_unique<Attacker>(1, 6, 2);
		boxer->ai = std::make_unique<MonsterAi>();
		return boxer;
	}
}

void map_utils::addMonster(GameplayState* gameplayState, Map* map, int x, int y) {
	gameplayState->addActor(map_utils::makeMonster(gameplayState, map, x, y));
}

std::unique_ptr<Actor> map_utils::makeItem(GameplayState* gameplayState, Map* map, int x, int y) {
	int r = d100();
	if(r < 40) {
		std::unique_ptr<Actor> stimpak = std::make_unique<Actor>(x, y, '!', "stimpak", sf::Color(128, 0, 128));
		stimpak->blocks = false;
		stimpak->pickable = std::make_unique<Pickable>(TargetSelector(TargetSelector::SelectorType::WEARER, 0), std::make_unique<HealthEffect>(4, "Damn, that does feel good!"));
		return stimpak;
	} else if(r < 50) {
		std::unique_ptr<Actor> fakeStimpak = std::make_unique<Actor>(x, y, '!', "stimpak", sf::Color(128, 128, 128));
		fakeStimpak->blocks = false;
		fakeStimpak->pickable = std::make_unique<Pickable>(TargetSelector(TargetSelector::SelectorType::WEARER, 0), std::make_unique<StatusEffectEffect>(std::make_unique<PoisonedStatusEffect>(PoisonedStatusEffect()), "Damn, that does NOT feel good!"));
		return fakeStimpak;
	} else if(r < 60) {
		std::unique_ptr<Actor> blasterBoltDevice = std::make_unique<Actor>(x, y, '?', "blaster bolt device", sf::Color(128, 128, 0));
		blasterBoltDevice->blocks = false;
		blasterBoltDevice->pickable = std::make_unique<Pickable>(TargetSelector(TargetSelector::SelectorType::CLOSEST_MONSTER, 5), std::make_unique<HealthEffect>(-20, "The %s is hit by a blast!\n The damage is %g hit points."));
		return blasterBoltDevice;
	} else if(r < 70) {
		std::unique_ptr<Actor> fragGrenade = std::make_unique<Actor>(x, y, '?', "fragmentation grenade", sf::Color(128, 255, 128));
		fragGrenade->blocks = false;
		fragGrenade->pickable = std::make_unique<Pickable>(TargetSelector(TargetSelector::SelectorType::SELECTED_RANGE, 3), std::make_unique<HealthEffect>(-12, "The grenade explodes, hurting the %s for %g hit points!"));
		return fragGrenade;
	} else if(r < 80) {
		std::unique_ptr<Actor> confusor = std::make_unique<Actor>(x, y, '?', "confusor", sf::Color(128, 128, 255));
		confusor->blocks = false;
		confusor->pickable = std::make_unique<Pickable>(TargetSelector(TargetSelector::SelectorType::SELECTED_MONSTER, 5), std::make_unique<AiChangeEffect>(std::make_unique<ConfusedMonsterAi>(10), "The eyes of the %s look vacant!"));
		return confusor;
	} else if(r < 85) {
		std::unique_ptr<Actor> teslaCoil = std::make_unique<Actor>(x, y, '#', "tesla coil", sf::Color(128, 128, 255));
		teslaCoil->blocks = false;
		teslaCoil->pickable = std::make_unique<Pickable>(TargetSelector(TargetSelector::SelectorType::WEARER_RANGE, 5), std::make_unique<HealthEffect>(-6, "The tesla coil sputters, emitting raw\n electricity, hurting %s for %g hp!"));
		return teslaCoil;
	} else if(r < 90) {
		std::unique_ptr<Actor> baton = std::make_unique<Actor>(x, y, '|', "stun baton", sf::Color(128, 128, 255));
		baton->blocks = false;
		baton->pickable = std::make_unique<Pickable>();
		baton->attacker = std::make_unique<Attacker>(1, 8, 1);
		return baton;
	} else if(r < 95) {
		std::unique_ptr<Actor> knuckleduster = std::make_unique<Actor>(x, y, '|', "knuckle duster", sf::Color(128, 255, 128));
		knuckleduster->blocks = false;
		knuckleduster->pickable = std::make_unique<Pickable>();
		knuckleduster->attacker = std::make_unique<Attacker>(2, 6, 2);
		return knuckleduster;
	} else {
		std::unique_ptr<Actor> rock = std::make_unique<Actor>(x, y, '|', "rock", sf::Color(255, 128, 128));
		rock->blocks = false;
		rock->pickable = std::make_unique<Pickable>();
		rock->attacker = std::make_unique<Attacker>(1, 4, 0);
		return rock;
	}
}

void map_utils::addItem(GameplayState* gameplayState, Map* map, int x, int y) {
	gameplayState->addActor(map_utils::makeItem(gameplayState, map, x, y));
}

