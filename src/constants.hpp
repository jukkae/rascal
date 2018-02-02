#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP
#include <string>
namespace constants {
	static int const CELL_WIDTH = 8;
	static int const CELL_HEIGHT = 16;
	static int const SCREEN_WIDTH = 160;
	static int const SCREEN_HEIGHT = 40;
	static int const SQUARE_SCREEN_WIDTH = 80;
	static int const SQUARE_CELL_WIDTH = 16;
	static int const SQUARE_CELL_HEIGHT = 16;

	static int const GUI_PANEL_HEIGHT = 7;
	static int const INVENTORY_WIDTH = 50;
	static int const INVENTORY_HEIGHT = 28;
	static float const DEFAULT_TURN_LENGTH = 100.0f;
	static std::string const SAVE_FILE_NAME = "save.txt";
	static int const DEFAULT_MAP_WIDTH = 120;
	static int const DEFAULT_MAP_HEIGHT = 72;
	static int const DEFAULT_FOV_RADIUS = 25;
	static int const DEFAULT_ENEMY_FOV_RADIUS = 10;
}
#endif /* CONSTANTS_HPP */
