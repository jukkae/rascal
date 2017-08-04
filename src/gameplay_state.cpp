#include "gameplay_state.hpp"
#include "engine.hpp"

void GameplayState::init() {
	actors = engine.actors;
	gui = &engine.gui;
	gui->setState(this);
	renderer = &engine.renderer;
	renderer->setState(this);

	player               = new Actor(40, 25, '@', "you", TCODColor::white, 2); // TODO
	player->destructible = std::unique_ptr<Destructible>(new PlayerDestructible(30, 2, 0, "your corpse"));
	player->attacker     = std::unique_ptr<Attacker>(new Attacker(5));
	player->ai           = std::unique_ptr<Ai>(new PlayerAi());
	player->container    = std::unique_ptr<Container>(new Container(26));
	actors->push_back(player);

	stairs = new Actor(0, 0, '>', "stairs", TCODColor::white);
    stairs->blocks = false;
    stairs->fovOnly = false;
    actors->push_back(stairs);
}

void GameplayState::initMap() {
	map = std::unique_ptr<Map>(new Map(120, 72));
	map->setState(this);
	map->init(true);
	for (auto a : *actors) a->setState(this);
}

void GameplayState::cleanup() {

}

void GameplayState::update(Engine* engine) {

}

void GameplayState::render(Engine* engine) {
	renderer->render(map.get(), actors.get());
	gui->render();
}

void GameplayState::updateNextActor() {
    Actor* activeActor = actors->at(0);

    int actionTime = activeActor->update(this);
    *activeActor->energy -= actionTime;

    actors->erase(actors->begin());
    auto it = std::lower_bound(actors->begin(), actors->end(), activeActor, [](const auto& lhs, const auto& rhs) { return lhs->energy > rhs->energy; });
    actors->insert(it, activeActor);
    /*std::sort(actors->begin(), actors->end(), [](const auto& lhs, const auto& rhs)
    {
        return lhs->energy > rhs->energy;
    });*/
}

Actor* GameplayState::getPlayer() const {
	for(Actor* actor : *actors) {
        if(actor->isPlayer()) return actor;
    }
    return nullptr;
}

Point GameplayState::getMouseLocation() {
	return Point(engine.mouse.cx, engine.mouse.cy);
}

bool GameplayState::canWalk(int x, int y) {
	for(Actor* actor : *actors) {
		if(actor->blocks && actor->x == x && actor->y == y) {
			return false;
		}
	}
	return true;
}
