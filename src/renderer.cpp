#include <chrono>

#include "gameplay_state.hpp"
#include "renderer.hpp"
#include "map.hpp"
#include "point.hpp"

using namespace std;
using namespace std::chrono;

static const TCODColor darkWall   (0, 0, 100);
static const TCODColor darkGround (50, 50, 150);
static const TCODColor lightWall  (130, 110, 50);
static const TCODColor lightGround(200, 180, 50);
static const TCODColor black      (0, 0, 0);

void Renderer::render(const Map* const map, const std::vector<Actor*>* const actors) const {
	//high_resolution_clock::time_point t1 = high_resolution_clock::now();
	TCODConsole::root->clear();
	//high_resolution_clock::time_point t2 = high_resolution_clock::now();
	renderMap(map);
	//high_resolution_clock::time_point t3 = high_resolution_clock::now();
	renderActors(map, actors);
	//high_resolution_clock::time_point t4 = high_resolution_clock::now();

	//auto duration1 = duration_cast<microseconds>( t2 - t1 ).count();
	//auto duration2 = duration_cast<microseconds>( t3 - t2 ).count();
	//auto duration3 = duration_cast<microseconds>( t4 - t3 ).count();
    //cout << "clear: " << duration1 << "\n";
    //cout << "rendermap: " << duration2 << "\n";
    //cout << "renderactors: " << duration3 << "\n";

}

void Renderer::renderMap(const Map* const map) const {
	//high_resolution_clock::time_point t1 = high_resolution_clock::now();

	int cameraX = state->getPlayer()->x - (screenWidth/2);
	int cameraY = state->getPlayer()->y - (screenHeight/2);
	int mapWidth = map->width;
	int mapHeight = map->height;
	//high_resolution_clock::time_point t2 = high_resolution_clock::now();

	// TODO this is slow, could be optimized - but how?
	for(int x = 0; x < screenWidth; ++x) {
		for(int y = 0; y < screenHeight; ++y) {
			int worldX = x + cameraX;
			int worldY = y + cameraY;
			if(worldX < 0 || worldX >= mapWidth || worldY < 0 || worldY >= mapHeight) {
				TCODConsole::root->setCharBackground(x, y, black);
			}
			else if(map->isInFov(worldX, worldY)) {
				TCODConsole::root->setCharBackground(x, y, map->isWall(worldX, worldY) ? lightWall : lightGround);
			}
			else if(map->isExplored(worldX, worldY)) {
				TCODConsole::root->setCharBackground(x, y, map->isWall(worldX, worldY) ? darkWall : darkGround);
			}
		}
	}
	//high_resolution_clock::time_point t3 = high_resolution_clock::now();
	//auto duration1 = duration_cast<nanoseconds>( t2 - t1 ).count();
	//auto duration2 = duration_cast<nanoseconds>( t3 - t2 ).count();
    //cout << "init: " << duration1 << "\n";
    //cout << "loops: " << duration2 << "\n";
}

void Renderer::renderActors(const Map* const map, const std::vector<Actor*>* const actors) const {
	// Crude implementation of render layers
	Actor* player;
	std::vector<Actor*> corpses;
	std::vector<Actor*> misc; // Maybe useless?
	std::vector<Actor*> pickables;
	std::vector<Actor*> live;

	for(Actor* actor : *actors) {
		if(!actor->isPlayer()) {
			if(actor->destructible && actor->destructible->isDead()) corpses.push_back(actor);
			else if(actor->pickable) pickables.push_back(actor);
			else if(actor->ai) live.push_back(actor);
			else misc.push_back(actor);
		} else player = actor;
	}

	for(Actor* actor : corpses) {
		if((!actor->fovOnly && map->isExplored(actor->x, actor->y)) || map->isInFov(actor->x, actor->y)) {
			renderActor(actor);
		}
	}
	for(Actor* actor : misc) {
		if((!actor->fovOnly && map->isExplored(actor->x, actor->y)) || map->isInFov(actor->x, actor->y)) {
			renderActor(actor);
		}
	}
	for(Actor* actor : pickables) {
		if((!actor->fovOnly && map->isExplored(actor->x, actor->y)) || map->isInFov(actor->x, actor->y)) {
			renderActor(actor);
		}
	}
	for(Actor* actor : live) {
		if((!actor->fovOnly && map->isExplored(actor->x, actor->y)) || map->isInFov(actor->x, actor->y)) {
			renderActor(actor);
		}
	}

	/*for (Actor* actor : *actors) {
		if(!actor->isPlayer() && ((!actor->fovOnly && map->isExplored(actor->x, actor->y)) || map->isInFov(actor->x, actor->y))) {
			renderActor(actor);
		}
		else if(actor->isPlayer()) player = actor;
	}*/
	renderActor(player);
}


void Renderer::renderActor(const Actor* const actor) const {
	Point worldPosition(actor->x, actor->y);
	Point screenPosition = getScreenCoordsFromWorldCoords(worldPosition);
	int x = screenPosition.x;
	int y = screenPosition.y;

	TCODConsole::root->setChar(x, y, actor->ch);
	TCODConsole::root->setCharForeground(x, y, actor->col);
}

Point Renderer::getWorldCoordsFromScreenCoords(const Point& point) const {
	int cameraX = state->getPlayer()->x - (screenWidth/2);
	int cameraY = state->getPlayer()->y - (screenHeight/2);

	int worldX = point.x + cameraX;
	int worldY = point.y + cameraY;

	return Point(worldX, worldY);
}

Point Renderer::getWorldCoordsFromScreenCoords(const Point& point, const Point& player) const {
	int cameraX = player.x - (screenWidth/2);
	int cameraY = player.y - (screenHeight/2);

	int worldX = point.x + cameraX;
	int worldY = point.y + cameraY;

	return Point(worldX, worldY);
}

Point Renderer::getScreenCoordsFromWorldCoords(const Point& point) const {
	int cameraX = state->getPlayer()->x - (screenWidth/2); // upper left corner of camera
	int cameraY = state->getPlayer()->y - (screenHeight/2);

	int screenX = point.x - cameraX;
	int screenY = point.y - cameraY;

	return Point(screenX, screenY);
}
