#include "actor.hpp"
#include "ai.hpp"
#include "armor.hpp"
#include "attacker.hpp"
#include "body.hpp"
#include "colors.hpp"
#include "comestible.hpp"
#include "container.hpp"
#include "dice.hpp"
#include "destructible.hpp"
#include "effect.hpp"
#include "map_utils.hpp"
#include "map.hpp"
#include "pickable.hpp"
#include "status_effect.hpp"
#include "transporter.hpp"
#include "wieldable.hpp"
#include "world.hpp"
#include <SFML/Graphics.hpp>
#include "../include/toml.hpp"

void map_utils::addItems(World* world, Map* map, int difficulty) {
	for(int x = 0; x < map->width; ++x) {
		for(int y = 0; y < map->height; ++y) {
			int r = d100();
			if (!map->isWall(x, y) && r == 1) { // can't use canWalk yet
					world->addActor(item::makeItem(world, map, x, y, difficulty));
			}
		}
	}
}

void map_utils::addDoors(World* world, Map* map) {
	for(int x = 1; x < map->width-1; ++x) {
		for(int y = 1; y < map->height-1; ++y) {
			if (!map->isWall(x, y)) {
				if ((map->isWall(x-1, y) && map->isWall(x+1, y)) ||
					(map->isWall(x, y-1) && map->isWall(x, y+1))) {
					std::unique_ptr<Actor> door = std::make_unique<Actor>(x, y, '+', "door", sf::Color::Black, 0);
					door->openable = std::make_unique<Openable>();
					door->blocks = true;
					door->blocksLight = true;
					door->fovOnly = false;
					world->addActor(std::move(door));
				}
			}
		}
	}
}

void map_utils::addMonsters(World* world, Map* map, int difficulty) {
	for(int x = 0; x < map->width; ++x) {
		for(int y = 0; y < map->height; ++y) {
			int r = d100();
			int s = d3();
			if (!map->isWall(x, y) && r == 1 && s == 1) { // can't use canWalk yet
				world->addActor(npc::makeMonster(world, map, x, y, difficulty));
			}
		}
	}
}

void map_utils::addPlayer(World* world, Map* map) {
	int x = 0;
	int y = 0;
	do {
		int r = d100();
		int s = d100();
		x = (map->width-1) * r / 100;
		y = (map->height-1) * s / 100;
	} while (map->isWall(x, y)); // should check for canWalk, but can't do that yet

	std::unique_ptr<Actor> player = std::make_unique<Actor>(x, y, '@', "you", sf::Color::White, 2);
	player->attacker     = std::make_unique<Attacker>(1, 2, 1);
	player->ai           = std::make_unique<PlayerAi>();
	player->container    = std::make_unique<Container>(100);
	player->body         = std::make_unique<Body>(Body::createRandomBody());
	int hpDice = (3 + player->body->getModifier(player->body->endurance));
	int hp = 0;
	for(int i = 0; i < hpDice; ++i) hp += d4();
	player->destructible = std::make_unique<PlayerDestructible>(hp, 2, 0, "your corpse");
	world->addActor(std::move(player));
}

// TODO now just randomly sprinkle one upstairs, one downstairs
void map_utils::addStairs(World* world, Map* map) {
	int x = 0;
	int y = 0;
	do {
		int r = d100();
		int s = d100();
		x = (map->width-1) * r / 100;
		y = (map->height-1) * s / 100;
	} while (map->isWall(x, y)); // should check for canWalk, but can't do that yet

	std::unique_ptr<Actor> stairs = std::make_unique<Actor>(x, y, '<', "stairs (up)", sf::Color::White, boost::none);
    stairs->blocks = false;
    stairs->fovOnly = false;
	stairs->transporter = std::make_unique<Transporter>();
	stairs->transporter->direction = VerticalDirection::UP;
	world->addActor(std::move(stairs));

	do {
		int r = d100();
		int s = d100();
		x = (map->width-1) * r / 100;
		y = (map->height-1) * s / 100;
	} while (map->isWall(x, y)); // should check for canWalk, but can't do that yet

	std::unique_ptr<Actor> downstairs = std::make_unique<Actor>(x, y, '>', "stairs (down)", sf::Color::White, boost::none);
    downstairs->blocks = false;
    downstairs->fovOnly = false;
	downstairs->transporter = std::make_unique<Transporter>();
	downstairs->transporter->direction = VerticalDirection::DOWN;
	world->addActor(std::move(downstairs));
}

// TODO temp
void map_utils::addStairs(World* world, Map* map, int dsX, int dsY) {
	int x = 0;
	int y = 0;
	do {
		int r = d100();
		int s = d100();
		x = (map->width-1) * r / 100;
		y = (map->height-1) * s / 100;
	} while (map->isWall(x, y)); // should check for canWalk, but can't do that yet

	std::unique_ptr<Actor> stairs = std::make_unique<Actor>(x, y, '<', "stairs (up)", sf::Color::White, boost::none);
    stairs->blocks = false;
    stairs->fovOnly = false;
	stairs->transporter = std::make_unique<Transporter>();
	stairs->transporter->direction = VerticalDirection::UP;
	world->addActor(std::move(stairs));

	if (map->isWall(dsX, dsY)) map->tiles(dsX, dsY).walkable = true; // FIXME
	std::unique_ptr<Actor> downstairs = std::make_unique<Actor>(dsX, dsY, '>', "stairs (down)", sf::Color::White, boost::none);
    downstairs->blocks = false;
    downstairs->fovOnly = false;
	downstairs->transporter = std::make_unique<Transporter>();
	downstairs->transporter->direction = VerticalDirection::DOWN;
	world->addActor(std::move(downstairs));
}

void map_utils::addMcGuffin(World* world, Map* map, int level) {
	int x = 0;
	int y = 0;
	do {
		int r = d100();
		int s = d100();
		x = (map->width-1) * r / 100;
		y = (map->height-1) * s / 100;
	} while (map->isWall(x, y)); // should check for canWalk, but can't do that yet

	std::unique_ptr<Actor> mcGuffin = std::make_unique<Actor>(x, y, 'q', "phlebotinum link", sf::Color::White, 0);
    mcGuffin->blocks = false;
	mcGuffin->pickable = std::make_unique<Pickable>(TargetSelector(TargetSelector::SelectorType::NONE));
	world->addActor(std::move(mcGuffin));
}

std::unique_ptr<Actor> npc::makeMonster(World* world, Map* map, int x, int y, int difficulty) {
	int r = d100();
	std::unique_ptr<Actor> npc;
	switch(difficulty) {
		case 1: {
			auto& levels = map_utils::LevelsTable::getInstance().levelsTable;

			auto level = toml::get<toml::table>(levels.at("1"));
			auto beings = toml::get<std::vector<toml::table>>(level.at("beings"));
			std::vector<int> weights;
			std::vector<std::string> types;
			for(auto& beingTable : beings) {
				int weight = toml::get<int>(beingTable.at("with_weight"));
				std::string beingType = toml::get<std::string>(beingTable.at("being"));
				weights.push_back(weight);
				types.push_back(beingType);
			}
			std::random_device rd; // TODO should use one for the whole program
			std::mt19937 gen(rd());
			std::discrete_distribution<> d(weights.begin(), weights.end());
			npc = makeBeingFromToml(world, map, x, y, types.at(d(gen)));

			return npc;
		}
		case 2: {
			if(r < 40) {
				npc = makeBeingFromToml(world, map, x, y, "snake");
			} else if (r < 60) {
				npc = makeBeingFromToml(world, map, x, y, "child");
			} else if (r < 70) {
				npc = makeBeingFromToml(world, map, x, y, "guard");
			} else if (r < 75) {
				npc = makeBeingFromToml(world, map, x, y, "boxer");
			} else {
				npc = makeBeingFromToml(world, map, x, y, "punk");
			}
			return npc;
		}
		case 3: {
			if(r < 30) {
				npc = makeBeingFromToml(world, map, x, y, "guard");
			} else if (r < 50) {
				npc = makeBeingFromToml(world, map, x, y, "boxer");
			} else if (r < 75) {
				npc = makeBeingFromToml(world, map, x, y, "mutant");
			} else {
				npc = makeBeingFromToml(world, map, x, y, "punk");
			}
			return npc;
		}
		case 4: {
			if(r < 20) {
				npc = makeBeingFromToml(world, map, x, y, "guard");
			} else if (r < 40) {
				npc = makeBeingFromToml(world, map, x, y, "boxer");
			} else if (r < 65) {
				npc = makeBeingFromToml(world, map, x, y, "mutant");
			} else if (r < 75) {
				npc = makeBeingFromToml(world, map, x, y, "cyborg");
			} else {
				npc = makeBeingFromToml(world, map, x, y, "punk");
			}
			return npc;
		}
		case 5: {
			if(r < 20) {
				npc = makeBeingFromToml(world, map, x, y, "guard");
			} else if (r < 40) {
				npc = makeBeingFromToml(world, map, x, y, "boxer");
			} else if (r < 65) {
				npc = makeBeingFromToml(world, map, x, y, "mutant");
			} else if (r < 75) {
				npc = makeBeingFromToml(world, map, x, y, "cyborg");
			} else if (r < 80) {
				npc = makeBeingFromToml(world, map, x, y, "android");
			} else {
				npc = makeBeingFromToml(world, map, x, y, "punk");
			}
			return npc;
		}
		default: {
				npc = makeBeingFromToml(world, map, x, y, "punk");
				return npc;
			break;
		}
	}
}

std::unique_ptr<Actor> npc::makeBeingFromToml(World* world, Map* map, int x, int y, std::string type) {
	auto& beings = map_utils::BeingsTable::getInstance().beingsTable;

	auto being = toml::get<toml::table>(beings.at(type));
	auto a = std::make_unique<Actor>(x, y);
	a->energy = 1;
	// TODO should check if keys and values are valid
	a->ch = toml::get<std::string>(being.at("ch"))[0]; // TODO assert(string.size() == 1);
	a->name = toml::get<std::string>(being.at("name"));
	a->col = colors::get(toml::get<std::string>(being.at("color")));

	if(being.count("ai") != 0) {
		auto ai = toml::get<toml::table>(being.at("ai"));
		if(toml::get<std::string>(ai.at("type")) == "MonsterAi") {
			int speed = toml::get<int>(ai.at("speed"));
			a->ai = std::make_unique<MonsterAi>(speed); // TODO default
		} else { /* DO WHAT */ }
	}

	if(being.count("destructible") != 0) {
		auto destructible = toml::get<toml::table>(being.at("destructible"));
		int maxHp = toml::get<int>(destructible.at("maxHp"));
		int defense = toml::get<int>(destructible.at("defense"));
		int xp = toml::get<int>(destructible.at("xp"));
		std::string corpseName = toml::get<std::string>(destructible.at("corpseName"));
		a->destructible = std::make_unique<MonsterDestructible>(maxHp, defense, xp, corpseName);
	}

	if(being.count("attacker") != 0) {
		auto attacker = toml::get<toml::table>(being.at("attacker"));
		int numberOfDice = toml::get<int>(attacker.at("numberOfDice"));
		int dice = toml::get<int>(attacker.at("dice"));
		int bonus = toml::get<int>(attacker.at("bonus"));
		a->attacker = std::make_unique<Attacker>(numberOfDice, dice, bonus);
	}

	if(being.count("body") != 0) {
		auto body = toml::get<toml::table>(being.at("body"));
		a->body = std::make_unique<Body>();
	}

	if(being.count("container") != 0) {
		auto container = toml::get<toml::table>(being.at("container"));
		int capacity = toml::get<int>(container.at("capacity"));
		a->container = std::make_unique<Container>(capacity);
		auto contents = toml::get<std::vector<toml::table>>(container.at("contents"));
		for(auto& table: contents) {
			std::string type = toml::get<std::string>(table.at("content"));

			if(table.count("with_probability") != 0) {
				int probability = toml::get<int>(table.at("with_probability"));

				if(d100() <= probability) {
					auto item = item::makeItemFromToml(world, map, x, y, type);
					a->container->add(std::move(item));
				}
			} else {
				auto item = item::makeItemFromToml(world, map, x, y, type);
				a->container->add(std::move(item));
			}
		}
	}

	return a;
}

std::unique_ptr<Actor> item::makeItemFromToml(World* world, Map* map, int x, int y, std::string type) {
	auto& items = map_utils::ItemsTable::getInstance().itemsTable;
	auto item = toml::get<toml::table>(items.at(type));

	auto a = std::make_unique<Actor>(x, y);
	a->world = world;

	// TODO should check if keys and values are valid
	a->ch = toml::get<std::string>(item.at("ch"))[0]; // TODO assert(string.size() == 1);
	a->name = toml::get<std::string>(item.at("name"));
	a->col = colors::get(toml::get<std::string>(item.at("color")));

	a->blocks = toml::get<bool>(item.at("blocks"));

	if(item.count("pickable") != 0) {
		auto pickable = toml::get<toml::table>(item.at("pickable"));
		a->pickable = std::make_unique<Pickable>();

		TargetSelector t; // TODO ughh
		if(pickable.count("targetSelector") != 0) {
			auto targetSelector = toml::get<toml::table>(pickable.at("targetSelector"));
			std::string targetSelectorType = toml::get<std::string>(targetSelector.at("type"));
			int targetSelectorRange = toml::get<int>(targetSelector.at("range"));
			if(targetSelectorType == "Wearer") {
				t = TargetSelector(TargetSelector::SelectorType::WEARER, targetSelectorRange);
			} else if (targetSelectorType == "ClosestMonster") {
				t = TargetSelector(TargetSelector::SelectorType::CLOSEST_MONSTER, targetSelectorRange);
			} else if (targetSelectorType == "SelectedRange") {
				t = TargetSelector(TargetSelector::SelectorType::SELECTED_RANGE, targetSelectorRange);
			} else if (targetSelectorType == "SelectedMonster") {
				t = TargetSelector(TargetSelector::SelectorType::SELECTED_MONSTER, targetSelectorRange);
			} else if (targetSelectorType == "WearerRange") {
				t = TargetSelector(TargetSelector::SelectorType::WEARER_RANGE, targetSelectorRange);
			}
			else throw std::logic_error("Not implemented");
		}

		std::unique_ptr<Effect> e = nullptr;
		if(pickable.count("effect") != 0) {
			auto effect = toml::get<toml::table>(pickable.at("effect"));
			std::string effectType = toml::get<std::string>(effect.at("type"));
			if(effectType == "HealthEffect") {
				int effectValue = toml::get<int>(effect.at("value"));
				e = std::make_unique<HealthEffect>(effectValue);
			} else if (effectType == "IodineEffect") {
				int effectValue = toml::get<int>(effect.at("value"));
				e = std::make_unique<HealthEffect>(effectValue, HealthEffectType::IODINE);
			} else if (effectType == "PoisonEffect") {
				e = std::make_unique<StatusEffectEffect>(std::make_unique<PoisonedStatusEffect>(PoisonedStatusEffect()));
			} else if (effectType == "RemovePoisonEffect") {
				e = std::make_unique<StatusEffectRemovalEffect>(std::make_unique<PoisonedStatusEffect>(PoisonedStatusEffect()));
			} else if (effectType == "ConfusedAiEffect") {
				int effectValue = toml::get<int>(effect.at("value"));
				e = std::make_unique<AiChangeEffect>(std::make_unique<ConfusedMonsterAi>(effectValue));
			}

			else throw std::logic_error("Not implemented");
		}

		if(pickable.count("targetSelector") != 0 &&
	     pickable.count("effect") != 0) { // TODO bad check, i know
			a->pickable = std::make_unique<Pickable>(t, std::move(e));
		}
		if(pickable.count("fragile") != 0) {
			bool fragile = toml::get<bool>(pickable.at("fragile"));
			a->pickable->fragile = fragile;
		}
		if(pickable.count("explosive") != 0) {
			bool explosive = toml::get<bool>(pickable.at("explosive"));
			a->pickable->explosive = explosive;
		}
		if(pickable.count("weight") != 0) {
			int weight = toml::get<int>(pickable.at("weight"));
			a->pickable->weight = weight;
		}
	}

	if(item.count("comestible") != 0) {
		auto comestible = toml::get<toml::table>(item.at("comestible"));
		a->comestible = std::make_unique<Comestible>();
		if(comestible.count("nutrition") != 0) {
			int nutrition = toml::get<int>(comestible.at("nutrition"));
			a->comestible->nutrition = nutrition;
		}
	}

	if(item.count("attacker") != 0) {
		auto attacker = toml::get<toml::table>(item.at("attacker"));
		int numberOfDice = toml::get<int>(attacker.at("numberOfDice"));
		int dice = toml::get<int>(attacker.at("dice"));
		int bonus = toml::get<int>(attacker.at("bonus"));
		a->attacker = std::make_unique<Attacker>(numberOfDice, dice, bonus);

		if(attacker.count("effect") != 0) {
			std::string effectType = toml::get<std::string>(attacker.at("effect"));
			if(effectType == "MoveEffect") {
				a->attacker->effectGenerator = std::make_unique<EffectGeneratorFor<MoveEffect>>();
			} else if(effectType == "SpeedEffect") {
				int effectValue = toml::get<int>(attacker.at("effectValue"));
				a->attacker->effectGenerator = std::make_unique<EffectGeneratorFor<StatusEffectEffect>>(Attribute::SPEED, effectValue);
			}
			else throw std::logic_error("Not implemented");
		}
	}

	if(item.count("rangedAttacker") != 0) {
		auto rangedAttacker = toml::get<toml::table>(item.at("rangedAttacker"));
		int numberOfDice = toml::get<int>(rangedAttacker.at("numberOfDice"));
		int dice = toml::get<int>(rangedAttacker.at("dice"));
		int bonus = toml::get<int>(rangedAttacker.at("bonus"));
		float range = toml::get<float>(rangedAttacker.at("range"));
		a->rangedAttacker = std::make_unique<RangedAttacker>(numberOfDice, dice, bonus, range);
	}

	if(item.count("wieldable") != 0) {
		auto wieldable = toml::get<toml::table>(item.at("wieldable"));
		std::string wieldableType = toml::get<std::string>(wieldable.at("type"));
		if(wieldableType == "OneHand") {
			a->wieldable = std::make_unique<Wieldable>(WieldableType::ONE_HAND);
		} else if (wieldableType == "Torso") {
			a->wieldable = std::make_unique<Wieldable>(WieldableType::TORSO);
		} else if (wieldableType == "Head") {
			a->wieldable = std::make_unique<Wieldable>(WieldableType::HEAD);
		} else if (wieldableType == "Feet") {
			a->wieldable = std::make_unique<Wieldable>(WieldableType::FEET);
		} else if (wieldableType == "TwoHands") {
			a->wieldable = std::make_unique<Wieldable>(WieldableType::TWO_HANDS);
		}
		else throw std::logic_error("Not implemented");
	}

	if(item.count("armor") != 0) {
		auto armorTable = toml::get<toml::table>(item.at("armor"));
		int armorValue = toml::get<int>(armorTable.at("value"));
		a->armor = std::make_unique<Armor>(armorValue);
	}

	return a;
}

std::unique_ptr<Actor> item::makeItem(World* world, Map* map, int x, int y, int difficulty) {
	int r = d100();
	if(r < 15) {
		return makeItemFromToml(world, map, x, y, "jerky");
	} else if(r < 25) {
		return makeItemFromToml(world, map, x, y, "iodine_syringe");
	} else if(r < 45) {
		return makeItemFromToml(world, map, x, y, "stimpak");
	} else if(r < 50) {
		return makeItemFromToml(world, map, x, y, "fake_stimpak");
	} else if(r < 55) {
		return makeItemFromToml(world, map, x, y, "antidote");
	} else if(r < 60) {
		return makeItemFromToml(world, map, x, y, "blaster_bolt_device");
	} else if(r < 65) {
		return makeItemFromToml(world, map, x, y, "frag_grenade");
	} else if(r < 70) {
		return makeItemFromToml(world, map, x, y, "confusor");
	} else if(r < 75) {
		return makeItemFromToml(world, map, x, y, "tesla_coil");
	} else if(r < 80) {
		return makeItemFromToml(world, map, x, y, "rock");
	} else if(r < 85/* && difficulty >= 2*/) {
		return makeItemFromToml(world, map, x, y, "baton");
	} else if(r < 90/* && difficulty >= 3*/) {
		return makeItemFromToml(world, map, x, y, "knuckleduster");
	} else if(r < 92) {
		return makeItemFromToml(world, map, x, y, "pistol");
	} else if(r < 94/* && difficulty >= 3*/){
		return makeItemFromToml(world, map, x, y, "rifle");
	} else if(r < 95){
		return makeItemFromToml(world, map, x, y, "leather_armor");
	} else if(r < 96/* && difficulty >= 4*/){
		return makeItemFromToml(world, map, x, y, "combat_armor");
	} else if(r < 97){
		return makeItemFromToml(world, map, x, y, "flat_cap");
	} else if(r < 98/* && difficulty >= 2*/){
		return makeItemFromToml(world, map, x, y, "combat_helmet");
	} else if(r < 99){
		return makeItemFromToml(world, map, x, y, "combat_boots");
	} else {
		return makeItemFromToml(world, map, x, y, "navcomp");
	}
}
