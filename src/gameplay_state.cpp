#include "gameplay_state.hpp"
#include "engine.hpp"

void GameplayState::init() {
	gui = &engine.gui;
	gui->setState(this);
	renderer = &engine.renderer;
	renderer->setState(this);

	player               = new Actor(40, 25, '@', "you", TCODColor::white, 2); // TODO
	player->destructible = std::unique_ptr<Destructible>(new PlayerDestructible(30, 2, 0, "your corpse"));
	player->attacker     = std::unique_ptr<Attacker>(new Attacker(5));
	player->ai           = std::unique_ptr<Ai>(new PlayerAi());
	player->container    = std::unique_ptr<Container>(new Container(26));
	actors->push_back(player);

	stairs = new Actor(0, 0, '>', "stairs", TCODColor::white);
    stairs->blocks = false;
    stairs->fovOnly = false;
    actors->push_back(stairs);

	gui->message(TCODColor::green, "Welcome to year 20XXAD, you strange rascal!\nPrepare to fight or die!");
}

void GameplayState::initMap() {
	map = std::unique_ptr<Map>(new Map(120, 72));
	map->setState(this);
	map->init(true);
	// not really the correct place for following, but w/e
	for (auto a : *actors) a->setState(this);
	std::sort(actors->begin(), actors->end(), [](const auto& lhs, const auto& rhs)
	{
		return lhs->energy > rhs->energy;
	});

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

void GameplayState::render(Engine* engine) {
	renderer->render(map.get(), actors.get());
	gui->render();
}

void GameplayState::updateNextActor() {
    Actor* activeActor = actors->at(0);

    int actionTime = activeActor->update(this);
    *activeActor->energy -= actionTime;

    actors->erase(actors->begin());
    auto it = std::lower_bound(actors->begin(), actors->end(), activeActor, [](const auto& lhs, const auto& rhs) { return lhs->energy > rhs->energy; });
    actors->insert(it, activeActor);
    /*std::sort(actors->begin(), actors->end(), [](const auto& lhs, const auto& rhs)
    {
        return lhs->energy > rhs->energy;
    });*/
	updateTime();
}

void GameplayState::updateTime() {
	if(actors->at(0)->energy.get() > 0) return; // TODO check if energy HAS value (if(a->energy))
	else {
		Actor* next = *std::find_if(actors->begin(), actors->end(), [](const auto& a) { return a->ai != nullptr; });

		float tuna = next->energy.get() * -1;

		increaseTime(tuna);

		for(auto a : *actors) {
			if(a->ai != nullptr) *a->energy += tuna;
		}
	}
}


Actor* GameplayState::getPlayer() const {
	for(Actor* actor : *actors) {
        if(actor->isPlayer()) return actor;
    }
    return nullptr;
}

Point GameplayState::getMouseLocation() {
	return Point(engine.mouse.cx, engine.mouse.cy);
}

bool GameplayState::canWalk(int x, int y) {
	for(Actor* actor : *actors) {
		if(actor->blocks && actor->x == x && actor->y == y) {
			return false;
		}
	}
	return true;
}

Actor* GameplayState::getClosestMonster(int x, int y, float range) const {
	Actor* closest = nullptr;
	float bestDistance = std::numeric_limits<float>::max();
	for (Actor* actor : *actors) {
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
	for(Actor* actor : *actors) {
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
	increaseLevel();
	gui->message(TCODColor::lightViolet,"You take a moment to rest, and recover your strength.");
	player->destructible->heal(player->destructible->maxHp/2);
	gui->message(TCODColor::red,"After a rare moment of peace, you descend\ndeeper into the heart of the dungeon...");

	// Clunky, not idiomatic
	auto it = actors->begin();
	while (it != actors->end()) {
		if (*it != player && *it != stairs) {
			it = actors->erase(it);
		}
		else ++it;
	}

	// gameplayState.initMap() or something like that, remember to init actors

	std::sort(actors->begin(), actors->end(), [](const auto& lhs, const auto& rhs)
	{
		return lhs->energy > rhs->energy;
	});
}

void GameplayState::showLevelUpMenu() {
	gui->menu.clear();
	gui->menu.addItem(Menu::MenuItemCode::CONSTITUTION,"Constitution (+20HP)");
	gui->menu.addItem(Menu::MenuItemCode::STRENGTH,"Strength (+1 attack)");
	gui->menu.addItem(Menu::MenuItemCode::AGILITY,"Agility (+1 defense)");
	gui->menu.addItem(Menu::MenuItemCode::SPEED,"Speed (+10 speed)");
}
