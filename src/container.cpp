#include "container.hpp"
#include "actor.hpp"

Container::Container(int capacity) : capacity(capacity) {;}

Container::~Container() {
	inventory.clear();
}

bool Container::add(std::unique_ptr<Actor> actor) {
	if(capacity > 0 && inventory.size() >= capacity) { return false; }
	inventory.push_back(std::move(actor));
	return true;
}

void Container::remove(Actor* actor) { // TODO is called already after the element is nulled
	inventory.erase(
		std::remove_if(
			inventory.begin(),
			inventory.end(),
			[&](auto const & p) { return p.get() == actor; }
		),
		inventory.end()
	);
}

int Container::getContentsWeight() {
	int w = 0;
	for(auto& i : inventory) w += i->pickable->weight;
	return w; 
}
