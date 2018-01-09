#ifndef RENDERER_HPP
#define RENDERER_HPP
#include <vector>
#include "constants.hpp"
#include "point.hpp"
#include <SFML/Graphics.hpp>

class Actor;
class GameplayState;
class World;
struct Point;
class Renderer {
public:
	Renderer() : Renderer(constants::SQUARE_SCREEN_WIDTH, constants::SCREEN_HEIGHT) {;}
	Renderer(int screenWidth, int screenHeight);
	void render(const World* const world, sf::RenderWindow* window);
	Point getWorldCoordsFromScreenCoords(const Point& point) const;
	void setState(GameplayState* s) { state = s; }

private:
	int elapsedTime = 0; // TODO crap just for testing
	bool goingUp = true; // TODO
	int screenWidth;
	int screenHeight;
	GameplayState* state;

	void renderMap(const World* const world, sf::RenderWindow* window);
	void renderActors(const World* const world, sf::RenderWindow* window);
	void renderActor(const Actor* const actor, sf::RenderWindow* window);
	void renderHighlight(const World* const world, sf::RenderWindow* window, const Point& point);

	Point getScreenCoordsFromWorldCoords(const Point& point) const;
	Point getWorldCoordsFromScreenCoords(const Point& point, const Point& player) const;
};
#endif /* RENDERER_HPP */
