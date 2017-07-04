#ifndef RENDERER_HPP
#define RENDERER_HPP
class Map;
class Renderer {
public:
	Renderer() : screenWidth(80), screenHeight(50) {;}
	Renderer(int screenWidth, int screenHeight): screenWidth(screenWidth), screenHeight(screenHeight) {;}
	void renderMap(const Map* const map) const; // should be const
	void renderActor(const Actor* const actor) const;

private:
	int screenWidth;
	int screenHeight;
};
#endif /* RENDERER_HPP */
