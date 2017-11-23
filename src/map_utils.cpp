#include "colors.hpp"
#include "dice.hpp"
#include "map_utils.hpp"
#include "map.hpp"
#include "gameplay_state.hpp"
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
	} while (map->isWall(x, y));

	std::unique_ptr<Actor> player = std::make_unique<Actor>(x, y, '@', "you", sf::Color::White, 2);
	player->destructible = std::make_unique<PlayerDestructible>(100, 2, 0, "your corpse", 11);
	player->attacker     = std::make_unique<Attacker>(5);
	player->ai           = std::make_unique<PlayerAi>();
	player->container    = std::make_unique<Container>(26);
	gameplayState->addActor(std::move(player));
}

void map_utils::addStairs(GameplayState* gameplayState, Map* map) {
	std::unique_ptr<Actor> stairs = std::make_unique<Actor>(1, 1, '>', "stairs", sf::Color::White, 0, true);
    stairs->blocks = false;
    stairs->fovOnly = false;
	gameplayState->addActor(std::move(stairs));
}

std::unique_ptr<Actor> map_utils::makeMonster(GameplayState* gameplayState, Map* map, int x, int y) {
	int r = d100();
	if(r < 70) {
		std::unique_ptr<Actor> punk = std::make_unique<Actor>(x, y, 'h', "punk", colors::desaturatedGreen, 1);
		punk->destructible = std::make_unique<MonsterDestructible>(10, 0, 50, "dead punk", 13);
		punk->attacker = std::make_unique<Attacker>(3);
		punk->ai = std::make_unique<MonsterAi>();
		return punk;
	} else if (r < 80) {
		std::unique_ptr<Actor> fighter = std::make_unique<Actor>(x, y, 'H', "fighter", colors::darkGreen, 1);
		fighter->destructible = std::make_unique<MonsterDestructible>(16, 1, 100, "fighter carcass", 15);
		fighter->attacker = std::make_unique<Attacker>(4);
		fighter->ai = std::make_unique<MonsterAi>();
		return fighter;
	}
	else {
		std::unique_ptr<Actor> boxer = std::make_unique<Actor>(x, y, 'H', "boxer", colors::darkerGreen, 1);
		boxer->destructible = std::make_unique<MonsterDestructible>(4, 1, 70, "boxer carcass", 16);
		boxer->attacker = std::make_unique<Attacker>(4);
		boxer->ai = std::make_unique<MonsterAi>();
		return boxer;
	}
}

void map_utils::addMonster(GameplayState* gameplayState, Map* map, int x, int y) {
	gameplayState->addActor(map_utils::makeMonster(gameplayState, map, x, y));
}

std::unique_ptr<Actor> map_utils::makeItem(GameplayState* gameplayState, Map* map, int x, int y) {
	int r = d100();
	if(r < 60) {
		std::unique_ptr<Actor> stimpak = std::make_unique<Actor>(x, y, '!', "stimpak", sf::Color(128, 0, 128));
		stimpak->blocks = false;
		stimpak->pickable = std::make_unique<Pickable>(TargetSelector(TargetSelector::SelectorType::WEARER, 0), std::make_unique<HealthEffect>(4));
		return stimpak;
	} else if(r < 70) {
		std::unique_ptr<Actor> blasterBoltDevice = std::make_unique<Actor>(x, y, '?', "blaster bolt device", sf::Color(128, 128, 0));
		blasterBoltDevice->blocks = false;
		blasterBoltDevice->pickable = std::make_unique<Pickable>(TargetSelector(TargetSelector::SelectorType::CLOSEST_MONSTER, 5), std::make_unique<HealthEffect>(-20, "The %s is hit by a blast!\n The damage is %g hit points."));
		return blasterBoltDevice;
	} else if(r < 80) {
		std::unique_ptr<Actor> fragGrenade = std::make_unique<Actor>(x, y, '?', "fragmentation grenade", sf::Color(128, 255, 128));
		fragGrenade->blocks = false;
		fragGrenade->pickable = std::make_unique<Pickable>(TargetSelector(TargetSelector::SelectorType::SELECTED_RANGE, 3), std::make_unique<HealthEffect>(-12, "The grenade explodes, hurting the %s for %g hit points!"));
		return fragGrenade;
	} else if(r < 90) {
		std::unique_ptr<Actor> confusor = std::make_unique<Actor>(x, y, '?', "confusor", sf::Color(128, 128, 255));
		confusor->blocks = false;
		confusor->pickable = std::make_unique<Pickable>(TargetSelector(TargetSelector::SelectorType::SELECTED_MONSTER, 5), std::make_unique<AiChangeEffect>(std::make_unique<ConfusedMonsterAi>(10), "The eyes of the %s look vacant!"));
		return confusor;
	} else {
		std::unique_ptr<Actor> teslaCoil = std::make_unique<Actor>(x, y, '#', "tesla coil", sf::Color(128, 128, 255));
		teslaCoil->blocks = false;
		teslaCoil->pickable = std::make_unique<Pickable>(TargetSelector(TargetSelector::SelectorType::WEARER_RANGE, 5), std::make_unique<HealthEffect>(-6, "The tesla coil sputters, emitting raw\n electricity, hurting %s for %g hp!"));
		return teslaCoil;
	}
}

void map_utils::addItem(GameplayState* gameplayState, Map* map, int x, int y) {
	gameplayState->addActor(map_utils::makeItem(gameplayState, map, x, y));
}

