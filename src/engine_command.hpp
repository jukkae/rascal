#ifndef ENGINE_COMMAND_HPP
#define ENGINE_COMMAND_HPP
class Engine;
class EngineCommand {
public:
	virtual ~EngineCommand() {}
	virtual void execute() = 0;
};

class DummyCommand : public EngineCommand {
public:
	DummyCommand(Engine* engine) : engine(engine) {;}
	~DummyCommand() {}
	void execute();
private:
	Engine* engine;
};

class NewGameCommand : public EngineCommand {
public:
	NewGameCommand(Engine* engine) : engine(engine) {;}
	~NewGameCommand() {}
	void execute();
private:
	Engine* engine;
};

class ContinueCommand : public EngineCommand {
public:
	ContinueCommand(Engine* engine) : engine(engine) {;}
	~ContinueCommand() {}
	void execute();
private:
	Engine* engine;
};

class ExitCommand : public EngineCommand {
public:
	ExitCommand(Engine* engine) : engine(engine) {;}
	~ExitCommand() {}
	void execute();
private:
	Engine* engine;
};
#endif /* ENGINE_COMMAND_HPP */
