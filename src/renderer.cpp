#include <chrono>

#include "engine.hpp"
#include "renderer.hpp"
#include "map.hpp"
#include "point.hpp"

using namespace std::chrono;

static const TCODColor darkWall   (0, 0, 100);
static const TCODColor darkGround (50, 50, 150);
static const TCODColor lightWall  (130, 110, 50);
static const TCODColor lightGround(200, 180, 50);

void Renderer::render(const Map* const map, const std::vector<Actor*>* const actors) const {
	TCODConsole::root->clear();
	renderMap(map);
	for (Actor* actor : *actors) {
		if(!actor->isPlayer() && ((!actor->fovOnly && map->isExplored(actor->x, actor->y)) || map->isInFov(actor->x, actor->y))) {
			renderActor(actor);
		}
	}
	renderActor(engine.getPlayer());
}

void Renderer::renderMap(const Map* const map) const {
	int playerX = engine.getPlayer()->x;
	int playerY = engine.getPlayer()->y;
	Point player(playerX, playerY);

	for(int x = 0; x < screenWidth; ++x) {
		for(int y = 0; y < screenHeight; ++y) {
			Point screenPosition(x, y);
			Point worldPosition = getWorldCoordsFromScreenCoords(screenPosition, player);
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

Point Renderer::getWorldCoordsFromScreenCoords(const Point& point) const {
	int cameraX = engine.getPlayer()->x - (screenWidth/2);
	int cameraY = engine.getPlayer()->y - (screenHeight/2);

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
	int cameraX = engine.getPlayer()->x - (screenWidth/2); // upper left corner of camera
	int cameraY = engine.getPlayer()->y - (screenHeight/2);

	int screenX = point.x - cameraX;
	int screenY = point.y - cameraY;

	return Point(screenX, screenY);
}
