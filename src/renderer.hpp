#ifndef RENDERER_HPP
#define RENDERER_HPP
#include <vector>
#include "point.hpp"
class Map;
class Renderer {
public:
	Renderer() : screenWidth(80), screenHeight(50) {;} // TODO remove default constructor
	Renderer(int screenWidth, int screenHeight): screenWidth(screenWidth), screenHeight(screenHeight) {;}
	void render(const Map* const map, const std::vector<Actor*>& actors) const;
	Point getWorldCoordsFromScreenCoords(const Point& point) const;

private:
	int screenWidth;
	int screenHeight;

	void renderMap(const Map* const map) const;
	void renderActor(const Actor* const actor) const;

	Point getScreenCoordsFromWorldCoords(const Point& point) const;
	Point getWorldCoordsFromScreenCoords(const Point& point, const Point& player) const;
};
#endif /* RENDERER_HPP */
