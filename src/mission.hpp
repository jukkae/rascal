#ifndef MISSION_HPP
#define MISSION_HPP


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

class Mission {
public:
  Mission(std::string name = "", std::string description = "", Actor* giver = nullptr):
    name(name), description(description), giver(giver) {}
  virtual ~Mission() {}
  virtual void notify(Event& event) = 0;
  std::string name;
  std::string description;
  Actor* giver;
  bool completed = false;
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & name;
    ar & description;
    ar & completed;
    ar & giver;
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

#endif /* MISSION_HPP */
