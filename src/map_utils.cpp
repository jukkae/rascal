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
			//
			r = 0;
			if(r == 0) {
				npc = makeBeingFromToml(world, map, x, y, "guard");
				return npc;
			}
			//
			if(r < 50) {
				npc = makeBeingFromToml(world, map, x, y, "dog");
				return npc;
			} else if (r < 60) {
				npc = makeBeingFromToml(world, map, x, y, "snake");
				return npc;
			} else if (r < 90) {
				npc = makeBeingFromToml(world, map, x, y, "child");
				return npc;
			} else {
				npc = makeBeingFromToml(world, map, x, y, "punk");
				return npc;
			}
		}
		case 2: {
			if(r < 40) {
				npc = makeBeingFromToml(world, map, x, y, "snake");
				return npc;
			} else if (r < 60) {
				npc = makeBeingFromToml(world, map, x, y, "child");
				return npc;
			} else if (r < 70) {
				npc = makeBeingFromToml(world, map, x, y, "guard");
				return npc;
			} else if (r < 75) {
				npc = makeBoxer(world, map, x, y);
				return npc;
			} else {
				npc = makeBeingFromToml(world, map, x, y, "punk");
				return npc;
			}
		}
		case 3: {
			if(r < 30) {
				npc = makeBeingFromToml(world, map, x, y, "guard");
				return npc;
			} else if (r < 50) {
				npc = makeBoxer(world, map, x, y);
				return npc;
			} else if (r < 75) {
				npc = makeMutant(world, map, x, y);
				return npc;
			} else {
				npc = makeBeingFromToml(world, map, x, y, "punk");
				return npc;
			}
		}
		case 4: {
			if(r < 20) {
				npc = makeBeingFromToml(world, map, x, y, "guard");
				return npc;
			} else if (r < 40) {
				npc = makeBoxer(world, map, x, y);
				return npc;
			} else if (r < 65) {
				npc = makeMutant(world, map, x, y);
				return npc;
			} else if (r < 75) {
				npc = makeCyborg(world, map, x, y);
				return npc;
			} else {
				npc = makeBeingFromToml(world, map, x, y, "punk");
				return npc;
			}
		}
		case 5: {
			if(r < 20) {
				npc = makeBeingFromToml(world, map, x, y, "guard");
				return npc;
			} else if (r < 40) {
				npc = makeBoxer(world, map, x, y);
				return npc;
			} else if (r < 65) {
				npc = makeMutant(world, map, x, y);
				return npc;
			} else if (r < 75) {
				npc = makeCyborg(world, map, x, y);
				return npc;
			} else if (r < 80) {
				npc = makeAndroid(world, map, x, y);
				return npc;
			} else {
				npc = makeBeingFromToml(world, map, x, y, "punk");
				return npc;
			}
		}
		default: {
				npc = makeBeingFromToml(world, map, x, y, "punk");
				return npc;
			break;
		}
	}
}

std::unique_ptr<Actor> npc::makeBeingFromToml(World* world, Map* map, int x, int y, std::string type) {
	auto beings = toml::parse("assets/beings.toml");

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
	auto items = toml::parse("assets/items.toml");
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
			}
			else throw std::logic_error("Not implemented");
		}

		std::unique_ptr<Effect> e = nullptr;
		if(pickable.count("effect") != 0) {
			auto effect = toml::get<toml::table>(pickable.at("effect"));
			std::string effectType = toml::get<std::string>(effect.at("type"));
			int effectValue = toml::get<int>(effect.at("value"));
			if(effectType == "HealthEffect") {
				e = std::make_unique<HealthEffect>(effectValue);
			}
			else throw std::logic_error("Not implemented");
		}
		if(pickable.count("targetSelector") != 0 &&
	     pickable.count("effect") != 0) { // TODO bad check, i know
			a->pickable = std::make_unique<Pickable>(t, std::move(e));
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

	return a;
}

std::unique_ptr<Actor> npc::makeFighter(World* world, Map* map, int x, int y) {
		std::unique_ptr<Actor> a = std::make_unique<Actor>(x, y, 'H', "fighter", colors::get("darkGreen"), 1);
		a->destructible = std::make_unique<MonsterDestructible>(5, 1, 100, "fighter carcass");
		a->attacker = std::make_unique<Attacker>(2, 3, 0);
		a->ai = std::make_unique<MonsterAi>();
		a->body = std::make_unique<Body>();

		a->container = std::make_unique<Container>(10);

		std::unique_ptr<Actor> jerky = std::make_unique<Actor>(x, y, '%', "jerky", sf::Color(128, 0, 0));
		jerky->blocks = false;
		jerky->pickable = std::make_unique<Pickable>();
		jerky->comestible = std::make_unique<Comestible>();
		jerky->world = world;

		a->container->add(std::move(jerky));

		return a;
}

std::unique_ptr<Actor> npc::makeBoxer(World* world, Map* map, int x, int y) {
		std::unique_ptr<Actor> a = std::make_unique<Actor>(x, y, 'H', "boxer", colors::get("darkerGreen"), 1);
		a->destructible = std::make_unique<MonsterDestructible>(4, 0, 70, "boxer carcass");
		a->attacker = std::make_unique<Attacker>(1, 4, 2);
		a->ai = std::make_unique<MonsterAi>();
		a->body = std::make_unique<Body>();
		a->container = std::make_unique<Container>(10);
		if(d6() == 6) {
			std::unique_ptr<Actor> knuckleduster = std::make_unique<Actor>(x, y, '|', "knuckle duster", sf::Color(128, 255, 128));
			knuckleduster->blocks = false;
			knuckleduster->pickable = std::make_unique<Pickable>();
			knuckleduster->attacker = std::make_unique<Attacker>(2, 6, 2);
			knuckleduster->attacker->effectGenerator = std::make_unique<EffectGeneratorFor<MoveEffect>>();
			knuckleduster->wieldable = std::make_unique<Wieldable>(WieldableType::ONE_HAND);
			knuckleduster->world = world;
			a->container->add(std::move(knuckleduster));
		}

		if(d6() >= 3) {
			std::unique_ptr<Actor> stimpak = std::make_unique<Actor>(x, y, '!', "stimpak", sf::Color(128, 128, 128));
			stimpak->blocks = false;
			stimpak->pickable = std::make_unique<Pickable>(TargetSelector(TargetSelector::SelectorType::WEARER, 0), std::make_unique<HealthEffect>(4));
			stimpak->world = world; //FIXME that I need to do this is bad and error-prone. Figure out better ways of actor creation.

			a->container->add(std::move(stimpak));
		}
		return a;
}

std::unique_ptr<Actor> npc::makeMutant(World* world, Map* map, int x, int y) {
		std::unique_ptr<Actor> a = std::make_unique<Actor>(x, y, 'm', "mutant", colors::get("darkerGreen"), 1);
		a->destructible = std::make_unique<MonsterDestructible>(8, 0, 200, "mutant carcass");
		a->attacker = std::make_unique<Attacker>(2, 4, 0);
		a->ai = std::make_unique<MonsterAi>(90);
		a->body = std::make_unique<Body>();
		a->container = std::make_unique<Container>(10);

		std::unique_ptr<Actor> ration = std::make_unique<Actor>(x, y, '%', "ration", sf::Color(0, 128, 0));
		ration->blocks = false;
		ration->pickable = std::make_unique<Pickable>();
		ration->comestible = std::make_unique<Comestible>();
		ration->comestible->nutrition = 30000;
		ration->world = world; //FIXME that I need to do this is bad and error-prone. Figure out better ways of actor creation.

		a->container->add(std::move(ration));
		return a;
}

std::unique_ptr<Actor> npc::makeCyborg(World* world, Map* map, int x, int y) {
		std::unique_ptr<Actor> a = std::make_unique<Actor>(x, y, 'r', "cyborg", colors::get("darkerGreen"), 1);
		a->destructible = std::make_unique<MonsterDestructible>(8, 0, 250, "cyborg carcass");
		a->attacker = std::make_unique<Attacker>(1, 8, 0);
		a->ai = std::make_unique<MonsterAi>(120);
		a->body = std::make_unique<Body>();
		a->container = std::make_unique<Container>(10);

		std::unique_ptr<Actor> ration = std::make_unique<Actor>(x, y, '%', "ration", sf::Color(0, 128, 0));
		ration->blocks = false;
		ration->pickable = std::make_unique<Pickable>();
		ration->comestible = std::make_unique<Comestible>();
		ration->comestible->nutrition = 30000;
		ration->world = world; //FIXME that I need to do this is bad and error-prone. Figure out better ways of actor creation.

		a->container->add(std::move(ration));
		return a;
}

std::unique_ptr<Actor> npc::makeAndroid(World* world, Map* map, int x, int y) {
		std::unique_ptr<Actor> a = std::make_unique<Actor>(x, y, 'h', "punk", colors::get("darkerGreen"), 1);
		a->destructible = std::make_unique<MonsterDestructible>(8, 1, 300, "broken android");
		a->attacker = std::make_unique<Attacker>(2, 6, 2);
		a->ai = std::make_unique<MonsterAi>(150);
		a->body = std::make_unique<Body>();
		return a;
}

std::unique_ptr<Actor> item::makeItem(World* world, Map* map, int x, int y, int difficulty) {
	int r = d100();
	if(r < 15) {
		std::unique_ptr<Actor> jerky = std::make_unique<Actor>(x, y, '%', "jerky", sf::Color(128, 0, 0));
		jerky->blocks = false;
		jerky->pickable = std::make_unique<Pickable>();
		jerky->comestible = std::make_unique<Comestible>();
		return jerky;
	} else if(r < 25) {
		std::unique_ptr<Actor> a = std::make_unique<Actor>(x, y, '!', "iodine syringe", sf::Color(128, 128, 0));
		a->blocks = false;
		a->pickable = std::make_unique<Pickable>(TargetSelector(TargetSelector::SelectorType::WEARER, 0), std::make_unique<HealthEffect>(d3()+2, HealthEffectType::IODINE));
		a->pickable->fragile = true;
		return a;
	} else if(r < 45) {
		std::unique_ptr<Actor> stimpak = std::make_unique<Actor>(x, y, '!', "stimpak", sf::Color(128, 0, 128));
		int amount = difficulty + d4();
		stimpak->blocks = false;
		stimpak->pickable = std::make_unique<Pickable>(TargetSelector(TargetSelector::SelectorType::WEARER, 0), std::make_unique<HealthEffect>(amount));
		stimpak->pickable->fragile = true;
		return stimpak;
	} else if(r < 50) {
		std::unique_ptr<Actor> fakeStimpak = std::make_unique<Actor>(x, y, '!', "stimpak", sf::Color(128, 0, 128));
		fakeStimpak->blocks = false;
		fakeStimpak->pickable = std::make_unique<Pickable>(TargetSelector(TargetSelector::SelectorType::WEARER, 0), std::make_unique<StatusEffectEffect>(std::make_unique<PoisonedStatusEffect>(PoisonedStatusEffect())));
		fakeStimpak->pickable->fragile = true;
		return fakeStimpak;
	} else if(r < 55) {
		std::unique_ptr<Actor> antidote = std::make_unique<Actor>(x, y, '!', "antidote", sf::Color(0, 128, 128));
		antidote->blocks = false;
		antidote->pickable = std::make_unique<Pickable>(TargetSelector(TargetSelector::SelectorType::WEARER, 0), std::make_unique<StatusEffectRemovalEffect>(std::make_unique<PoisonedStatusEffect>(PoisonedStatusEffect())));
		antidote->pickable->fragile = true;
		return antidote;
	} else if(r < 60) {
		std::unique_ptr<Actor> blasterBoltDevice = std::make_unique<Actor>(x, y, '?', "blaster bolt device", sf::Color(128, 128, 0));
		blasterBoltDevice->blocks = false;
		blasterBoltDevice->pickable = std::make_unique<Pickable>(TargetSelector(TargetSelector::SelectorType::CLOSEST_MONSTER, 5), std::make_unique<HealthEffect>(-20));
		return blasterBoltDevice;
	} else if(r < 65) {
		std::unique_ptr<Actor> fragGrenade = std::make_unique<Actor>(x, y, '?', "fragmentation grenade", sf::Color(0, 128, 128));
		fragGrenade->blocks = false;
		fragGrenade->pickable = std::make_unique<Pickable>(TargetSelector(TargetSelector::SelectorType::SELECTED_RANGE, 3), std::make_unique<HealthEffect>(-12));
		fragGrenade->pickable->fragile = true;
		fragGrenade->pickable->explosive = true;
		return fragGrenade;
	} else if(r < 70) {
		std::unique_ptr<Actor> confusor = std::make_unique<Actor>(x, y, '?', "confusor", sf::Color(128, 0, 128));
		confusor->blocks = false;
		confusor->pickable = std::make_unique<Pickable>(TargetSelector(TargetSelector::SelectorType::SELECTED_MONSTER, 5), std::make_unique<AiChangeEffect>(std::make_unique<ConfusedMonsterAi>(10)));
		return confusor;
	} else if(r < 75) {
		std::unique_ptr<Actor> teslaCoil = std::make_unique<Actor>(x, y, '#', "tesla coil", sf::Color(128, 128, 128));
		teslaCoil->blocks = false;
		teslaCoil->pickable = std::make_unique<Pickable>(TargetSelector(TargetSelector::SelectorType::WEARER_RANGE, 5), std::make_unique<HealthEffect>(-6));
		return teslaCoil;
	} else if(r < 80) {
		std::unique_ptr<Actor> rock = std::make_unique<Actor>(x, y, '|', "rock", sf::Color(255, 0, 128));
		rock->blocks = false;
		rock->pickable = std::make_unique<Pickable>();
		rock->pickable->weight = 5;
		rock->attacker = std::make_unique<Attacker>(1, 4, 0);
		rock->wieldable = std::make_unique<Wieldable>(WieldableType::ONE_HAND);
		return rock;
	} else if(r < 85 && difficulty >= 2) {
		std::unique_ptr<Actor> baton = std::make_unique<Actor>(x, y, '|', "stun baton", sf::Color(0, 128, 255));
		baton->blocks = false;
		baton->pickable = std::make_unique<Pickable>();
		baton->attacker = std::make_unique<Attacker>(1, 3, 1);
		baton->attacker->effectGenerator = std::make_unique<EffectGeneratorFor<StatusEffectEffect>>(Attribute::SPEED, -4);
		baton->wieldable = std::make_unique<Wieldable>(WieldableType::ONE_HAND);
		return baton;
	} else if(r < 90 && difficulty >= 3) {
		std::unique_ptr<Actor> knuckleduster = std::make_unique<Actor>(x, y, '|', "knuckle duster", sf::Color(128, 255, 0));
		knuckleduster->blocks = false;
		knuckleduster->pickable = std::make_unique<Pickable>();
		knuckleduster->attacker = std::make_unique<Attacker>(2, 6, 2);
		knuckleduster->attacker->effectGenerator = std::make_unique<EffectGeneratorFor<MoveEffect>>();
		knuckleduster->wieldable = std::make_unique<Wieldable>(WieldableType::ONE_HAND);
		return knuckleduster;
	} else if(r < 92) {
		std::unique_ptr<Actor> pistol = std::make_unique<Actor>(x, y, '\\', "pistol", sf::Color(0, 128, 255));
		pistol->blocks = false;
		pistol->pickable = std::make_unique<Pickable>();
		pistol->rangedAttacker = std::make_unique<RangedAttacker>(1, 4, 0, 10.0);
		pistol->wieldable = std::make_unique<Wieldable>(WieldableType::ONE_HAND);
		return pistol;
	} else if(r < 94 && difficulty >= 3){
		std::unique_ptr<Actor> rifle = std::make_unique<Actor>(x, y, '\\', "rifle", sf::Color(128, 0, 255));
		rifle->blocks = false;
		rifle->pickable = std::make_unique<Pickable>();
		rifle->rangedAttacker = std::make_unique<RangedAttacker>(2, 6, 0, 10.0);
		rifle->wieldable = std::make_unique<Wieldable>(WieldableType::TWO_HANDS);
		return rifle;
	} else if(r < 95){
		std::unique_ptr<Actor> armor = std::make_unique<Actor>(x, y, '[', "leather armor", sf::Color(0, 128, 255));
		armor->blocks = false;
		armor->pickable = std::make_unique<Pickable>();
		armor->wieldable = std::make_unique<Wieldable>(WieldableType::TORSO);
		armor->armor = std::make_unique<Armor>(1);
		return armor;
	} else if(r < 96 && difficulty >= 4){
		std::unique_ptr<Actor> armor = std::make_unique<Actor>(x, y, '[', "combat armor", sf::Color(128, 255, 0));
		armor->blocks = false;
		armor->pickable = std::make_unique<Pickable>();
		armor->wieldable = std::make_unique<Wieldable>(WieldableType::TORSO);
		armor->armor = std::make_unique<Armor>(2);
		return armor;
	} else if(r < 97){
		std::unique_ptr<Actor> armor = std::make_unique<Actor>(x, y, '[', "flat cap", sf::Color(0, 128, 128));
		armor->blocks = false;
		armor->pickable = std::make_unique<Pickable>();
		armor->wieldable = std::make_unique<Wieldable>(WieldableType::HEAD);
		armor->armor = std::make_unique<Armor>(1);
		return armor;
	} else if(r < 98 && difficulty >= 2){
		std::unique_ptr<Actor> armor = std::make_unique<Actor>(x, y, '[', "combat helmet", sf::Color(255, 0, 128));
		armor->blocks = false;
		armor->pickable = std::make_unique<Pickable>();
		armor->wieldable = std::make_unique<Wieldable>(WieldableType::HEAD);
		armor->armor = std::make_unique<Armor>(2);
		return armor;
	} else if(r < 99){
		std::unique_ptr<Actor> armor = std::make_unique<Actor>(x, y, '[', "combat boots", sf::Color(128, 0, 128));
		armor->blocks = false;
		armor->pickable = std::make_unique<Pickable>();
		armor->wieldable = std::make_unique<Wieldable>(WieldableType::FEET);
		armor->armor = std::make_unique<Armor>(1);
		return armor;
	} else {
		std::unique_ptr<Actor> navcomp = std::make_unique<Actor>(x, y, 'q', "navigation computer", sf::Color::Blue, 0);
		navcomp->blocks = false;
		navcomp->pickable = std::make_unique<Pickable>(TargetSelector(TargetSelector::SelectorType::NONE));
		return navcomp;
	}
}
