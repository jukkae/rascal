#ifndef IO_HPP
#define IO_HPP
#include <SFML/Window.hpp>
#include "point.hpp"

class Actor;
class GameplayState;
class Renderer;

// TODO idea: instead of mousePosition, change to target that could be moved by, say, shift+arrows or tab
namespace io {
extern sf::RenderWindow window;
extern Point mousePosition; // note: already in WORLD coordinates!
bool waitForMouseClick(GameplayState* state);
bool fileExists(std::string name);
} // namespace io
#endif /* IO_HPP */
