#ifndef IO_HPP
#define IO_HPP
#include <SFML/Window.hpp>

class Actor;
namespace io {
extern sf::RenderWindow window;
bool pickTile(Actor* actor, int* x, int* y, float maxRange = 0.0f);
} // namespace io
#endif /* IO_HPP */
