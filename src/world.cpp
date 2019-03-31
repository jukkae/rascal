#include "world.hpp"

#include "ai.hpp"
#include "body.hpp"
#include "container.hpp"
#include "damage.hpp"
#include "destructible.hpp"
#include "dice.hpp"
#include "effect.hpp"
#include "event.hpp"
#include "fov.hpp"
#include "gameplay_state.hpp"
#include "ignore.hpp"
#include "map_utils.hpp"
#include <iostream>
#include "../include/toml.hpp"

MapType World::getMapType(int level) {
	MapType mapType;

	auto& levelsTable = map_utils::LevelsTable::getInstance().levelsTable;
	auto levelTable = toml::get<toml::table>(levelsTable.at(std::to_string(level)));
	std::string mapTypeFromToml = toml::get<std::string>(levelTable.at("mapType"));
	if(mapTypeFromToml == "Building") {
		mapType = MapType::BUILDING;
	} else if(mapTypeFromToml == "Water") {
		mapType = MapType::WATER;
	} else if(mapTypeFromToml == "Pillars") {
		mapType = MapType::PILLARS;
	} else if(mapTypeFromToml == "Hardcoded") {
		throw std::logic_error("Hardcoded maps are not implemented yet");
	} else {
		throw std::logic_error("This map type is not implemented yet");
	}
	return mapType;
}

World::World(int width, int height, int level, GameplayState* state):
width(width), height(height), level(level), state(state),
map(width, height, getMapType(level), this) {
	radiation = level;

	map_utils::addDoors(this, &map);
	//map_utils::addItems(this, &map, level);
	//map_utils::addMonsters(this, &map, level);
	map_utils::addMonstersBasedOnRoomTypes(this, &map, level);
	map_utils::addItemsBasedOnRoomTypes(this, &map, level);
	map_utils::fixMainPath(this, &map, level);
	for(auto& a : actors) {
		if(a->ai) a->ai->updateFov(a.get());
	}
}

void World::movePlayerFrom(World* other) {
	std::unique_ptr<Actor> player;

	auto it = other->actors.begin();
	while (it != other->actors.end()) {
		if ((*it)->isPlayer()) {
			player = std::move(*it); // i want to move(*it) move(*it)
			it = other->actors.erase(it);
		}
		else ++it;
	}
	player->world = this;
	for (auto& a : player->container->inventory) a->world = this;
	this->addActor(std::move(player));
	this->sortActors();
}

Actor* World::getPlayer() const {
	for(auto& actor : actors) {
        if(actor->isPlayer()) return actor.get();
    }
    return nullptr;
}

bool World::canWalk(int x, int y) {
	for(auto& actor : actors) {
		if(actor->blocks && actor->x == x && actor->y == y) {
			return false;
		}
	}
	return true;
}

void World::update() {
	Actor* activeActor = getNextActor();
	if(activeActor->ai) activeActor->ai->updateFov(activeActor); // update own fov before
	if(activeActor->isPlayer()) {
		state->handleEvents();
	}
	updateNextActor();
	if(activeActor->ai) activeActor->ai->updateFov(activeActor); // update own fov after
}

void World::updateNextActor() {
	Actor* activeActor = getNextActor();

	float actionTime = activeActor->update(state);
	*activeActor->energy -= actionTime;
	//applyRadiation(actionTime);

	sortActors();
	updateTime();
}

void World::applyRadiation(float dt) {
	if (dt > 0) {
		int r = d1000();
		int rx = r * dt;
		if(rx <= radiation * 50){
			Actor* player = getPlayer();
			int dmg = 0;
			if(d3() == 1) {
				switch(radiation){
					case 0:  dmg = 0;    break;
					case 1:  dmg = 1;    break;
					case 2:  dmg = d2(); break;
					case 3:  dmg = d3(); break;
					case 4:  dmg = d4(); break;
					case 5:  dmg = d5(); break;
					default: dmg = d6(); break;
				}
			}
			if(dmg != 0) {
				GenericActorEvent e(player, "You feel a bit sick from the radiation...");
				notify(e);
			}
			int iodine = player->body->iodine;
			if(d10() >= iodine) {
				player->destructible->takeDamage(player, dmg, DamageType::RADIATION);
			}
		}
	}
}

void World::updateTime() {
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

void World::sortActors() {
    std::sort(actors.begin(), actors.end(), [](const auto& lhs, const auto& rhs)
    {
        return lhs->energy > rhs->energy;
    });
}

Actor* World::getClosestMonster(int x, int y, float range) const {
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

Actor* World::getLiveActor(int x, int y) const {
	for(auto& actor : actors) {
		if(actor->x == x && actor->y == y && actor->destructible && !actor->destructible->isDead()) return actor.get();
	}
	return nullptr;
}

std::vector<Actor*> World::getActorsAt(int x, int y) {
	std::vector<Actor*> v;
	for(auto& actor : actors) {
		if(actor->x == x && actor->y == y) v.push_back(actor.get());
	}
	return v;
}

//TODO
void World::notify(Event& e) {
	e.time = time;
	if(state) state->notify(e);
	// TODO this is a dirty hack, ok?
	for(auto& mission : getPlayer()->missions) {
		mission->notify(e);
	}
	// TODO more dirty hacks
	if(auto event = dynamic_cast<EnemyHasSeenPlayerEvent*>(&e)) {
		ignore(event);
    getPlayer()->actionsQueue.clear();
	}
}
