#include "actor.hpp"
#include "constants.hpp"
#include "io.hpp"
#include "point.hpp"
#include <SFML/Graphics.hpp>

bool io::pickTile(Actor* actor, int* x, int* y, float maxRange) { // Should be moved over to renderer
	while(true) {
		sf::Event event;
		while(window.pollEvent(event)) { // Dummy polling to keep macOS happy
			render();
			for(int cx = 0; cx < constants::SQUARE_SCREEN_WIDTH; ++cx) {
				for(int cy = 0; cy < constants::SCREEN_HEIGHT; ++cy) {
					Point location = renderer.getWorldCoordsFromScreenCoords(Point(cx, cy));
					int realX = location.x;
					int realY = location.y;
					if(isInFov(realX, realY) && (maxRange == 0 || actor->getDistance(realX, realY) <= maxRange)) {
						// TODO Highlight background for tiles in range
					}
				}
			}
			int mouseXPix = sf::Mouse::getPosition(window).x;
			int mouseYPix = sf::Mouse::getPosition(window).y;
			int xCells = mouseXPix / constants::SQUARE_CELL_WIDTH;
			int yCells = mouseYPix / constants::SQUARE_CELL_HEIGHT;
			Point mouseLocationScreen(xCells, yCells);
			Point mouseLocation = renderer.getWorldCoordsFromScreenCoords(Point(mouseLocationScreen.x, mouseLocationScreen.y));
			int realX = mouseLocation.x;
			int realY = mouseLocation.y;
			if(isInFov(realX, realY) && (maxRange == 0 || actor->getDistance(realX, realY) <= maxRange)) {
				// Highlight background for tile under cursor: Done globally
				if(sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
					*x = realX;
					*y = realY;
					return true;
				}
			}
			if(sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
				return false;
			}
		}
	}
}
