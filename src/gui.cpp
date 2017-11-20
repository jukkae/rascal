#include "ai.hpp"
#include "constants.hpp"
#include "destructible.hpp"
#include "gameplay_state.hpp"
#include "gui.hpp"
#include "map.hpp"
#include <stdarg.h>
#include <sstream>
#include <SFML/Window/Mouse.hpp>

static const int BAR_WIDTH = 20;
static const int MSG_X = BAR_WIDTH + 2;
static const int MSG_HEIGHT = constants::GUI_PANEL_HEIGHT - 1;

static const sf::Color lightRed(255, 115, 115);
static const sf::Color darkerRed(128, 0, 0);
static const sf::Color lightViolet(185, 115, 255);
static const sf::Color darkerViolet(64, 0, 128);
static const sf::Color lightGrey(159, 159, 159);

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
	renderBar(1, 1, BAR_WIDTH, "HP", state->getPlayer()->destructible->hp, state->getPlayer()->destructible->maxHp, lightRed, darkerRed, window);

	std::string dungeonLvlString = "Dungeon level " + std::to_string(state->getLevel());
	sf::Text dlvl(dungeonLvlString, font, 16);
	dlvl.setPosition(3*constants::CELL_WIDTH, (3+constants::SCREEN_HEIGHT-constants::GUI_PANEL_HEIGHT)*constants::CELL_HEIGHT);
	dlvl.setFillColor(sf::Color::White);
	window.draw(dlvl);

	std::string timeString = "Time: " + std::to_string(state->getTime());
	sf::Text time(timeString, font, 16);
	time.setPosition(3*constants::CELL_WIDTH, (4+constants::SCREEN_HEIGHT-constants::GUI_PANEL_HEIGHT)*constants::CELL_HEIGHT);
	time.setFillColor(sf::Color::White);
	window.draw(time);

	renderXpBar(window);
	renderMouseLook(state->getActors(), window);
}

void Gui::renderXpBar(sf::RenderWindow& window) {
	PlayerAi* ai = (PlayerAi*)state->getPlayer()->ai.get(); // Don't transfer ownership!
	char xpTxt[128];
	sprintf(xpTxt, "XP(%d)", ai->xpLevel);
	renderBar(1, 5, BAR_WIDTH, xpTxt, ai->experience, ai->getNextLevelXp(), lightViolet, darkerViolet, window);
}

void Gui::renderMessageLog(sf::RenderWindow& window) {
	int y = 1;
	float colCoef = 0.4f;
	for(Message msg : log) {
		sf::Text text(msg.text, font, 16);
		text.setFillColor(msg.col); //TODO  msg.col * colCoef
		text.setPosition(MSG_X * constants::CELL_WIDTH, (y + constants::SCREEN_HEIGHT - constants::GUI_PANEL_HEIGHT) * constants::CELL_HEIGHT); // correct position
		window.draw(text);
		++y;
		if(colCoef < 1.0f) {
			colCoef += 0.3f;
		}
	}
}

void Gui::renderBar(int x, int y, int width, std::string name, float value, float maxValue, const sf::Color barColor, const sf::Color backColor, sf::RenderWindow& window) {
	sf::RectangleShape bg(sf::Vector2f(constants::CELL_WIDTH * width, constants::CELL_HEIGHT));
	bg.setFillColor(backColor);
	bg.setPosition(x * constants::CELL_WIDTH, (y + constants::SCREEN_HEIGHT - constants::GUI_PANEL_HEIGHT) * constants::CELL_HEIGHT);
	window.draw(bg);

	sf::RectangleShape rect(sf::Vector2f(constants::CELL_WIDTH * width, constants::CELL_HEIGHT));

	int barWidth = (int) (value / maxValue * width);
	if ( barWidth > 0 ) {
		rect.setFillColor(barColor);
		rect.setSize(sf::Vector2f(constants::CELL_WIDTH * barWidth, constants::CELL_HEIGHT));
		rect.setPosition(x * constants::CELL_WIDTH, (y + constants::SCREEN_HEIGHT - constants::GUI_PANEL_HEIGHT) * constants::CELL_HEIGHT);
		window.draw(rect);
	}
	sf::Text text;
	text.setFont(font);
	std::string string = name + " : " + std::to_string((int)value) + "/" + std::to_string((int)maxValue);
	text.setString(string);
	text.setCharacterSize(16);
	text.setFillColor(sf::Color::White);
	text.setPosition(x*constants::CELL_WIDTH, (y + constants::SCREEN_HEIGHT - constants::GUI_PANEL_HEIGHT) * constants::CELL_HEIGHT);
	window.draw(text);
}

void Gui::renderMouseLook(std::vector<std::unique_ptr<Actor>>& actors, sf::RenderWindow& window) {
	int xPix = sf::Mouse::getPosition(window).x;
	int yPix = sf::Mouse::getPosition(window).y;
	int xCells = xPix / constants::CELL_WIDTH;
	int yCells = yPix / constants::CELL_HEIGHT;
	Point screenCells(xCells, yCells);

	Point location = state->getWorldCoordsFromScreenCoords(screenCells);
	int x = location.x;
	int y = location.y;

	if(!state->isInFov(x, y)) {
		return;
	}
	std::string buf = "";
	bool first = true;
	for(auto& actor : actors) {
		if(actor->x == x && actor->y == y) {
			if(!first) {
				buf += ", ";
			} else { first = false; }
			buf += actor->name;
		}
	}
	sf::Text text;
	text.setFont(font);
	std::string string = buf;
	text.setString(string);
	text.setCharacterSize(16);
	text.setFillColor(lightGrey);
	text.setPosition(1 * constants::CELL_WIDTH, (0 + constants::SCREEN_HEIGHT - constants::GUI_PANEL_HEIGHT) * constants::CELL_HEIGHT);
	window.draw(text);
}


Gui::Message::Message(std::string text, sf::Color col) :
col(col) {
	this->text = text;
}


Gui::Message::~Message() {
}

void Gui::message(sf::Color col, std::string text, va_list args) {
	std::string buf = "";
	char dest[1024*16]; // maybe something a bit more sane would be in order at some point
	vsnprintf(dest, 1024*16, text.c_str(), args); // FIXME

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

void Gui::message(sf::Color col, std::string text, ...) {
	va_list ap;
	std::string buf = "";
	char dest[1024*16]; // maybe something a bit more sane would be in order at some point
	va_start(ap, text);
	vsnprintf(dest, 1024*16, text.c_str(), ap); // FIXME
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
