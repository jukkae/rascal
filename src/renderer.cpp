#include "colors.hpp"
#include "font.hpp"
#include "gameplay_state.hpp"
#include "renderer.hpp"
#include "map.hpp"
#include "point.hpp"

Renderer::Renderer(int screenWidth, int screenHeight): screenWidth(screenWidth), screenHeight(screenHeight) {
}

void Renderer::render(const Map* const map, const std::vector<std::unique_ptr<Actor>>& actors, sf::RenderWindow* window) {
	//window.clear(sf::Color::Black);

	renderMap(map, window);
	renderActors(map, actors, window);

	//window.display();
}

void Renderer::renderMap(const Map* const map, sf::RenderWindow* window) {
	int cameraX = state->getPlayer()->x - (screenWidth/2);
	int cameraY = state->getPlayer()->y - (screenHeight/2);
	int mapWidth = map->width;
	int mapHeight = map->height;

	int mouseXpx = sf::Mouse::getPosition(*window).x;
	int mouseYpx = sf::Mouse::getPosition(*window).y;
	int mouseXcells = mouseXpx / constants::SQUARE_CELL_WIDTH;
	int mouseYcells = mouseYpx / constants::SQUARE_CELL_HEIGHT;

	for(int x = 0; x < screenWidth; ++x) {
		for(int y = 0; y < screenHeight; ++y) {
			if(x == mouseXcells && y == mouseYcells) {
				renderHighlight(map, window, Point(x, y));
			}
			else {
				int worldX = x + cameraX;
				int worldY = y + cameraY;

				sf::RectangleShape rectangle(sf::Vector2f(constants::SQUARE_CELL_WIDTH, constants::SQUARE_CELL_HEIGHT));
				rectangle.setPosition(x * constants::SQUARE_CELL_WIDTH, y * constants::SQUARE_CELL_HEIGHT);

				if(worldX < 0 || worldX >= mapWidth || worldY < 0 || worldY >= mapHeight) {
					rectangle.setFillColor(colors::black);
				}
				else if(map->tiles[worldX + mapWidth*worldY].inFov) {
					rectangle.setFillColor(map->isWall(worldX, worldY) ? colors::lightWall : colors::lightGround);
				}
				else if(map->isExplored(worldX, worldY)) {
					rectangle.setFillColor(map->isWall(worldX, worldY) ? colors::darkWall : colors::darkGround);
				}
				else {
					rectangle.setFillColor(colors::black);
				}

				window->draw(rectangle);
			}
		}
	}
}

void Renderer::renderHighlight(const Map* const map, sf::RenderWindow* window, const Point& point) {
	int x = point.x;
	int y = point.y;
	int cameraX = state->getPlayer()->x - (screenWidth/2);
	int cameraY = state->getPlayer()->y - (screenHeight/2);
	int mapWidth = map->width;
	int mapHeight = map->height;

	int worldX = x + cameraX;
	int worldY = y + cameraY;

	float coef = 1.2;

	sf::RectangleShape rectangle(sf::Vector2f(constants::SQUARE_CELL_WIDTH, constants::SQUARE_CELL_HEIGHT));
	rectangle.setPosition(x * constants::SQUARE_CELL_WIDTH, y * constants::SQUARE_CELL_HEIGHT);

	if(worldX < 0 || worldX >= mapWidth || worldY < 0 || worldY >= mapHeight) {
		rectangle.setFillColor(colors::darkestGrey);
	}
	else if(map->tiles[worldX + mapWidth*worldY].inFov) {
		rectangle.setFillColor(map->isWall(worldX, worldY) ? colors::multiply(colors::lightWall, coef) : colors::multiply(colors::lightGround, coef));
	}
	else if(map->isExplored(worldX, worldY)) {
		rectangle.setFillColor(map->isWall(worldX, worldY) ? colors::multiply(colors::darkWall, coef) : colors::multiply(colors::darkGround, coef));
	}
	else {
		rectangle.setFillColor(colors::black);
	}

	window->draw(rectangle);
}

void Renderer::renderActors(const Map* const map, const std::vector<std::unique_ptr<Actor>>& actors, sf::RenderWindow* window) {
	// Crude implementation of render layers
	Actor* player;
	std::vector<Actor*> corpses;
	std::vector<Actor*> misc; // Maybe useless?
	std::vector<Actor*> pickables;
	std::vector<Actor*> live;

	for(auto& actor : actors) {
		if(!actor->isPlayer()) {
			if(actor->destructible && actor->destructible->isDead()) corpses.push_back(actor.get());
			else if(actor->pickable) pickables.push_back(actor.get());
			else if(actor->ai) live.push_back(actor.get());
			else misc.push_back(actor.get());
		} else player = actor.get();
	}

	for(auto& actor : corpses) {
		if((!actor->fovOnly && map->isExplored(actor->x, actor->y)) || map->isInFov(actor->x, actor->y)) {
			renderActor(actor, window);
		}
	}
	for(auto& actor : misc) {
		if((!actor->fovOnly && map->isExplored(actor->x, actor->y)) || map->isInFov(actor->x, actor->y)) {
			renderActor(actor, window);
		}
	}
	for(auto& actor : pickables) {
		if((!actor->fovOnly && map->isExplored(actor->x, actor->y)) || map->isInFov(actor->x, actor->y)) {
			renderActor(actor, window);
		}
	}
	for(auto& actor : live) {
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


void Renderer::renderActor(const Actor* const actor, sf::RenderWindow* window) {
	Point worldPosition(actor->x, actor->y);
	Point screenPosition = getScreenCoordsFromWorldCoords(worldPosition);
	int x = screenPosition.x;
	int y = screenPosition.y;

	sf::Text t((char)actor->ch, font::squareFont, 16);
	t.setPosition(x*constants::SQUARE_CELL_WIDTH, y*constants::SQUARE_CELL_HEIGHT);
	t.setFillColor(actor->col);
	window->draw(t);

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
