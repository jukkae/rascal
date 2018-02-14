#ifndef RENDERER_HPP
#define RENDERER_HPP
#include <vector>
#include "console.hpp"
#include "constants.hpp"
#include <SFML/Graphics.hpp>

class Actor;
class GameplayState;
class World;
struct Point;
struct Event;
class Renderer {
public:
	Renderer() : Renderer(constants::SQUARE_SCREEN_WIDTH, constants::SCREEN_HEIGHT) {;}
	Renderer(int screenWidth, int screenHeight);
	void render(const World* const world, sf::RenderWindow* window);
	Point getWorldCoordsFromScreenCoords(const Point& point) const;
	void setState(GameplayState* s) { state = s; }
	void notify(Event& event, World* world);

private:
	int elapsedTime = 0; // TODO crap just for testing
	bool goingUp = true; // TODO
	int screenWidth;
	int screenHeight;
	GameplayState* state;
	Console console;

	void renderMap(const World* const world, sf::RenderWindow* window);
	void renderActors(const World* const world, sf::RenderWindow* window);
	void renderActor(const Actor* const actor, sf::RenderWindow* window);
	void renderHighlight(const World* const world, sf::RenderWindow* window, const Point& point);

	Point getScreenCoordsFromWorldCoords(const Point& point) const;
	Point getWorldCoordsFromScreenCoords(const Point& point, const Point& player) const;
};
#endif /* RENDERER_HPP */
