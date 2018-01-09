#include "container.hpp"
#include "actor.hpp"
#include "effect.hpp"
#include "pickable.hpp"
#include "status_effect.hpp"

Container::Container(int capacity) : capacity(capacity) {;}

bool Container::add(std::unique_ptr<Actor> actor) {
	if(capacity > 0 && inventory.size() >= capacity) { return false; }
	inventory.push_back(std::move(actor));
	return true;
}

void Container::remove(Actor* actor) {
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
