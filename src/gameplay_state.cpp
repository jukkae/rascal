#include "gameplay_state.hpp"
#include "engine.hpp"

void GameplayState::init() {

}

void GameplayState::cleanup() {

}

void GameplayState::update(Engine* engine) {

}

void GameplayState::render(Engine* engine) {
	engine->renderer.render(engine->map.get(), engine->actors);
	engine->gui.render();
}
