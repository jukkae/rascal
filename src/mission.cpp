#include "mission.hpp"
#include "event.hpp"

void KillMission::notify(Event& event) {
  if(auto e = dynamic_cast<DeathEvent*>(&event)) {
    if(e->actor->name == "bill" && this->name == "Kill Bill"){
      status = MissionStatus::REQUIRES_CONFIRMATION;
    }
  }
}

void AcquireItemsMission::notify(Event& event) {
  if(auto e = dynamic_cast<DeathEvent*>(&event)) {
    // if(e->actor->name == "bill" && this->name == "Kill Bill"){
    //   completed = true;
    // }
  }
}