#include "renderer.hpp"

#include "actor.hpp"
#include "ai.hpp" // Required for getting color from faction
#include "animation.hpp"
#include "colors.hpp"
#include "destructible.hpp"
#include "effect.hpp"
#include "event.hpp"
#include "faction.hpp" // Required for faction colors
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


	int mouseX = io::mousePositionInScreenCoords.x;
	int mouseY = io::mousePositionInScreenCoords.y;

	for(int x = 0; x < screenWidth; ++x) {
		for(int y = 0; y < screenHeight; ++y) {
			int worldX = x + cameraX;
			int worldY = y + cameraY;

			if(worldX < 0 || worldX >= mapWidth || worldY < 0 || worldY >= mapHeight) {
				console.setBackground(Point(x, y), colors::get("black"));
			}
			else if(!map->isExplored(worldX, worldY)) {
				console.setBackground(Point(x, y), colors::get("black"));
			}
			else if(map->isInFov(worldX, worldY)) {
				console.setBackground(Point(x, y), map->isWall(worldX, worldY) ? colors::get("lightWall") : colors::get("lightGround"));
			}
			else if(map->isExplored(worldX, worldY)) {
				console.setBackground(Point(x, y), map->isWall(worldX, worldY) ? colors::get("darkWall") : colors::get("darkGround"));
				if((map->tiles(worldX, worldY).terrain == Terrain::WATER)) { // blechh
					console.setBackground(Point(x, y), map->isWall(worldX, worldY) ? colors::get("darkerBlue") : sf::Color(0, 0, 32));
				}
			}
			else {
				console.setBackground(Point(x, y), colors::get("black"));
			}
		}
	}
	if(map->hasAnimations) renderAnimations(world, window);
	if(mouseX >= 0 && mouseX < console.width && mouseY >= 0 && mouseY < console.height) {
		console.highlight(Point(mouseX, mouseY));
	}
}

// FIXME shouldn't be called render animations, rather run automata or something
void Renderer::renderAnimations(const World* const world, sf::RenderWindow* window) {
	const Map* const map = &world->map;
	int cameraX = world->getPlayer()->x - (screenWidth/2);
	int cameraY = world->getPlayer()->y - (screenHeight/2);
	int mapWidth = map->width;
	int mapHeight = map->height;

	// build buffer for cellular automaton
	std::vector<int> previous(mapWidth*mapHeight);
	for(int x = 0; x < mapWidth; ++x) {
		for(int y = 0; y < mapHeight; ++y) {
			previous.at(x + mapWidth*y) = 64;
			if(map->tiles(x, y).animation) {
				previous.at(x + mapWidth*y) = map->tiles(x, y).animation->colors[0].b;
			}
		}
	}

	for(int x = 0; x < screenWidth; ++x) {
		for(int y = 0; y < screenHeight; ++y) {
			int worldX = x + cameraX;
			int worldY = y + cameraY;
			if(worldX < 0 || worldX >= mapWidth || worldY < 0 || worldY >= mapHeight) continue;
			if(map->tiles(worldX, worldY).animation) {
				Animation& animation = const_cast<Animation&>(*map->tiles(worldX, worldY).animation);
				sf::Color& color = animation.colors[0];
				for(int i = -1; i <= 1; ++i) {
					for(int j = -1; j <= 1; ++j) {
						if(i == 0 && j == 0) continue;
						if(i + worldX < 0 || i + worldX >= mapWidth || j + worldY < 0 || j + worldY >= mapHeight) continue;
						if(!(map->tiles(worldX+i, worldY+j).terrain == Terrain::WATER) || !map->tiles(worldX+i, (worldY+j)).walkable) continue;
						else {
							int neighbor = previous[(worldX+i) + mapWidth*(worldY+j)];
							if(neighbor > color.b) {
								color.b = neighbor - 4;
							}
							for(int c = 0; c < 4; ++c) if(color.b > 64) --color.b;
						}
					}
				}
				if(map->tiles(worldX, worldY).inFov) {
					console.setBackground(Point(x, y), color);
				}
			}
		}
	}
}

void Renderer::notify(Event& event, World* world) {
	if(auto e = dynamic_cast<MoveEvent*>(&event)) {
		int x = e->x;
		int y = e->y;
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
			renderActor(world, actor, window);
		}
	}
	for(auto& actor : misc) {
		if((!actor->fovOnly && map->isExplored(actor->x, actor->y)) || map->isInFov(actor->x, actor->y)) {
			renderActor(world, actor, window);
		}
	}
	for(auto& actor : pickables) {
		if((!actor->fovOnly && map->isExplored(actor->x, actor->y)) || map->isInFov(actor->x, actor->y)) {
			renderActor(world, actor, window);
		}
	}
	for(auto& actor : live) {
		if((!actor->fovOnly && map->isExplored(actor->x, actor->y)) || map->isInFov(actor->x, actor->y)) {
			renderActor(world, actor, window);
		}
	}

	/*for (Actor* actor : *actors) {
		if(!actor->isPlayer() && ((!actor->fovOnly && map->isExplored(actor->x, actor->y)) || map->isInFov(actor->x, actor->y))) {
			renderActor(actor);
		}
		else if(actor->isPlayer()) player = actor;
	}*/
	renderActor(world, player, window);
}


void Renderer::renderActor(const World* const world, const Actor* const actor, sf::RenderWindow* window) {
	Point worldPosition(actor->x, actor->y);
	Point screenPosition = getScreenCoordsFromWorldCoords(worldPosition);
	sf::Color color;
	if(actor->ai) color = actor->ai->faction.color;
	else color = actor->col;
	if(actor->ai) {
		// render FOV cone
		for(auto& p : actor->ai->fov) {
			sf::Color oldColor = console.getBackground(getScreenCoordsFromWorldCoords(p));
			sf::Color newColor = world->map.isWall(p.x, p.y) ? colors::get("lightWall") : colors::get("lightGround");
			console.setBackground(getScreenCoordsFromWorldCoords(p), colors::add(oldColor, colors::multiply(newColor, 0.1f)));
		}
	}
	console.drawGlyph(screenPosition, (char)actor->ch, color);
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
