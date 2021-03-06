#ifndef IO_HPP
#define IO_HPP
#include <SFML/Window.hpp>
#include "point.hpp"

class Actor;
class GameplayState;
class Renderer;

namespace io {

extern sf::RenderWindow window;
// TODO idea: instead of mousePosition, change to target that could be moved by, say, shift+arrows or tab
extern Point mousePosition; // note: already in WORLD coordinates!
extern Point mousePositionInScreenCoords;
extern Engine* engine;

bool waitForMouseClick(GameplayState* state);
bool fileExists(std::string name);
bool removeFile(std::string name);

void text(std::string text, int x, int y, sf::Color color = sf::Color::White);
void squareText(std::string text, int x, int y, sf::Color color = sf::Color::White);

} // namespace io
#endif /* IO_HPP */
