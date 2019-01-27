#include "mission.hpp"

void Mission::notify(Event& event) {
  if(auto e = dynamic_cast<DeathEvent*>(&event)) {
    if(e->actor->name == "bill" && this->name == "Kill Bill"){
      completed = true;
    }
  }
}
