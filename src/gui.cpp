#include "main.hpp"
#include <stdarg.h>

static const int PANEL_HEIGHT = 7;
static const int BAR_WIDTH = 20;
static const int MSG_X = BAR_WIDTH + 2;
static const int MSG_HEIGHT = PANEL_HEIGHT - 1;

Gui::Gui() {
	con = new TCODConsole(engine.screenWidth, PANEL_HEIGHT);
}

Gui::~Gui() {
	delete con;
}

void Gui::clear() {
	log.clear();
}

void Gui::render() {
	con->setDefaultBackground(TCODColor::black);
	con->clear();
	renderBar(1, 1, BAR_WIDTH, "HP", engine.player->destructible->hp, engine.player->destructible->maxHp, TCODColor::lightRed, TCODColor::darkerRed);
	int y = 1;
	float colCoef = 0.4f;
	for(auto i = log.begin(); i != log.end(); i++) {
		Message *msg = *i;
		con->setDefaultForeground(msg->col * colCoef);
		con->print(MSG_X, y, msg->text.c_str());
		y++;
		if(colCoef < 1.0f) {
			colCoef += 0.3f;
		}
	}
	renderMouseLook();
	con->setDefaultForeground(TCODColor::white);
	con->print(3, 3, "Dungeon level %d", engine.level);
	PlayerAi *ai=(PlayerAi *)engine.player->ai;

	// TODO extremely ugly
	char xpTxt[128];
	sprintf(xpTxt, "XP(%d)", ai->xpLevel);
	renderBar(1, 5, BAR_WIDTH, xpTxt, engine.player->destructible->xp, ai->getNextLevelXp(), TCODColor::lightViolet,TCODColor::darkerViolet);
			  			  
	TCODConsole::blit(con, 0, 0, engine.screenWidth, PANEL_HEIGHT, TCODConsole::root, 0, engine.screenHeight - PANEL_HEIGHT);
}

void Gui::renderBar(int x, int y, int width, std::string name, float value, float maxValue, const TCODColor& barColor, const TCODColor& backColor) {
	con->setDefaultBackground(backColor);
	con->rect(x, y, width, 1, false, TCOD_BKGND_SET);
	int barWidth = (int) (value / maxValue * width);
	if ( barWidth > 0 ) {
		con->setDefaultBackground(barColor);
		con->rect(x, y, barWidth, 1, false, TCOD_BKGND_SET);
	}
	con->setDefaultForeground(TCODColor::white);
	con->printEx(x + width/2, y, TCOD_BKGND_NONE, TCOD_CENTER, "%s : %g/%g", name.c_str(), value, maxValue);
}

void Gui::renderMouseLook() {
	if(!engine.map->isInFov(engine.mouse.cx, engine.mouse.cy)) {
		return;
	}
	std::string buf = "";
	bool first = true;
	for(auto i = engine.actors.begin(); i != engine.actors.end(); i++) {
		Actor* actor = *i;
		if(actor->x == engine.mouse.cx && actor->y == engine.mouse.cy) {
			if(!first) {
				buf += ", ";
			//strcat(buf, ", ");
			} else { first = false; }
			buf += actor->name;
			//strcat(buf, actor->name);
		}
	}
	con->setDefaultForeground(TCODColor::lightGrey);
	con->print(1, 0, buf.c_str());
}


Gui::Message::Message(std::string text, const TCODColor& col) :
col(col) {
	//this->text = "";
	this->text = text; //strcpy(this->text, text);
}

Gui::Message::Message() : // TODO dirty hack
col(TCODColor::white) {
	this->text = "";
}

Gui::Message::~Message() {
	//delete [] text;
}

void Gui::message(const TCODColor& col, std::string text, ...) {
	// TODO clean all this up
	// build the text
	va_list ap;
	std::string buf = "";
	char dest[1024*16]; // TODO YEAH ITS CRAP BUT IT DO WORK EH
	va_start(ap, text);
	vsnprintf(dest, 1024*16, text.c_str(), ap);
	va_end(ap);
	std::istringstream iss (dest);
	std::string line;
	while (std::getline(iss, line, '\n')) { // TODO make sure this logic works
		// make room for the new message
		if(log.size() == MSG_HEIGHT) {
			log.erase(log.begin());
		}
		// detect end of the line
		//lineEnd = strchr(lineBegin,'\n');
		//if(lineEnd) { *lineEnd = '\0'; }
		// add a new message to the log
		Message* msg = new Message(line, col);
		log.push_back(msg);
		// go to next line
		//lineBegin = lineEnd + 1;
	} //while (lineEnd);
}

Menu::~Menu() {
	clear();
}

void Menu::clear() {
	items.clear(); // TODO free memory
}

void Menu::addItem(MenuItemCode code, const std::string label) {
	MenuItem* item = new MenuItem();
	item->code = code;
	item->label = label;
	items.push_back(item);
}

Menu::MenuItemCode Menu::pick() {
	static TCODImage img("menu_background1.png");
	int selectedItem = 0;
	while(!TCODConsole::isWindowClosed()) {
		img.blit2x(TCODConsole::root, 0, 0);
		int currentItem = 0;
		for (auto it = items.begin(); it != items.end(); it++) {
			if (currentItem == selectedItem) {
				TCODConsole::root->setDefaultForeground(TCODColor::lighterOrange);
			} else {
			   TCODConsole::root->setDefaultForeground(TCODColor::lightGrey);
			}
			TCODConsole::root->print(10, 10+currentItem*3, (*it)->label.c_str());
			currentItem++;
		}
		TCODConsole::flush();
		TCOD_key_t key;
		TCODSystem::checkForEvent(TCOD_EVENT_KEY_PRESS, &key, NULL);
		switch (key.vk) {
			case TCODK_UP:
				selectedItem--;
				if (selectedItem < 0) {
					selectedItem = items.size() - 1;
				}
				break;
			case TCODK_DOWN: 
				selectedItem = (selectedItem + 1) % items.size(); 
				break;
			case TCODK_ENTER:
				return items.at(selectedItem)->code;
			default:
				break;
		}
	}
	return NONE;
}
