#include "scheduler.hpp"
#include "actor.hpp"

void Scheduler::updateNextActor() {
	std::pair<int, Actor*>& activeActor = actorsQueue.at(0);
	int activeActorTUNA = activeActor.first;

	for(std::pair<int, Actor*>& a : actorsQueue) {
		a.first -= activeActorTUNA;
	}
	time += activeActorTUNA;

	int actionTime = activeActor.second->update();
	activeActor.first += actionTime;
	std::sort(actorsQueue.begin(), actorsQueue.end()); // TODO inefficient
	updateTime();
}

void Scheduler::insertActor(Actor* actor) {
	if(actor->ai) actorsQueue.push_back(std::pair<int, Actor*>(100, actor));
	std::sort(actorsQueue.begin(), actorsQueue.end());
}

void Scheduler::updateTime() { // TODO a bit nasty
	std::pair<int, Actor*>& activeActor = actorsQueue.at(0);
	int tuna = activeActor.first;

	for(std::pair<int, Actor*>& a : actorsQueue) {
		a.first -= tuna;
	}
	time += tuna;
}
