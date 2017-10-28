#ifndef RENDERER_HPP
#define RENDERER_HPP
#include <vector>
#include "constants.hpp"
#include "point.hpp"
#include <SFML/Graphics.hpp>
//#include "libtcod.hpp"
class GameplayState;
class Map;
class Renderer {
public:
	Renderer() : Renderer(constants::SCREEN_WIDTH, constants::SCREEN_HEIGHT) {;}
	Renderer(int screenWidth, int screenHeight);
	void render(const Map* const map, const std::vector<Actor*>* const actors, sf::RenderWindow& window);
	Point getWorldCoordsFromScreenCoords(const Point& point) const;
	void setState(GameplayState* s) { state = s; }

private:
	int screenWidth;
	int screenHeight;
	GameplayState* state;
	sf::Font font;
	//TCODConsole con;

	void renderMap(const Map* const map, sf::RenderWindow& window);
	void renderActors(const Map* const map, const std::vector<Actor*>* const actors, sf::RenderWindow& window);
	void renderActor(const Actor* const actor, sf::RenderWindow& window);

	Point getScreenCoordsFromWorldCoords(const Point& point) const;
	Point getWorldCoordsFromScreenCoords(const Point& point, const Point& player) const;
};
#endif /* RENDERER_HPP */
