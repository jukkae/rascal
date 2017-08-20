#include "command.hpp"
#include "engine.hpp"

void DummyCommand::execute() {
	engine->popState();
}
