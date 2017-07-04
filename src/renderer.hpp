#ifndef RENDERER_HPP
#define RENDERER_HPP
#include <vector>
class Map;
class Renderer {
public:
	Renderer() : screenWidth(80), screenHeight(50) {;}
	Renderer(int screenWidth, int screenHeight): screenWidth(screenWidth), screenHeight(screenHeight) {;}
	void render(const Map* const map, const std::vector<Actor*> actors) const;

private:
	int screenWidth;
	int screenHeight;

	void renderMap(const Map* const map) const;
	void renderActor(const Actor* const actor) const;
};
#endif /* RENDERER_HPP */
