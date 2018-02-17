#include "renderer.hpp"

#include "actor.hpp"
#include "animation.hpp"
#include "colors.hpp"
#include "destructible.hpp"
#include "effect.hpp"
#include "event.hpp"
#include "font.hpp"
#include "gameplay_state.hpp"
#include "io.hpp"
#include "status_effect.hpp"
#include "map.hpp"
#include "point.hpp"
#include "world.hpp"

Renderer::Renderer(int screenWidth, int screenHeight): screenWidth(screenWidth), screenHeight(screenHeight), console(ConsoleType::SQUARE) {
	console.x = 0;
	console.y = 0;
	console.width = constants::SCREEN_WIDTH;
	console.height = constants::SCREEN_HEIGHT;
}

void Renderer::render(const World* const world, sf::RenderWindow* window) {
	//window.clear(sf::Color::Black);
	console.clear();

	const Map* const map = &world->map;
	const std::vector<std::unique_ptr<Actor>>& actors = world->actors;
	renderMap(world, window);
	renderActors(world, window);

	console.draw();
	//window.display();
}

void Renderer::renderMap(const World* const world, sf::RenderWindow* window) {
	const Map* const map = &world->map;
	int cameraX = world->getPlayer()->x - (screenWidth/2);
	int cameraY = world->getPlayer()->y - (screenHeight/2);
	int mapWidth = map->width;
	int mapHeight = map->height;

	int mouseXpx = sf::Mouse::getPosition(*window).x;
	int mouseYpx = sf::Mouse::getPosition(*window).y;
	int mouseXcells = mouseXpx / constants::SQUARE_CELL_WIDTH;
	int mouseYcells = mouseYpx / constants::SQUARE_CELL_HEIGHT;

	for(int x = 0; x < screenWidth; ++x) {
		for(int y = 0; y < screenHeight; ++y) {
			int worldX = x + cameraX;
			int worldY = y + cameraY;

			if(worldX < 0 || worldX >= mapWidth || worldY < 0 || worldY >= mapHeight) {
				console.setBackground(Point(x, y), colors::black);
			}
			else if(!map->isExplored(worldX, worldY)) {
				console.setBackground(Point(x, y), colors::black);
			}
			else if(map->isInFov(worldX, worldY)) {
				console.setBackground(Point(x, y), map->isWall(worldX, worldY) ? colors::lightWall : colors::lightGround);
			}
			else if(map->isExplored(worldX, worldY)) {
				console.setBackground(Point(x, y), map->isWall(worldX, worldY) ? colors::darkWall : colors::darkGround);
			}
			else {
				console.setBackground(Point(x, y), colors::black);
			}
		}
	}
	if(mouseXcells >= 0 && mouseXcells < console.width && mouseYcells >= 0 && mouseYcells < console.height) {
		console.highlight(Point(mouseXcells, mouseYcells));
	}

	renderAnimations(world, window);
}

void Renderer::renderAnimations(const World* const world, sf::RenderWindow* window) {
	const Map* const map = &world->map;
	int cameraX = world->getPlayer()->x - (screenWidth/2);
	int cameraY = world->getPlayer()->y - (screenHeight/2);
	int mapWidth = map->width;
	int mapHeight = map->height;

	int mouseXpx = sf::Mouse::getPosition(*window).x;
	int mouseYpx = sf::Mouse::getPosition(*window).y;
	int mouseXcells = mouseXpx / constants::SQUARE_CELL_WIDTH;
	int mouseYcells = mouseYpx / constants::SQUARE_CELL_HEIGHT;

	// build buffer for cellular automaton
	std::vector<int> previous(mapWidth*mapHeight);
	for(int x = 0; x < mapWidth; ++x) {
		for(int y = 0; y < mapHeight; ++y) {
			previous.at(x + mapWidth*y) = 64;
			if(map->tiles(x, y).terrain == Terrain::WATER && map->tiles(x, y).walkable) {
				previous.at(x + mapWidth*y) = map->tiles(x, y).animation->colors[0].b;
				if(previous.at(x + mapWidth*y) > 0) previous.at(x + mapWidth*y) = previous.at(x + mapWidth*y) - 1;
			}
		}
	}

	for(int x = 0; x < screenWidth; ++x) {
		for(int y = 0; y < screenHeight; ++y) {
			int worldX = x + cameraX;
			int worldY = y + cameraY;
			if(map->tiles(worldX, worldY).terrain == Terrain::WATER) {
				if(map->tiles(worldX, worldY).walkable) {
					if(map->tiles(worldX, worldY).animation) {
						Animation& animation = const_cast<Animation&>(*map->tiles(worldX, worldY).animation);
						sf::Color& color = animation.colors[0];
						int blue = 0;
						for(int i = -1; i <= 1; ++i) {
							for(int j = -1; j <= 1; ++j) {
								if(i == 0 && j == 0) continue;
								if(i + worldX < 0 || i + worldX >= mapWidth || j + worldY < 0 || j + worldY >= mapHeight) continue;
								if(!(map->tiles(worldX+i, worldY+j).terrain == Terrain::WATER) || !map->tiles(worldX+i, (worldY+j)).walkable) continue;
								else {
									int neighbor = previous[(worldX+i) + mapWidth*(worldY+j)];
									if(neighbor > color.b) {
										color.b = neighbor - 4;
									} else {
										//--color.b;
									}
									if(color.b > 64) --color.b;
									if(color.b > 64) --color.b;
									if(color.b > 64) --color.b;
									if(color.b > 64) --color.b;
								}
							}
						}
						sf::Color col;
						if(map->tiles(worldX, worldY).inFov) {
							col = color;
						} else if(map->isExplored(worldX, worldY)) {
							col = colors::darkestBlue;
						} else col = colors::black;

						console.setBackground(Point(x, y), col);
					}
					//rectangle.setFillColor(colors::lightBlue);
				} else {
					if(map->isExplored(worldX, worldY) && map->tiles(worldX, worldY).terrain == Terrain::WATER) { // water, not walkable
						console.setBackground(Point(x, y), colors::darkerBlue);
					}
				}
			}
		}
	}
}

void Renderer::notify(Event& event, World* world) {
	if(auto e = dynamic_cast<MoveEvent*>(&event)) {
		int x = e->x;
		int y = e->y;
		int mapWidth = world->width;
		if(world->map.tiles(x, y).animation) {
			world->map.tiles(x, y).animation->colors.at(0).b = 255;
		}
	}
}

void Renderer::renderActors(const World* const world, sf::RenderWindow* window) {
	const std::vector<std::unique_ptr<Actor>>& actors = world->actors;
	const Map* const map = &world->map;
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
	console.drawGlyph(screenPosition, (char)actor->ch, actor->col);
}

Point Renderer::getWorldCoordsFromScreenCoords(const Point& point) const {
	int cameraX = state->world->getPlayer()->x - (screenWidth/2); // TODO don't do this
	int cameraY = state->world->getPlayer()->y - (screenHeight/2); // TODO don't do this

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
	int cameraX = state->world->getPlayer()->x - (screenWidth/2); // upper left corner of camera
	int cameraY = state->world->getPlayer()->y - (screenHeight/2); // TODO don't do this

	int screenX = point.x - cameraX;
	int screenY = point.y - cameraY;

	return Point(screenX, screenY);
}
