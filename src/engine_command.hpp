#ifndef ENGINE_COMMAND_HPP
#define ENGINE_COMMAND_HPP

#include "engine.hpp"
class State;
class EngineCommand {
public:
	virtual ~EngineCommand() {}
	virtual void execute() = 0;
};

class NewGameCommand : public EngineCommand {
public:
	NewGameCommand(Engine* engine) : engine(engine) {;}
	void execute();
private:
	Engine* engine;
};

class ContinueCommand : public EngineCommand {
public:
	ContinueCommand(Engine* engine) : engine(engine) {;}
	void execute();
private:
	Engine* engine;
};

class ExitCommand : public EngineCommand {
public:
	ExitCommand(Engine* engine) : engine(engine) {;}
	void execute();
private:
	Engine* engine;
};

class ChangeStateCommand : public EngineCommand {
public:
	ChangeStateCommand(Engine* engine, std::unique_ptr<State> state);
	void execute();
private:
	Engine* engine;
	std::unique_ptr<State> state;
};
#endif /* ENGINE_COMMAND_HPP */
