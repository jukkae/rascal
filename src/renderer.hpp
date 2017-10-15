#ifndef RENDERER_HPP
#define RENDERER_HPP
#include <vector>
#include "point.hpp"
class GameplayState;
class Map;
class Renderer {
public:
	Renderer(int screenWidth, int screenHeight): screenWidth(screenWidth), screenHeight(screenHeight) {;}
	void render(const Map* const map, const std::vector<Actor*>* const actors) const;
	Point getWorldCoordsFromScreenCoords(const Point& point) const;
	void setState(GameplayState* s) { state = s; }

private:
	int screenWidth;
	int screenHeight;
	GameplayState* state;

	void renderMap(const Map* const map) const;
	void renderActors(const Map* const map, const std::vector<Actor*>* const actors) const;
	void renderActor(const Actor* const actor) const;

	Point getScreenCoordsFromWorldCoords(const Point& point) const;
	Point getWorldCoordsFromScreenCoords(const Point& point, const Point& player) const;
};
#endif /* RENDERER_HPP */
