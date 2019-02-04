#include "messaging.hpp"

#include "actor.hpp"
#include "colors.hpp"
#include "effect.hpp"
#include "event.hpp"
#include "ignore.hpp"

std::string messaging::formatString(std::string text, ...) {
	va_list ap;
	std::string buf = "";
	char dest[1024*16]; // maybe something a bit more sane would be in order at some point
	va_start(ap, text);
	vsnprintf(dest, 1024*16, text.c_str(), ap); // FIXME
	va_end(ap);
	return std::string(dest);
	// std::istringstream iss (dest);
	// std::string line;
	// while (std::getline(iss, line, '\n')) {
	// 	// make room for the message
	// 	if(log.size() == MSG_HEIGHT) {
	// 		log.erase(log.begin());
	// 	}
	// 	Message msg(line, col);
	// 	log.push_back(msg);
	// } // multiline formatting for future reference
}


Message messaging::createMessageFromEvent(Event& event) {
	std::string fmt;
	std::string messageText = "UNKNOWN EVENT";
	sf::Color color = colors::get("white");

	if(auto e = dynamic_cast<ItemFoundEvent*>(&event)) {
		fmt = "%d: There's a %s here!";
		messageText = formatString(fmt, e->time, e->item->name.c_str());
			color = colors::get("green");
	}

	if(auto e = dynamic_cast<MeleeHitEvent*>(&event)) {
		if(!e->hit) {
			fmt = "%d: %s misses %s.";
			messageText = formatString(fmt, e->time, e->hitter->name.c_str(), e->hittee->name.c_str());
			color = colors::get("grey");
		} else if(e->damage <= 0) {
			fmt = "%d: %s hits %s, but it seems to have no effect.";
			messageText = formatString(fmt, e->time, e->hitter->name.c_str(), e->hittee->name.c_str());
			color = colors::get("red");
		} else if(e->weapon) {
			fmt = "%d: %s attacks %s for %d hit points with a %s.";
			messageText = formatString(fmt, e->time, e->hitter->name.c_str(), e->hittee->name.c_str(), e->damage, e->weapon->name.c_str());
			color = colors::get("red");
		} else {
			fmt = "%d: %s attacks %s for %d hit points.";
			messageText = formatString(fmt, e->time, e->hitter->name.c_str(), e->hittee->name.c_str(), e->damage);
			color = colors::get("red");
		}
	}

	if(auto e = dynamic_cast<RangedHitEvent*>(&event)) {
		if(!e->hit) {
			fmt = "%d: %s misses %s. Bullet ricochets harmlessly!";
			messageText = formatString(fmt, e->time, e->hitter->name.c_str(), e->hittee->name.c_str());
			color = colors::get("grey");
		} else if(e->damage <= 0) {
			fmt = "%d: %s hits %s, but it seems to have no effect.";
			messageText = formatString(fmt, e->time, e->hitter->name.c_str(), e->hittee->name.c_str());
			color = colors::get("red");
		} else if(e->weapon) {
			fmt = "%d: %s shoots %s for %d hit points with a %s.";
			messageText = formatString(fmt, e->time, e->hitter->name.c_str(), e->hittee->name.c_str(), e->damage, e->weapon->name.c_str());
			color = colors::get("red");
		}
	}

	if(auto e = dynamic_cast<ActionFailureEvent*>(&event)) {
		fmt = "%d: %s";
		messageText = formatString(fmt, e->time, e->failureMessage.c_str());
		color = colors::get("lightRed");
	}

	if(auto e = dynamic_cast<ActionSuccessEvent*>(&event)) {
		fmt = "%d: %s";
		messageText = formatString(fmt, e->time, e->successMessage.c_str());
		color = colors::get("green");
	}

	if(auto e = dynamic_cast<RequestDescriptionEvent*>(&event)) {
		ignore(e);
		messageText = "You take a look around. It's bleak."; // TODO obvs more stuff; time!
		color = colors::get("grey");
	}

	if(auto e = dynamic_cast<UiEvent*>(&event)) {
		messageText = e->text;
		color = colors::get("cyan");
	}

	if(auto e = dynamic_cast<StatusEffectEvent*>(&event)) {
		fmt = "%d: %s";
		messageText = formatString(fmt, e->time, e->text.c_str());
		color = colors::get("red"); //TODO now red for poison dmg
	}

	if(auto e = dynamic_cast<DeathEvent*>(&event)) {
		if(e->xp == 0) {
			fmt = "%d: %s is dead!";
			messageText = formatString(fmt, e->time, e->actor->name.c_str());
			color = colors::get("lightGrey");
		} else {
			fmt = "%d: %s is dead! You gain %d xp!";
			messageText = formatString(fmt, e->time, e->actor->name.c_str(), e->xp);
			color = colors::get("lightGrey");
		}
	}

	if(auto e = dynamic_cast<PickableHealthEffectEvent*>(&event)) {
		if(e->amount >= 0) {
			fmt = "%d: %s gains %d hp!";
			messageText = formatString(fmt, e->time, e->actor->name.c_str(), e->amount);
			color = colors::get("green");
		} else {
			fmt = "%d: %s is harmed for %d hp!";
			messageText = formatString(fmt, e->time, e->actor->name.c_str(), -e->amount);
			color = colors::get("red");
		}
	}

	if(auto e = dynamic_cast<PlayerDeathEvent*>(&event)) {
		fmt = "%d: You are dead!";
		messageText = formatString(fmt, e->time);
		color = colors::get("red");
	}

	if(auto e = dynamic_cast<PlayerStatChangeEvent*>(&event)) {
		fmt = "%d: You've reached level %d!";
		messageText = formatString(fmt, e->time, e->xpLevel);
		color = colors::get("yellow");
	}

	if(auto e = dynamic_cast<AiChangeEvent*>(&event)) {
		fmt = "%d: The eyes of the %s look vacant!";
		messageText = formatString(fmt, e->time, e->actor->name.c_str());
		color = colors::get("cyan");
	}

	if(auto e = dynamic_cast<StatusEffectChangeEvent*>(&event)) {
		if(e->activity == status_effect::Activity::ADD) {
			if(auto fx = dynamic_cast<PoisonedStatusEffect*>(e->effect)) {
				ignore(fx);
				fmt = "%d: The %s feels bad";
				messageText = formatString(fmt, e->time, e->actor->name.c_str());
				color = colors::get("cyan");
			} else {
				fmt = "%d: The %s feels a bit different";
				messageText = formatString(fmt, e->time, e->actor->name.c_str());
				color = colors::get("cyan");
			}
		} else {
			fmt = "%d: The %s feels a bit more normal";
			messageText = formatString(fmt, e->time, e->actor->name.c_str());
			color = colors::get("cyan");
		}
	}

	if(auto e = dynamic_cast<GenericActorEvent*>(&event)) {
		std::string text = formatString(e->formatString, e->actor->name.c_str());
		fmt = "%d: %s";
		messageText = formatString(fmt, e->time, text.c_str());
		color = colors::get("cyan");
	}

	if(auto e = dynamic_cast<EnemyHasSeenPlayerEvent*>(&event)) {
		fmt = "%d: %s has seen you!";
		messageText = formatString(fmt, e->time, e->actor);
		color = colors::get("red");
	}

	return Message(messageText, color);
}
