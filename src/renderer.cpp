#include "gameplay_state.hpp"
#include "renderer.hpp"
#include "map.hpp"
#include "point.hpp"

static const sf::Color darkWall   (0,   0,   100);
static const sf::Color darkGround (50,  50,  150);
static const sf::Color lightWall  (130, 110, 50 );
static const sf::Color lightGround(200, 180, 50 );
static const sf::Color black      (0,   0,   0  );

void Renderer::render(const Map* const map, const std::vector<Actor*>* const actors, sf::RenderWindow& window) {
	window.clear(sf::Color::Black);

	renderMap(map, window);
	renderActors(map, actors, window);

	window.display();
}

void Renderer::renderMap(const Map* const map, sf::RenderWindow& window) {
	int cameraX = state->getPlayer()->x - (screenWidth/2);
	int cameraY = state->getPlayer()->y - (screenHeight/2);
	int mapWidth = map->width;
	int mapHeight = map->height;

	for(int x = 0; x < screenWidth; ++x) {
		for(int y = 0; y < screenHeight; ++y) {
			int worldX = x + cameraX;
			int worldY = y + cameraY;

			sf::RectangleShape rectangle(sf::Vector2f(constants::CELL_WIDTH, constants::CELL_HEIGHT));
			rectangle.setFillColor(sf::Color::Red);
			rectangle.setPosition(x * constants::CELL_WIDTH, y * constants::CELL_HEIGHT);

			if(worldX < 0 || worldX >= mapWidth || worldY < 0 || worldY >= mapHeight) {
				rectangle.setFillColor(black);
			}
			else if(map->tiles[worldX + mapWidth*worldY].inFov) {
				rectangle.setFillColor(map->isWall(worldX, worldY) ? lightWall : lightGround);
			}
			else if(map->isExplored(worldX, worldY)) {
				rectangle.setFillColor(map->isWall(worldX, worldY) ? darkWall : darkGround);
			}
			else {
				rectangle.setFillColor(black);
			}

			window.draw(rectangle);
		}
	}
}

void Renderer::renderActors(const Map* const map, const std::vector<Actor*>* const actors, sf::RenderWindow& window) {
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
			renderActor(actor, window);
		}
	}
	for(Actor* actor : misc) {
		if((!actor->fovOnly && map->isExplored(actor->x, actor->y)) || map->isInFov(actor->x, actor->y)) {
			renderActor(actor, window);
		}
	}
	for(Actor* actor : pickables) {
		if((!actor->fovOnly && map->isExplored(actor->x, actor->y)) || map->isInFov(actor->x, actor->y)) {
			renderActor(actor, window);
		}
	}
	for(Actor* actor : live) {
		if((!actor->fovOnly && map->isExplored(actor->x, actor->y)) || map->isInFov(actor->x, actor->y)) {
			renderActor(actor, window);
		}
	}

	/*for (Actor* actor : *actors) {
		if(!actor->isPlayer() && ((!actor->fovOnly && map->isExplored(actor->x, actor->y)) || map->isInFov(actor->x, actor->y))) {
			renderActor(actor);
		}
		else if(actor->isPlayer()) player = actor;
	}*/
	renderActor(player, window);
}


void Renderer::renderActor(const Actor* const actor, sf::RenderWindow& window) {
	Point worldPosition(actor->x, actor->y);
	Point screenPosition = getScreenCoordsFromWorldCoords(worldPosition);
	int x = screenPosition.x;
	int y = screenPosition.y;

	//con.setChar(x, y, actor->ch);
	//con.setCharForeground(x, y, actor->col);
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
