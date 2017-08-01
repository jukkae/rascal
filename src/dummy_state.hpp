#ifndef DUMMY_STATE_HPP
#define DUMMY_STATE_HPP
class DummyState : public State {
public:
	void init() override {;}
	void cleanup() override {;}

	void update(Engine* engine) override {std::cout << "Dummy update\n";}
	void render(Engine* engine) override {std::cout << "Dummy render\n";}
};
#endif /* DUMMY_STATE_HPP */
