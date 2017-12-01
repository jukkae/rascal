#ifndef IO_HPP
#define IO_HPP
#include <SFML/Window.hpp>
#include "point.hpp"

class Actor;
class GameplayState;
class Renderer;

namespace io {
extern sf::RenderWindow window;
extern Point mousePosition; // TODO this is not always updated correctly - seems that second click works
bool pickTile(GameplayState* state, Renderer* renderer, Actor* actor, int* x, int* y, float maxRange = 0.0f);
bool waitForMouseClick(GameplayState* state);
} // namespace io
#endif /* IO_HPP */
