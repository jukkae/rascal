#include "constants.hpp"
#include "gameplay_state.hpp"
#include "engine.hpp"

void GameplayState::init(Engine* engine) {
	e = engine;
	inputHandler = std::unique_ptr<InputHandler>(new InputHandler(engine));

	gui = std::unique_ptr<Gui>(new Gui());
	gui->setState(this);
	renderer = std::unique_ptr<Renderer>(new Renderer(constants::SCREEN_WIDTH, constants::SCREEN_HEIGHT));
	renderer->setState(this);

	Actor* player        = new Actor(40, 25, '@', "you", TCODColor::white, 2); // TODO
	player->destructible = std::unique_ptr<Destructible>(new PlayerDestructible(30, 2, 0, "your corpse"));
	player->attacker     = std::unique_ptr<Attacker>(new Attacker(5));
	player->ai           = std::unique_ptr<Ai>(new PlayerAi());
	player->container    = std::unique_ptr<Container>(new Container(26));
	actors.push_back(player);

	Actor* stairs = new Actor(0, 0, '>', "stairs", TCODColor::white, 0, true);
    stairs->blocks = false;
    stairs->fovOnly = false;
    actors.push_back(stairs);

	map = std::unique_ptr<Map>(new Map(120, 72));
	map->setState(this);
	map->init(true);
	// not really the correct place for following, but w/e
	for (auto a : actors) a->setState(this);
	std::sort(actors.begin(), actors.end(), [](const auto& lhs, const auto& rhs)
	{
		return lhs->energy > rhs->energy;
	});


	gui->message(TCODColor::green, "Welcome to year 20XXAD, you strange rascal!\nPrepare to fight or die!");
}

void GameplayState::initLoaded(Engine* engine) {
	e = engine;
	inputHandler = std::unique_ptr<InputHandler>(new InputHandler(engine));
	gui->setState(this);
	renderer = std::unique_ptr<Renderer>(new Renderer(constants::SCREEN_WIDTH, constants::SCREEN_HEIGHT));
	renderer->setState(this);
	map->setState(this);
	map->init(false);
	for (auto a : actors) a->setState(this);
}

void GameplayState::cleanup() {

}

void GameplayState::update(Engine* engine) {
	computeFov();

	render(engine); // yep rendering is that cheap now
	Actor* activeActor = getNextActor();
	if(activeActor->isPlayer()) {
		//render();
	}
	updateNextActor();
	if(activeActor->isPlayer()) {
		markExploredTiles();
		render(engine);
	}
}

void GameplayState::handleEvents(Engine* engine) {
	inputHandler->handleEvents();
}

void GameplayState::render(Engine* engine) {
	renderer->render(map.get(), &actors);
	gui->render();
}

void GameplayState::updateNextActor() {
    Actor* activeActor = actors.front();

    float actionTime = activeActor->update(this);
    *activeActor->energy -= actionTime;

    actors.erase(actors.begin());
    auto it = std::lower_bound(actors.begin(), actors.end(), activeActor, [](const auto& lhs, const auto& rhs) { return lhs->energy > rhs->energy; });
    actors.insert(it, activeActor);

    /*std::sort(actors.begin(), actors.end(), [](const auto& lhs, const auto& rhs)
    {
        return lhs->energy > rhs->energy;
    });*/
	updateTime();
}

void GameplayState::updateTime() {
	if(!actors.front()->energy) return;
	if(actors.front()->energy.get() > 0) return;
	else {
		Actor* next = *std::find_if(actors.begin(), actors.end(), [](const auto& a) { return a->ai != nullptr; });

		float tuna = next->energy.get() * -1;
		time += tuna;

		for(auto a : actors) {
			if(a->ai != nullptr) *a->energy += tuna;
		}
	}
}


Actor* GameplayState::getPlayer() const {
	for(Actor* actor : actors) {
        if(actor->isPlayer()) return actor;
    }
    return nullptr;
}

Actor* GameplayState::getStairs() const {
	for(Actor* actor : actors) {
        if(actor->isStairs()) return actor;
    }
    return nullptr;
}

Point GameplayState::getMouseLocation() {
	return inputHandler->getMouseLocation();
}

bool GameplayState::canWalk(int x, int y) {
	for(Actor* actor : actors) {
		if(actor->blocks && actor->x == x && actor->y == y) {
			return false;
		}
	}
	return true;
}

Actor* GameplayState::getClosestMonster(int x, int y, float range) const {
	Actor* closest = nullptr;
	float bestDistance = std::numeric_limits<float>::max();
	for (Actor* actor : actors) {
		if(!actor->isPlayer() && actor->destructible && !actor->destructible->isDead()) {
			float distance = actor->getDistance(x,y);
			if(distance < bestDistance && (distance <= range || range == 0.0f)) {
				bestDistance = distance;
				closest = actor;
			}
		}
	}
	return closest;
}

Actor* GameplayState::getLiveActor(int x, int y) const {
	for(Actor* actor : actors) {
		if(actor->x == x && actor->y == y && actor->destructible && !actor->destructible->isDead()) return actor;
	}
	return nullptr;
}

void GameplayState::message(const TCODColor& col, std::string text, ...) {
	va_list args;
	va_start(args, text);
	gui->message(col, text, args);
	va_end(args);
}

void GameplayState::nextLevel() {
	++level;
	gui->message(TCODColor::lightViolet,"You take a moment to rest, and recover your strength.");
	getPlayer()->destructible->heal(getPlayer()->destructible->maxHp/2);
	gui->message(TCODColor::red,"After a rare moment of peace, you descend\ndeeper into the heart of the dungeon...");

	// Clunky, not idiomatic
	auto it = actors.begin();
	while (it != actors.end()) {
		if (!((*it)->isPlayer()) && !((*it)->isStairs())) {
			it = actors.erase(it);
		}
		else ++it;
	}

	// gameplayState.initMap() or something like that, remember to init actors
	map = std::unique_ptr<Map>(new Map(120, 72));
	map->setState(this);
	map->init(true);

	std::sort(actors.begin(), actors.end(), [](const auto& lhs, const auto& rhs)
	{
		return lhs->energy > rhs->energy;
	});
	for (auto a : actors) a->setState(this);
}

bool GameplayState::pickTile(int* x, int* y, float maxRange) {
	while(!TCODConsole::isWindowClosed()) {
		render(nullptr); // yeah inorite
		for(int cx = 0; cx < constants::SCREEN_WIDTH; ++cx) {
			for(int cy = 0; cy < constants::SCREEN_HEIGHT; ++cy) {
				Point location = renderer->getWorldCoordsFromScreenCoords(Point(cx, cy));
				int realX = location.x;
				int realY = location.y;
				if(isInFov(realX, realY) && (maxRange == 0 || getPlayer()->getDistance(realX, realY) <= maxRange)) {
					TCODColor col = TCODConsole::root->getCharBackground(cx, cy);
					col = col * 1.2;
					TCODConsole::root->setCharBackground(cx, cy, col);
				}
			}
		}
		inputHandler->handleEvents(); // ehh
		Point mouseLocationScreen = inputHandler->getMouseLocation();
		Point mouseLocation = renderer->getWorldCoordsFromScreenCoords(Point(mouseLocationScreen.x, mouseLocationScreen.y));
		int realX = mouseLocation.x;
		int realY = mouseLocation.y;
		if(isInFov(realX, realY) && (maxRange == 0 || getPlayer()->getDistance(realX, realY) <= maxRange)) {
			TCODConsole::root->setCharBackground(mouseLocationScreen.x, mouseLocationScreen.y, TCODColor::white);
			if(inputHandler->mouseLeftClicked) {
				inputHandler->mouseLeftClicked = false;
				*x = realX;
				*y = realY;
				return true;
			}
		}
		if(inputHandler->mouseRightClicked) {
			inputHandler->mouseRightClicked = false;
			return false;
		}
		TCODConsole::flush();
	}
	return false;
}

BOOST_CLASS_EXPORT(GameplayState)
