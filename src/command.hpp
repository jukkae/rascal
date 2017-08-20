#ifndef COMMAND_HPP
#define COMMAND_HPP
class Command {
public:
	virtual ~Command() {}
	virtual void execute() = 0;
};

class DummyCommand : public Command {
public:
	~DummyCommand() {}
	void execute() { std::cout << "Hello from dummy command!\n"; }
};
#endif /* COMMAND_HPP */
