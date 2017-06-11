#include "main.hpp"

Container::Container(int size) : size(size) {;}
Container::Container() : size(0) {;} // TODO dirty hack

Container::~Container() {
	inventory.clear();
}

bool Container::add(Actor *actor) {
	if(size > 0 && inventory.size() >= size) { return false; }
	inventory.push_back(actor);
	return true;
}

void Container::remove(Actor *actor) {
	inventory.erase(std::remove(inventory.begin(), inventory.end(), actor), inventory.end());
}
