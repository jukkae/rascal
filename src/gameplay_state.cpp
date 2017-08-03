#include "gameplay_state.hpp"
#include "engine.hpp"

void GameplayState::init() {
	actors = engine.actors;
	gui = &engine.gui;

	player               = new Actor(40, 25, '@', "you", TCODColor::white, 2); // TODO
	player->destructible = std::unique_ptr<Destructible>(new PlayerDestructible(30, 2, 0, "your corpse"));
	player->attacker     = std::unique_ptr<Attacker>(new Attacker(5));
	player->ai           = std::unique_ptr<Ai>(new PlayerAi());
	player->container    = std::unique_ptr<Container>(new Container(26));
	actors->push_back(player);
}

void GameplayState::cleanup() {

}

void GameplayState::update(Engine* engine) {

}

void GameplayState::render(Engine* engine) {
	engine->renderer.render(engine->map.get(), engine->actors.get());
	gui->render();
}

void GameplayState::updateNextActor() {
    Actor* activeActor = actors->at(0);

    int actionTime = activeActor->update();
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
