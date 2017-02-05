#include "libtcod.hpp"
int main() {
	TCODConsole::initRoot(80,50,"libtcod C++ tutorial",false);
	while ( !TCODConsole::isWindowClosed() ) {
		TCODSystem::checkForEvent(TCOD_EVENT_KEY_PRESS,NULL,NULL);
		TCODConsole::root->clear();
		TCODConsole::root->putChar(40,25,'@');
		TCODConsole::flush();
	}
	return 0;
}
