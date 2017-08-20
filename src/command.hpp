#ifndef COMMAND_HPP
#define COMMAND_HPP
class Engine;
class Command {
public:
	virtual ~Command() {}
	virtual void execute() = 0;
};

class DummyCommand : public Command {
public:
	DummyCommand(Engine* engine) : engine(engine) {;}
	~DummyCommand() {}
	void execute();
private:
	Engine* engine;
};
#endif /* COMMAND_HPP */
