#ifndef MISSION_HPP
#define MISSION_HPP
#include "event.hpp"

struct Event;

class Mission {
public:
  Mission(std::string name = "", std::string description = ""):
    name(name), description(description) {}
  void notify(Event& e);
  std::string name;
  std::string description;
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & name;
    ar & description;
	}
};

#endif /* MISSION_HPP */
