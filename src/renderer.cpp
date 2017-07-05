#include "engine.hpp"
#include "renderer.hpp"
#include "map.hpp"
#include "point.hpp"

static const TCODColor darkWall   (0, 0, 100);
static const TCODColor darkGround (50, 50, 150);
static const TCODColor lightWall  (130, 110, 50);
static const TCODColor lightGround(200, 180, 50);

void Renderer::render(const Map* const map, const std::vector<Actor*> actors) const {
	TCODConsole::root->clear();
	renderMap(map);
	for (Actor* actor : actors) {
		if(actor != engine.player && ((!actor->fovOnly && map->isExplored(actor->x, actor->y)) || map->isInFov(actor->x, actor->y))) {
			renderActor(actor);
		}
	}
	renderActor(engine.player);
}

void Renderer::renderMap(const Map* const map) const {

	for(int x = 0; x < screenWidth; x++) { // TODO no need to loop through all of map, just what's visible
		for(int y = 0; y < screenHeight; y++) {
			Point screenPosition(x, y);
			Point worldPosition = getWorldCoordsFromScreenCoords(screenPosition);
			int worldX = worldPosition.x;
			int worldY = worldPosition.y;
			if(map->isInFov(worldX, worldY)) {
				TCODConsole::root->setCharBackground(x, y, map->isWall(worldX, worldY) ? lightWall : lightGround);
			}
			else if(map->isExplored(worldX, worldY)) {
				TCODConsole::root->setCharBackground(x, y, map->isWall(worldX, worldY) ? darkWall : darkGround);
			}
		}
	}
}

void Renderer::renderActor(const Actor* const actor) const {
	Point worldPosition(actor->x, actor->y);
	Point screenPosition = getScreenCoordsFromWorldCoords(worldPosition);
	int x = screenPosition.x;
	int y = screenPosition.y;

	TCODConsole::root->setChar(x, y, actor->ch);
	TCODConsole::root->setCharForeground(x, y, actor->col);
}

Point Renderer::getWorldCoordsFromScreenCoords(const Point point) const {
	int cameraX = engine.player->x - (screenWidth/2);
	int cameraY = engine.player->y - (screenHeight/2);

	int worldX = point.x + cameraX;
	int worldY = point.y + cameraY;

	return Point(worldX, worldY);
}

Point Renderer::getScreenCoordsFromWorldCoords(const Point point) const {
	int cameraX = engine.player->x - (screenWidth/2); // upper left corner of camera
	int cameraY = engine.player->y - (screenHeight/2);

	int screenX = point.x - cameraX;
	int screenY = point.y - cameraY;

	return Point(screenX, screenY);
}
