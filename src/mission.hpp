#ifndef MISSION_HPP
#define MISSION_HPP

#include "persistent.hpp"


namespace boost { namespace serialization { class access; } }

struct Event;
class Actor;

enum class MissionType {
  KILL,
  ACQUIRE_ITEMS,
  DELIVER,
  ESCORT,
  GATHER_INTEL,
  DEFEND,
  DISCOVER,
  NEGOTIATE
};

enum class MissionStatus {
  NOT_ACTIVE,
  ACTIVE,
  REQUIRES_CONFIRMATION, // Mission has been completed, but not yet confirmed
  COMPLETED, // Mission has been confirmed to be completed
  FAILED
};

class Mission {
public:
  Mission(std::string name = "", std::string description = "", Actor* giver = nullptr):
    name(name), description(description), giver(giver) {}
  virtual ~Mission() {}
  virtual void notify(Event& event) = 0;
  std::string name;
  std::string description;
  Actor* giver;
  MissionStatus status = MissionStatus::NOT_ACTIVE;
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & name;
    ar & description;
    ar & giver;
    ar & status;
	}
};

class KillMission : public Mission {
public:
	KillMission(std::string name = "", std::string description = "", Actor* giver = nullptr):
    Mission(name, description, giver) {}
	void notify(Event& event);
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Mission);
	}
};

class AcquireItemsMission : public Mission {
public:
  AcquireItemsMission(std::string name = "", std::string description = "", Actor* giver = nullptr):
    Mission(name, description, giver) {}
  void notify(Event& event);
private:
  friend class boost::serialization::access;
  template<class Archive>
  void serialize(Archive & ar, const unsigned int version) {
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Mission);
  }
};

// Whenever you add new Mission subclass, you need to register it.
#endif /* MISSION_HPP */
