#include "engine_command.hpp"
#include "engine.hpp"
#include "state.hpp"

void NewGameCommand::execute() {
	engine->newGame();
	engine->popState();
}
void ContinueCommand::execute() {
	engine->popState();
}
void ExitCommand::execute() {
	engine->exit();
}

ChangeStateCommand::ChangeStateCommand(Engine* engine, std::unique_ptr<State> state) :
engine(engine), state(std::move(state)) {;}

void ChangeStateCommand::execute() {
	engine->popState();
	engine->pushState(std::move(state));
}
