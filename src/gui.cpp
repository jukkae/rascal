#include "ai.hpp"
#include "constants.hpp"
#include "destructible.hpp"
#include "engine.hpp"
#include "gui.hpp"
#include "map.hpp"
#include <stdarg.h>
#include <sstream>

static const int PANEL_HEIGHT = 7;
static const int BAR_WIDTH = 20;
static const int MSG_X = BAR_WIDTH + 2;
static const int MSG_HEIGHT = PANEL_HEIGHT - 1;

Gui::~Gui() {}

void Gui::clear() {
	log.clear();
}

void Gui::render() {
	con.setDefaultBackground(TCODColor::black);
	con.setDefaultForeground(TCODColor::white);
	con.clear();

	renderMessageLog();
	renderBar(1, 1, BAR_WIDTH, "HP", engine.getPlayer()->destructible->hp, engine.getPlayer()->destructible->maxHp, TCODColor::lightRed, TCODColor::darkerRed);
	con.print(3, 3, "Dungeon level %d", engine.getLevel());
	con.print(3, 4, "Time: %d", engine.getTime());
	renderXpBar();
	renderMouseLook(engine.getActors());

	TCODConsole::blit(&con, 0, 0, constants::SCREEN_WIDTH, PANEL_HEIGHT, TCODConsole::root, 0, constants::SCREEN_HEIGHT - PANEL_HEIGHT);
}

void Gui::renderXpBar() {
	PlayerAi* ai = (PlayerAi*)engine.getPlayer()->ai.get(); // Don't transfer ownership!
	char xpTxt[128];
	sprintf(xpTxt, "XP(%d)", ai->xpLevel);
	renderBar(1, 5, BAR_WIDTH, xpTxt, engine.getPlayer()->destructible->xp, ai->getNextLevelXp(), TCODColor::lightViolet,TCODColor::darkerViolet);
}

void Gui::renderMessageLog() {
	int y = 1;
	float colCoef = 0.4f;
	for(Message msg : log) {
		con.setDefaultForeground(msg.col * colCoef);
		con.print(MSG_X, y, msg.text.c_str());
		++y;
		if(colCoef < 1.0f) {
			colCoef += 0.3f;
		}
	}
}

void Gui::renderBar(int x, int y, int width, std::string name, float value, float maxValue, const TCODColor& barColor, const TCODColor& backColor) {
	con.setDefaultBackground(backColor);
	con.rect(x, y, width, 1, false, TCOD_BKGND_SET);
	int barWidth = (int) (value / maxValue * width);
	if ( barWidth > 0 ) {
		con.setDefaultBackground(barColor);
		con.rect(x, y, barWidth, 1, false, TCOD_BKGND_SET);
	}
	con.setDefaultForeground(TCODColor::white);
	con.printEx(x + width/2, y, TCOD_BKGND_NONE, TCOD_CENTER, "%s : %g/%g", name.c_str(), value, maxValue);
}

void Gui::renderMouseLook(std::vector<Actor*>* actors) {
	int mouseX = engine.mouse.cx;
	int mouseY = engine.mouse.cy;
	Point location = engine.renderer.getWorldCoordsFromScreenCoords(Point(mouseX, mouseY));
	int x = location.x;
	int y = location.y;

	if(!engine.map->isInFov(x, y)) {
		return;
	}
	std::string buf = "";
	bool first = true;
	for(Actor* actor : *actors) {
		if(actor->x == x && actor->y == y) {
			if(!first) {
				buf += ", ";
			} else { first = false; }
			buf += actor->name;
		}
	}
	con.setDefaultForeground(TCODColor::lightGrey);
	con.print(1, 0, buf.c_str());
}


Gui::Message::Message(std::string text, const TCODColor& col) :
col(col) {
	this->text = text;
}


Gui::Message::~Message() {
}

void Gui::message(const TCODColor& col, std::string text, ...) {
	va_list ap;
	std::string buf = "";
	char dest[1024*16]; // maybe something a bit more sane would be in order at some point
	va_start(ap, text);
	vsnprintf(dest, 1024*16, text.c_str(), ap);
	va_end(ap);
	std::istringstream iss (dest);
	std::string line;
	while (std::getline(iss, line, '\n')) {
		// make room for the message
		if(log.size() == MSG_HEIGHT) {
			log.erase(log.begin());
		}
		Message msg(line, col);
		log.push_back(msg);
	}
}

Menu::~Menu() {
	clear();
}

void Menu::clear() {
	items.clear();
}

void Menu::addItem(MenuItemCode code, const std::string label) {
	MenuItem item;
	item.code = code;
	item.label = label;
	items.push_back(item);
}

const int PAUSE_MENU_WIDTH = 30;
const int PAUSE_MENU_HEIGHT = 15;

Menu::MenuItemCode Menu::pick(DisplayMode mode) {
	int selectedItem = 0;
	int menuX, menuY;
	if(mode == DisplayMode::PAUSE) {
		menuX = constants::SCREEN_WIDTH / 2 - PAUSE_MENU_WIDTH / 2;
		menuY = constants::SCREEN_HEIGHT / 2 - PAUSE_MENU_HEIGHT / 2;
		TCODConsole::root->setDefaultForeground(TCODColor(200, 180, 50));
		TCODConsole::root->printFrame(menuX, menuY, PAUSE_MENU_WIDTH, PAUSE_MENU_HEIGHT, true, TCOD_BKGND_ALPHA(70), "menu");
		menuX += 2;
		menuY += 3;
	} else {
		static TCODImage img("menu_background1.png");
		img.blit2x(TCODConsole::root, 0, 0);
		menuX = 10;
		menuY = TCODConsole::root->getHeight() / 3;
	}
	while(!TCODConsole::isWindowClosed()) {
		int currentItem = 0;
		for (MenuItem item : items) {
			if (currentItem == selectedItem) {
				TCODConsole::root->setDefaultForeground(TCODColor::lighterOrange);
			} else {
			   TCODConsole::root->setDefaultForeground(TCODColor::lightGrey);
			}
			TCODConsole::root->print(menuX, menuY + currentItem * 3, item.label.c_str());
			++currentItem;
		}
		TCODConsole::flush();
		TCOD_key_t key;
		TCODSystem::waitForEvent(TCOD_EVENT_KEY_PRESS, &key, nullptr, true);
		switch (key.vk) {
			case TCODK_UP:
				--selectedItem;
				if (selectedItem < 0) {
					selectedItem = items.size() - 1;
				}
				break;
			case TCODK_DOWN: 
				selectedItem = (selectedItem + 1) % items.size(); 
				break;
			case TCODK_ENTER:
				return items.at(selectedItem).code;
			default:
				break;
		}
	}
	return MenuItemCode::NONE;
}
