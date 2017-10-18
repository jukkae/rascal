#include "engine_command.hpp"
#include "engine.hpp"

void DummyCommand::execute() {

}

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
