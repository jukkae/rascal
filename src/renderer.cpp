#include "engine.hpp"
#include "renderer.hpp"
#include "map.hpp"

void Renderer::renderMap(const Map* const map) const {
	static const TCODColor darkWall   (0, 0, 100);
	static const TCODColor darkGround (50, 50, 150);
	static const TCODColor lightWall  (130, 110, 50);
	static const TCODColor lightGround(200, 180, 50);
	int cameraX = engine.player->x - (screenWidth/2); // upper left corner of camera
	int cameraY = engine.player->y - (screenHeight/2);

	for(int x = 0; x < map->width; x++) { // TODO no need to loop through all of map, just what's visible
		for(int y = 0; y < map->height; y++) {
			int screenX = x - cameraX;
			int screenY = y - cameraY;
			if(map->isInFov(x, y)) {
				TCODConsole::root->setCharBackground(screenX, screenY, map->isWall(x,y) ? lightWall : lightGround);
			}
			else if(map->isExplored(x, y)) {
				TCODConsole::root->setCharBackground(screenX, screenY, map->isWall(x,y) ? darkWall : darkGround);
			}
		}
	}
}

void Renderer::renderActor(const Actor* const actor) const {
	int cameraX = engine.player->x - (screenWidth/2); // upper left corner of camera
	int cameraY = engine.player->y - (screenHeight/2);
	int x = actor->x - cameraX;
	int y = actor->y - cameraY;

	TCODConsole::root->setChar(x, y, actor->ch);
	TCODConsole::root->setCharForeground(x, y, actor->col);
}
