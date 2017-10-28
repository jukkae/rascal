#include "ai.hpp"
#include "constants.hpp"
#include "destructible.hpp"
#include "gameplay_state.hpp"
#include "gui.hpp"
#include "map.hpp"
#include <stdarg.h>
#include <sstream>

static const int BAR_WIDTH = 20;
static const int MSG_X = BAR_WIDTH + 2;
static const int MSG_HEIGHT = constants::GUI_PANEL_HEIGHT - 1;

Gui::Gui() {
	if(!font.loadFromFile("assets/FSEX300.ttf")) {
		std::cout << "error loading font\n";
	} else std::cout << "font loaded!\n";
	std::cout << font.getInfo().family << "\n";

}

Gui::~Gui() {}

void Gui::clear() {
	log.clear();
}

void Gui::render(sf::RenderWindow& window) {

	renderMessageLog(window);
	renderBar(1, 1, BAR_WIDTH, "HP", state->getPlayer()->destructible->hp, state->getPlayer()->destructible->maxHp, TCODColor::lightRed, TCODColor::darkerRed, window);
	//con.print(3, 3, "Dungeon level %d", state->getLevel());
	//con.print(3, 4, "Time: %d", state->getTime());
	renderXpBar(window);
	renderMouseLook(state->getActors());
}

void Gui::renderXpBar(sf::RenderWindow& window) {
	PlayerAi* ai = (PlayerAi*)state->getPlayer()->ai.get(); // Don't transfer ownership!
	char xpTxt[128];
	sprintf(xpTxt, "XP(%d)", ai->xpLevel);
	renderBar(1, 5, BAR_WIDTH, xpTxt, ai->experience, ai->getNextLevelXp(), TCODColor::lightViolet,TCODColor::darkerViolet, window);
}

void Gui::renderMessageLog(sf::RenderWindow& window) {
	int y = 1;
	float colCoef = 0.4f;
	for(Message msg : log) {
		sf::Text text(msg.text, font, 16);
		text.setColor(sf::Color::White); // msg.col * colCoef
		text.setPosition(MSG_X * constants::CELL_WIDTH, (y + constants::SCREEN_HEIGHT - constants::GUI_PANEL_HEIGHT) * constants::CELL_HEIGHT); // correct position
		window.draw(text);
		++y;
		if(colCoef < 1.0f) {
			colCoef += 0.3f;
		}
	}
}

void Gui::renderBar(int x, int y, int width, std::string name, float value, float maxValue, const TCODColor& barColor, const TCODColor& backColor, sf::RenderWindow& window) {
	//TODO draw back and front separately
	sf::RectangleShape rect(sf::Vector2f(constants::CELL_WIDTH * width, constants::CELL_HEIGHT));

	int barWidth = (int) (value / maxValue * width);
	if ( barWidth > 0 ) {
		rect.setFillColor(sf::Color::Red); // TODO color
		rect.setSize(sf::Vector2f(constants::CELL_WIDTH * barWidth, constants::CELL_HEIGHT));
		rect.setPosition(x * constants::CELL_WIDTH, (y + constants::SCREEN_HEIGHT - constants::GUI_PANEL_HEIGHT) * constants::CELL_HEIGHT);
		window.draw(rect);
		//con.setDefaultBackground(barColor);
		//con.rect(x, y, barWidth, 1, false, TCOD_BKGND_SET);
	}
	//con.setDefaultForeground(TCODColor::white);
	//con.printEx(x + width/2, y, TCOD_BKGND_NONE, TCOD_CENTER, "%s : %g/%g", name.c_str(), value, maxValue);
	sf::Text text;
	text.setFont(font);
	//std::string string = "%s : %g/%g", name.c_str(), value, maxValue;
	std::string string = name + " : " + std::to_string((int)value) + "/" + std::to_string((int)maxValue);
	text.setString(string);
	text.setCharacterSize(16);
	text.setColor(sf::Color::White);
	text.setPosition((x /*+ width/2*/)*constants::CELL_WIDTH, (y + constants::SCREEN_HEIGHT - constants::GUI_PANEL_HEIGHT) * constants::CELL_HEIGHT);
	window.draw(text);
}

void Gui::renderMouseLook(std::vector<Actor*>* actors) {
	Point mouse = state->getMouseLocation();
	Point location = state->getWorldCoordsFromScreenCoords(mouse);
	int x = location.x;
	int y = location.y;

	if(!state->isInFov(x, y)) {
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
	//con.setDefaultForeground(TCODColor::lightGrey);
	//con.print(1, 0, buf.c_str());
}


Gui::Message::Message(std::string text, const TCODColor& col) :
col(col) {
	this->text = text;
}


Gui::Message::~Message() {
}

void Gui::message(const TCODColor& col, std::string text, va_list args) {
	std::string buf = "";
	char dest[1024*16]; // maybe something a bit more sane would be in order at some point
	vsnprintf(dest, 1024*16, text.c_str(), args);

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
