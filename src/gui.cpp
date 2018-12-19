#include "ai.hpp"
#include "attacker.hpp"
#include "body.hpp"
#include "colors.hpp"
#include "constants.hpp"
#include "container.hpp"
#include "destructible.hpp"
#include "effect.hpp"
#include "event.hpp"
#include "font.hpp"
#include "gameplay_state.hpp"
#include "gui.hpp"
#include "ignore.hpp"
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

Gui::Gui(): console() {
	console.x = 0;
	console.y = constants::SCREEN_HEIGHT - constants::GUI_PANEL_HEIGHT;
	console.width = constants::SCREEN_WIDTH;
	console.height = constants::GUI_PANEL_HEIGHT;
}

void Gui::clear() {
	log.clear();
}

void Gui::render(World* world, sf::RenderWindow* window) {
	console.clear();

	renderMessageLog(window);
	renderBar(1, 1, BAR_WIDTH, "HP", world->getPlayer()->destructible->hp, world->getPlayer()->destructible->maxHp, lightRed, darkerRed, window);

	std::string dungeonLvlString = "Floor " + std::to_string(world->level);
	console.drawText(Point(3, 3), dungeonLvlString, colors::get("white"));


	std::string timeString = "Time: " + std::to_string(world->time);
	console.drawText(Point(3, 4), timeString, colors::get("white"));

	renderXpBar(world, window);
	renderMouseLook(world, window);

	renderStatusEffects(world, window);
	renderStats(world, window);
	renderNav(world, window);
	console.draw();
}

void Gui::renderXpBar(World* world, sf::RenderWindow* window) {
	PlayerAi* ai = (PlayerAi*)world->getPlayer()->ai.get();
	char xpTxt[128];
	sprintf(xpTxt, "XP(%d)", ai->xpLevel);
	renderBar(1, 5, BAR_WIDTH, xpTxt, ai->experience, ai->getNextLevelXp(), lightViolet, darkerViolet, window);
}

void Gui::renderStats(World* world, sf::RenderWindow* window) {
	Actor* player = world->getPlayer(); // TODO instead pass player - rather, the actor - as a parameter to both Gui and Renderer

	int ac = player->getAC();
	std::string acString = "AC: " + std::to_string(ac);
	console.drawText(Point(120, 1), acString, colors::get("lightBlue"));

	std::string weaponString;
	if(!player->wornWeapon) weaponString = "Wielding: nothing";
	else { // FIXME i know it's bad
		weaponString = "Wielding: " + player->wornWeapon->name;
		if(player->wornWeapon->rangedAttacker) weaponString.append(" (").append(std::to_string(player->wornWeapon->rangedAttacker->rounds)).append("/").append(std::to_string(player->wornWeapon->rangedAttacker->capacity)).append(")");
	}
	console.drawText(Point(120, 2), weaponString, colors::get("lightBlue"));

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
	console.drawText(Point(120, 3), atkString, colors::get("lightBlue"));
}

void Gui::renderNav(World* world, sf::RenderWindow* window) {
	Actor* player = world->getPlayer(); // TODO instead pass player - rather, the actor - as a parameter to both Gui and Renderer
	if(std::any_of(
		player->container->inventory.begin(),
		player->container->inventory.end(),
		[&] (const std::unique_ptr<Actor>& a) { return a->name == "navigation computer"; }
	)) {
		std::string location = std::to_string(player->x) + ", " + std::to_string(player->y);
		console.drawText(Point(100, 3), location, colors::get("lightBlue"));
		std::string rad = "rad: " + std::to_string(world->radiation);
		console.drawText(Point(100, 4), rad, colors::get("lightBlue"));

		// TODO doesn't belong here, temporary until info management is figured out
		std::string hungerString = "nutrition:" + std::to_string(player->body->nutrition);
		console.drawText(Point(120, 5), hungerString, colors::get("lightBlue"));

		std::string iodineString = "iodine:" + std::to_string(player->body->iodine);
		console.drawText(Point(120, 6), iodineString, colors::get("lightBlue"));
	}

}

void Gui::renderStatusEffects(World* world, sf::RenderWindow* window) {
	Actor* player = world->getPlayer();

	if(player->getStatusEffects().size() > 0) {
		console.drawText(Point(100, 1), "status effects:", colors::get("lightBlue"));
		int i = 1;
		for (auto& e : player->getStatusEffects()) {
			std::string effectStr = e->name;
			console.drawText(Point(100, 1 + i), effectStr, colors::get("lightBlue"));
			++i;
		}
	}
}

void Gui::renderMessageLog(sf::RenderWindow* window) {
	int y = 0;
	float colCoef = 0.4f;
	for(Message msg : log) {
		console.drawText(Point(MSG_X, y), msg.text, colors::multiply(msg.col, colCoef));
		++y;
		if(colCoef < 1.0f) {
			colCoef += 0.3f;
		}
	}
}

void Gui::renderBar(int x, int y, int width, std::string name, float value, float maxValue, const sf::Color barColor, const sf::Color backColor, sf::RenderWindow* window) {
	for(int i = 0; i < width; ++i) {
		console.setBackground(Point(x + i, y), backColor);
	}

	int barWidth = (int) (value / maxValue * width);
	if ( barWidth > 0 ) {
		for(int i = 0; i < barWidth; ++i) {
			console.setBackground(Point(x + i, y), barColor);
		}
	}
	std::string string = name + " : " + std::to_string((int)value) + "/" + std::to_string((int)maxValue);
	console.drawText(Point(x, y), string, colors::get("white"));
}

void Gui::renderMouseLook(World* world, sf::RenderWindow* window) {
	int xPix = sf::Mouse::getPosition(*window).x;
	int yPix = sf::Mouse::getPosition(*window).y;
	int windowWidth = window->getSize().x;
	int windowHeight = window->getSize().y;

	// Clamp within window
	if(xPix < 0) xPix = 0;
	if(xPix >= windowWidth) xPix = windowWidth - 1;
	if(yPix < 0) yPix = 0;
	if(yPix >= windowHeight) yPix = windowHeight - 1;

	int xCells = xPix / constants::SQUARE_CELL_WIDTH;
	int yCells = yPix / constants::SQUARE_CELL_HEIGHT;
	Point screenCells(xCells, yCells);

	Point location = state->getWorldCoordsFromScreenCoords(screenCells);
	io::mousePosition = location; // TODO this is nasty to do here, but it's still cleaner than it was before
	io::mousePositionInScreenCoords = screenCells; // TODO this is nasty to do here, but it's still cleaner than it was before
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
	std::string string = buf;
	console.drawText(Point(1, 0), string, colors::get("lightGrey"));
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
		ignore(e);
		/* if(e->actor->isPlayer()) */ return true;
	}
	if(auto e = dynamic_cast<ActionSuccessEvent*>(&event)) {
		ignore(e);
		/* if(e->actor->isPlayer()) */ return true;
	}
	if(auto e = dynamic_cast<ItemFoundEvent*>(&event)) {
		if(e->finder->isPlayer()) return true;
	}
	if(auto e = dynamic_cast<MeleeHitEvent*>(&event)) {
		ignore(e);
		return true;
	}
	if(auto e = dynamic_cast<RangedHitEvent*>(&event)) {
		ignore(e);
		return true;
	}
	if(auto e = dynamic_cast<RequestDescriptionEvent*>(&event)) {
		ignore(e);
		return true;
	}
	if(auto e = dynamic_cast<DeathEvent*>(&event)) {
		ignore(e);
		return true;
	}
	if(auto e = dynamic_cast<PlayerDeathEvent*>(&event)) {
		ignore(e);
		return true;
	}
	if(auto e = dynamic_cast<PlayerStatChangeEvent*>(&event)) {
		ignore(e);
		return true;
	}
	if(auto e = dynamic_cast<UiEvent*>(&event)) {
		ignore(e);
		return true;
	}
	if(auto e = dynamic_cast<StatusEffectEvent*>(&event)) {
		ignore(e);
		/* if(e->actor->isPlayer()) */ return true;
	}
	if(auto e = dynamic_cast<PickableHealthEffectEvent*>(&event)) {
		ignore(e);
		/* if(e->actor->isPlayer()) */ return true;
	}
	if(auto e = dynamic_cast<AiChangeEvent*>(&event)) {
		ignore(e);
		return true;
	}
	if(auto e = dynamic_cast<StatusEffectChangeEvent*>(&event)) {
		ignore(e);
		return true;
	}
	if(auto e = dynamic_cast<GenericActorEvent*>(&event)) {
		ignore(e);
		return true;
	}
	return false;
}
