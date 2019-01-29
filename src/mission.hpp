#ifndef MISSION_HPP
#define MISSION_HPP
#include "event.hpp"

struct Event;

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

class Mission {
public:
  Mission(std::string name = "", std::string description = ""):
    name(name), description(description) {}
  virtual ~Mission() {}
  virtual void notify(Event& event) = 0;
  std::string name;
  std::string description;
  bool completed = false;
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & name;
    ar & description;
    ar & completed;
	}
};

class KillMission : public Mission {
public:
	KillMission(std::string name = "", std::string description = ""):
    Mission(name, description) {}
	void notify(Event& event);
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Mission);
	}
};

#endif /* MISSION_HPP */
