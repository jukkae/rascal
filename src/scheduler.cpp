#include "scheduler.hpp"
#include "actor.hpp"

void Scheduler::updateNextActor() {
	std::pair<float, Actor*>& activeActor = actorsQueue.at(0);
	float activeActorTUNA = activeActor.first;

	for(std::pair<float, Actor*>& a : actorsQueue) {
		a.first -= activeActorTUNA;
	}
	time += activeActorTUNA;

	float elapsedTime = activeActor.second->update();
	activeActor.first += elapsedTime;
	std::sort(actorsQueue.begin(), actorsQueue.end());
}

void Scheduler::insertActor(Actor* actor) {
	if(actor->ai) actorsQueue.push_back(std::pair<float, Actor*>(100, actor));
}
