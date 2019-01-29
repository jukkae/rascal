#include "mission.hpp"

void KillMission::notify(Event& event) {
  if(auto e = dynamic_cast<DeathEvent*>(&event)) {
    if(e->actor->name == "bill" && this->name == "Kill Bill"){
      completed = true;
    }
  }
}
