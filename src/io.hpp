#ifndef IO_HPP
#define IO_HPP
#include <SFML/Window.hpp>

class Actor;
class GameplayState;
class Renderer;
namespace io {
extern sf::RenderWindow window;
bool pickTile(GameplayState* state, Renderer* renderer, Actor* actor, int* x, int* y, float maxRange = 0.0f);
} // namespace io
#endif /* IO_HPP */
