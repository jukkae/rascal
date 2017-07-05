#ifndef SCHEDULER_HPP
#define SCHEDULER_HPP
#include <vector>
class Actor;
class Scheduler {
public:
	float getCurrentTime() const { return time; }
	Actor* getNextActor() const { return actorsQueue.at(0).second; }
	void updateNextActor();
	void insertActor(Actor* actor);
private:
	std::vector<std::pair<float, Actor*>> actorsQueue;
	float time;
};
#endif /* SCHEDULER_HPP */
