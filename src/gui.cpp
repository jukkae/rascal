#include "ai.hpp"
#include "colors.hpp"
#include "constants.hpp"
#include "destructible.hpp"
#include "event.hpp"
#include "font.hpp"
#include "gameplay_state.hpp"
#include "gui.hpp"
#include "io.hpp"
#include "map.hpp"
#include "messaging.hpp"
#include "world.hpp"
#include <cstdarg>
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
}

void Gui::clear() {
	log.clear();
}

void Gui::render(World* world, sf::RenderWindow* window) {

	renderMessageLog(window);
	renderBar(1, 1, BAR_WIDTH, "HP", world->getPlayer()->destructible->hp, world->getPlayer()->destructible->maxHp, lightRed, darkerRed, window);

	std::string dungeonLvlString = "Floor " + std::to_string(world->level);
	sf::Text dlvl(dungeonLvlString, font::mainFont, 16);
	dlvl.setPosition(3*constants::CELL_WIDTH, (3+constants::SCREEN_HEIGHT-constants::GUI_PANEL_HEIGHT)*constants::CELL_HEIGHT);
	dlvl.setFillColor(sf::Color::White);
	window->draw(dlvl);

	std::string timeString = "Time: " + std::to_string(world->time);
	sf::Text time(timeString, font::mainFont, 16);
	time.setPosition(3*constants::CELL_WIDTH, (4+constants::SCREEN_HEIGHT-constants::GUI_PANEL_HEIGHT)*constants::CELL_HEIGHT);
	time.setFillColor(sf::Color::White);
	window->draw(time);

	renderXpBar(world, window);
	renderMouseLook(world, window);

	renderStatusEffects(world, window);
	renderStats(world, window);
}

void Gui::renderXpBar(World* world, sf::RenderWindow* window) {
	PlayerAi* ai = (PlayerAi*)world->getPlayer()->ai.get();
	char xpTxt[128];
	sprintf(xpTxt, "XP(%d)", ai->xpLevel);
	renderBar(1, 5, BAR_WIDTH, xpTxt, ai->experience, ai->getNextLevelXp(), lightViolet, darkerViolet, window);
}

void Gui::renderStats(World* world, sf::RenderWindow* window) {
	Actor* player = world->getPlayer(); // TODO instead pass player - rather, the actor - as a parameter to both Gui and Renderer

	int ac = player->destructible->armorClass;
	std::string acString = "AC: " + std::to_string(ac);
	sf::Text acText(acString, font::mainFont, 16);
	acText.setFillColor(colors::lightBlue);
	acText.setPosition((constants::SCREEN_WIDTH-40)*constants::CELL_WIDTH, (1+constants::SCREEN_HEIGHT-constants::GUI_PANEL_HEIGHT)*constants::CELL_HEIGHT);
	window->draw(acText);

	std::string weaponString;
	if(!player->wornWeapon) weaponString = "Wielding: nothing";
	else { // FIXME i know it's bad
		weaponString = "Wielding: " + player->wornWeapon->name;
		if(player->wornWeapon->rangedAttacker) weaponString.append(" (").append(std::to_string(player->wornWeapon->rangedAttacker->rounds)).append("/").append(std::to_string(player->wornWeapon->rangedAttacker->capacity)).append(")");
	}
	sf::Text weaponText(weaponString, font::mainFont, 16);
	weaponText.setFillColor(colors::lightBlue);
	weaponText.setPosition((constants::SCREEN_WIDTH-40)*constants::CELL_WIDTH, (2+constants::SCREEN_HEIGHT-constants::GUI_PANEL_HEIGHT)*constants::CELL_HEIGHT);
	window->draw(weaponText);

	int n, d, b;
	if(!player->wornWeapon) {
		n = player->attacker->numberOfDice;
		d = player->attacker->dice;
		b = player->attacker->bonus;
	}
	else if(player->wornWeapon->attacker) {
		n = player->wornWeapon->attacker->numberOfDice;
		d = player->wornWeapon->attacker->dice;
		b = player->wornWeapon->attacker->bonus;
	}
	else {
		n = player->wornWeapon->rangedAttacker->numberOfDice;
		d = player->wornWeapon->rangedAttacker->dice;
		b = player->wornWeapon->rangedAttacker->bonus;
	}
	std::string atkString = "dm: " + std::to_string(n) + "d" + std::to_string(d) + "+" + std::to_string(b);
	sf::Text atkText(atkString, font::mainFont, 16);
	atkText.setFillColor(colors::lightBlue);
	atkText.setPosition((constants::SCREEN_WIDTH-40)*constants::CELL_WIDTH, (3+constants::SCREEN_HEIGHT-constants::GUI_PANEL_HEIGHT)*constants::CELL_HEIGHT);
	window->draw(atkText);
}

void Gui::renderStatusEffects(World* world, sf::RenderWindow* window) {
	Actor* player = world->getPlayer();

	if(player->getStatusEffects().size() > 0) {
		sf::Text effectsHeading("status effects:", font::mainFont, 16);
		effectsHeading.setFillColor(colors::lightBlue);
		effectsHeading.setPosition((constants::SCREEN_WIDTH-60)*constants::CELL_WIDTH, (1 + constants::SCREEN_HEIGHT-constants::GUI_PANEL_HEIGHT)*constants::CELL_HEIGHT);
		window->draw(effectsHeading);
		int i = 1;
		for (auto& e : player->getStatusEffects()) {
			std::string effectStr = e->name;
			sf::Text effectText(effectStr, font::mainFont, 16);
			effectText.setFillColor(colors::lightBlue);
			effectText.setPosition((constants::SCREEN_WIDTH-60)*constants::CELL_WIDTH, (1 + i + constants::SCREEN_HEIGHT-constants::GUI_PANEL_HEIGHT)*constants::CELL_HEIGHT);
			window->draw(effectText);
			++i;
		}
	}
}

void Gui::renderMessageLog(sf::RenderWindow* window) {
	int y = 1;
	float colCoef = 0.4f;
	for(Message msg : log) {
		sf::Text text(msg.text, font::mainFont, 16);
		text.setFillColor(colors::multiply(msg.col, colCoef));
		text.setPosition(MSG_X * constants::CELL_WIDTH, (y + constants::SCREEN_HEIGHT - constants::GUI_PANEL_HEIGHT) * constants::CELL_HEIGHT); // correct position
		window->draw(text);
		++y;
		if(colCoef < 1.0f) {
			colCoef += 0.3f;
		}
	}
}

void Gui::renderBar(int x, int y, int width, std::string name, float value, float maxValue, const sf::Color barColor, const sf::Color backColor, sf::RenderWindow* window) {
	sf::RectangleShape bg(sf::Vector2f(constants::CELL_WIDTH * width, constants::CELL_HEIGHT));
	bg.setFillColor(backColor);
	bg.setPosition(x * constants::CELL_WIDTH, (y + constants::SCREEN_HEIGHT - constants::GUI_PANEL_HEIGHT) * constants::CELL_HEIGHT);
	window->draw(bg);

	sf::RectangleShape rect(sf::Vector2f(constants::CELL_WIDTH * width, constants::CELL_HEIGHT));

	int barWidth = (int) (value / maxValue * width);
	if ( barWidth > 0 ) {
		rect.setFillColor(barColor);
		rect.setSize(sf::Vector2f(constants::CELL_WIDTH * barWidth, constants::CELL_HEIGHT));
		rect.setPosition(x * constants::CELL_WIDTH, (y + constants::SCREEN_HEIGHT - constants::GUI_PANEL_HEIGHT) * constants::CELL_HEIGHT);
		window->draw(rect);
	}
	sf::Text text;
	text.setFont(font::mainFont);
	std::string string = name + " : " + std::to_string((int)value) + "/" + std::to_string((int)maxValue);
	text.setString(string);
	text.setCharacterSize(16);
	text.setFillColor(sf::Color::White);
	text.setPosition(x*constants::CELL_WIDTH, (y + constants::SCREEN_HEIGHT - constants::GUI_PANEL_HEIGHT) * constants::CELL_HEIGHT);
	window->draw(text);
}

void Gui::renderMouseLook(World* world, sf::RenderWindow* window) {
	int xPix = sf::Mouse::getPosition(*window).x;
	int yPix = sf::Mouse::getPosition(*window).y;
	int xCells = xPix / constants::SQUARE_CELL_WIDTH;
	int yCells = yPix / constants::SQUARE_CELL_HEIGHT;
	Point screenCells(xCells, yCells);

	Point location = state->getWorldCoordsFromScreenCoords(screenCells);
	io::mousePosition = location; // TODO this is nasty to do here, but it's still cleaner than it was before
	int x = location.x;
	int y = location.y;

	if(!world->isInFov(x, y)) {
		return;
	}
	std::string buf = "";
	bool first = true;
	std::vector<std::unique_ptr<Actor>>& actors = world->actors;
	for(auto& actor : actors) {
		if(actor->x == x && actor->y == y) {
			if(!first) {
				buf += ", ";
			} else { first = false; }
			buf += actor->name;
			if(actor->destructible && !actor->destructible->isDead()) {
				if(actor->destructible->hp < (actor->destructible->maxHp / 2)) {
					buf += " (almost dead)";
				}
			}
		}
	}
	sf::Text text;
	text.setFont(font::mainFont);
	std::string string = buf;
	text.setString(string);
	text.setCharacterSize(16);
	text.setFillColor(lightGrey);
	text.setPosition(1 * constants::CELL_WIDTH, (0 + constants::SCREEN_HEIGHT - constants::GUI_PANEL_HEIGHT) * constants::CELL_HEIGHT);
	window->draw(text);
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

void Gui::message(Message m) {
	std::istringstream iss (m.text); // TODO fix formatting and line breaks
	std::string line;
	while (std::getline(iss, line, '\n')) {
		// make room for the message
		if(log.size() == MSG_HEIGHT) {
			log.erase(log.begin());
		}
		Message msg(line, m.col);
		log.push_back(msg);
	}
}

void Gui::notify(Event& event) {
	if(isInteresting(event)) {
		Message m = messaging::createMessageFromEvent(event);
		message(m);
	}
}

bool Gui::isInteresting(Event& event) {
	if(auto e = dynamic_cast<ActionFailureEvent*>(&event)) {
		if(e->actor->isPlayer()) return true;
	}
	if(auto e = dynamic_cast<ItemFoundEvent*>(&event)) {
		if(e->finder->isPlayer()) return true;
	}
	if(auto e = dynamic_cast<MeleeHitEvent*>(&event)) {
		return true;
	}
	if(auto e = dynamic_cast<RangedHitEvent*>(&event)) {
		return true;
	}
	return false;
}
