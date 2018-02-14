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
	io::text(dungeonLvlString, 3, 3+constants::SCREEN_HEIGHT-constants::GUI_PANEL_HEIGHT, colors::white);

	std::string timeString = "Time: " + std::to_string(world->time);
	io::text(timeString, 3, 4+constants::SCREEN_HEIGHT-constants::GUI_PANEL_HEIGHT, colors::white);

	renderXpBar(world, window);
	renderMouseLook(world, window);

	renderStatusEffects(world, window);
	renderStats(world, window);
	renderNav(world, window);
	//console.draw();
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
	io::text(acString, constants::SCREEN_WIDTH-40, 1+constants::SCREEN_HEIGHT-constants::GUI_PANEL_HEIGHT, colors::lightBlue);

	std::string weaponString;
	if(!player->wornWeapon) weaponString = "Wielding: nothing";
	else { // FIXME i know it's bad
		weaponString = "Wielding: " + player->wornWeapon->name;
		if(player->wornWeapon->rangedAttacker) weaponString.append(" (").append(std::to_string(player->wornWeapon->rangedAttacker->rounds)).append("/").append(std::to_string(player->wornWeapon->rangedAttacker->capacity)).append(")");
	}
	io::text(weaponString, constants::SCREEN_WIDTH-40, 2+constants::SCREEN_HEIGHT-constants::GUI_PANEL_HEIGHT, colors::lightBlue);

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
	io::text(atkString, constants::SCREEN_WIDTH-40, 3+constants::SCREEN_HEIGHT-constants::GUI_PANEL_HEIGHT, colors::lightBlue);

//	std::string atkBonusString = "s:" + std::to_string(player->body->getModifier(player->body->strength))
//		+ " a:" + std::to_string(player->body->getModifier(player->body->agility))
//		+ " i:" + std::to_string(player->body->getModifier(player->body->intelligence));
//	io::text(atkBonusString, constants::SCREEN_WIDTH-40, 4+constants::SCREEN_HEIGHT-constants::GUI_PANEL_HEIGHT, colors::lightBlue);

}

void Gui::renderNav(World* world, sf::RenderWindow* window) {
	Actor* player = world->getPlayer(); // TODO instead pass player - rather, the actor - as a parameter to both Gui and Renderer
	if(std::any_of(
		player->container->inventory.begin(),
		player->container->inventory.end(),
		[&] (const std::unique_ptr<Actor>& a) { return a->name == "navigation computer"; }
	)) {
		std::string location = std::to_string(player->x) + ", " + std::to_string(player->y);
		io::text(location, constants::SCREEN_WIDTH-20, 3+constants::SCREEN_HEIGHT-constants::GUI_PANEL_HEIGHT, colors::lightBlue);
		std::string rad = "rad: " + std::to_string(world->radiation);
		io::text(rad, constants::SCREEN_WIDTH-20, 4+constants::SCREEN_HEIGHT-constants::GUI_PANEL_HEIGHT, colors::lightBlue);

		// TODO doesn't belong here, temporary until info management is figured out
		std::string hungerString = "nutrition:" + std::to_string(player->body->nutrition);
		io::text(hungerString, constants::SCREEN_WIDTH-40, 5+constants::SCREEN_HEIGHT-constants::GUI_PANEL_HEIGHT, colors::lightBlue);

		std::string iodineString = "iodine:" + std::to_string(player->body->iodine);
		io::text(iodineString, constants::SCREEN_WIDTH-40, 6+constants::SCREEN_HEIGHT-constants::GUI_PANEL_HEIGHT, colors::lightBlue);
	}

}

void Gui::renderStatusEffects(World* world, sf::RenderWindow* window) {
	Actor* player = world->getPlayer();

	if(player->getStatusEffects().size() > 0) {
		io::text("status effects:", constants::SCREEN_WIDTH-60, 1+constants::SCREEN_HEIGHT-constants::GUI_PANEL_HEIGHT, colors::lightBlue);
		int i = 1;
		for (auto& e : player->getStatusEffects()) {
			std::string effectStr = e->name;
			io::text(effectStr, constants::SCREEN_WIDTH-60, 1+i+constants::SCREEN_HEIGHT-constants::GUI_PANEL_HEIGHT, colors::lightBlue);
			++i;
		}
	}
}

void Gui::renderMessageLog(sf::RenderWindow* window) {
	int y = 1;
	float colCoef = 0.4f;
	for(Message msg : log) {
		io::text(msg.text, MSG_X, y+constants::SCREEN_HEIGHT-constants::GUI_PANEL_HEIGHT, colors::multiply(msg.col, colCoef));
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
	std::string string = name + " : " + std::to_string((int)value) + "/" + std::to_string((int)maxValue);
	io::text(string, x, y+constants::SCREEN_HEIGHT-constants::GUI_PANEL_HEIGHT, colors::white);
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
	std::string string = buf;
	io::text(string, 1, 0+constants::SCREEN_HEIGHT-constants::GUI_PANEL_HEIGHT, colors::lightGrey);
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
