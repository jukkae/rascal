#ifndef RENDERER_HPP
#define RENDERER_HPP
#include <vector>
#include "point.hpp"
class GameplayState;
class Map;
class Renderer {
public:
	Renderer() : screenWidth(80), screenHeight(50) {;} // TODO remove default constructor
	Renderer(int screenWidth, int screenHeight): screenWidth(screenWidth), screenHeight(screenHeight) {;}
	void render(const Map* const map, const std::vector<Actor*>* const actors) const; // TODO 2nd param s.b. const
	Point getWorldCoordsFromScreenCoords(const Point& point) const;
	void setState(GameplayState* s) { state = s; }

private:
	int screenWidth;
	int screenHeight;
	GameplayState* state;

	void renderMap(const Map* const map) const;
	void renderActor(const Actor* const actor) const;

	Point getScreenCoordsFromWorldCoords(const Point& point) const;
	Point getWorldCoordsFromScreenCoords(const Point& point, const Point& player) const;
};
#endif /* RENDERER_HPP */
