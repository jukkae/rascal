#include "container.hpp"
#include "actor.hpp"

Container::Container(int size) : size(size) {;}

Container::~Container() {
	inventory.clear();
}

bool Container::add(std::unique_ptr<Actor> actor) {
	if(size > 0 && inventory.size() >= size) { return false; }
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
