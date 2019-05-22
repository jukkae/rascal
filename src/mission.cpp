#include "mission.hpp"
#include "container.hpp"
#include "event.hpp"

void KillMission::notify(Event& event) {
  if(auto e = dynamic_cast<DeathEvent*>(&event)) {
    if(e->actor->name == "bill" && this->name == "Kill Bill"){
      status = MissionStatus::REQUIRES_CONFIRMATION;
    }
  }
}

void AcquireItemsMission::notify(Event& event) {
  if(auto e = dynamic_cast<ItemPickedUpEvent*>(&event)) {
    std::vector<Actor*> inventoryContents;
    for(auto& item : e->finder->container->inventory) inventoryContents.push_back(item.get());

    if(status == MissionStatus::ACTIVE) {
      int numberOfRamChips = 0;
      for(auto& i : inventoryContents) if(i->name == "ram chip") ++numberOfRamChips;
      if(numberOfRamChips >= 2) status = MissionStatus::REQUIRES_CONFIRMATION;
    }
  }
}
