#ifndef DUMMY_STATE_HPP
#define DUMMY_STATE_HPP
class DummyState : public State {
public:
	void init(Engine* engine) override {;}
	void cleanup() override {;}

	void handleEvents(Engine* engine) override {;}
	void update(Engine* engine) override {;}
	void render(Engine* engine) override {;}
};
#endif /* DUMMY_STATE_HPP */
