#include "world.hpp"

#include "body.hpp"
#include "damage.hpp"
#include "destructible.hpp"
#include "dice.hpp"
#include "effect.hpp"
#include "event.hpp"
#include "gameplay_state.hpp"
#include <iostream>

World::World(int width, int height): width(width), height(height) {
	map = Map(width, height);
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
	if(activeActor->isPlayer()) {
		state->handleEvents();
	}
	updateNextActor();
}

void World::updateNextActor() {
	Actor* activeActor = getNextActor();

	float actionTime = activeActor->update(state);
	*activeActor->energy -= actionTime;
	applyRadiation(actionTime);

    /*actors.erase(actors.begin());
    auto it = std::lower_bound(actors.begin(), actors.end(), activeActor, [](const auto& lhs, const auto& rhs) { return lhs->energy > rhs->energy; });
    actors.insert(it, std::move(activeActor));*/

	sortActors();
	updateTime();
}

void World::applyRadiation(float dt) {
	if (dt > 0) {
		int r = d1000();
		int rx = r * dt;
		if(rx <= radiation * 100){
			int dmg = 0;
			if(d3() == 1) {
				dmg = d2();
			}
			Actor* player = getPlayer();
			int iodine = player->body->iodine;
			if(d10() >= iodine) {
				player->destructible->takeDamage(player, dmg, DamageType::RADIATION);
			}
			GenericActorEvent e(player, "You feel a bit sick from the radiation...");
			notify(e);
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
}
