#include "scheduler.hpp"
#include "actor.hpp"

void Scheduler::updateNextActor() {
	std::pair<int, Actor*>& activeActor = actorsQueue.at(0);
	int activeActorTUNA = activeActor.first;

	for(std::pair<int, Actor*>& a : actorsQueue) {
		a.first -= activeActorTUNA;
	}
	time += activeActorTUNA;

	int elapsedTime = activeActor.second->update();
	activeActor.first += elapsedTime;
	std::sort(actorsQueue.begin(), actorsQueue.end()); // TODO inefficient
}

void Scheduler::insertActor(Actor* actor) {
	if(actor->ai) actorsQueue.push_back(std::pair<int, Actor*>(100, actor));
}
