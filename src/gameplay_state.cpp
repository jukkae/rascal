#include "gameplay_state.hpp"
#include "engine.hpp"

void GameplayState::init() {
	actors = engine.actors;

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
	engine->gui.render();
}
