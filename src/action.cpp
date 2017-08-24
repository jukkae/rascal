#include "action.hpp"
#include "actor.hpp"

void MoveAction::execute() {
	switch(direction) {
		case Direction::N:  actor->y -= 1;                break;
		case Direction::NE: actor->y -= 1; actor->x += 1; break;
		case Direction::E:                 actor->x += 1; break;
		case Direction::SE: actor->y += 1; actor->x += 1; break;
		case Direction::S:  actor->y += 1;                break;
		case Direction::SW: actor->y += 1; actor->x -= 1; break;
		case Direction::W:                 actor->x -= 1; break;
		case Direction::NW: actor->y -= 1; actor->x -= 1; break;
		case Direction::NONE: break;
		default: break;
	}
}
