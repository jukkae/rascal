#ifndef IO_HPP
#define IO_HPP
#include <SFML/Window.hpp>
#include "point.hpp"

class Actor;
class GameplayState;
class Renderer;

namespace io {
extern sf::RenderWindow window;
extern Point mousePosition;
bool waitForMouseClick(GameplayState* state);
} // namespace io
#endif /* IO_HPP */
