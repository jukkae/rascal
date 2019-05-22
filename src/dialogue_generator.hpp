#ifndef DIALOGUE_GENERATOR_HPP
#define DIALOGUE_GENERATOR_HPP
#include "persistent.hpp"
#include "mission.hpp"

class DialogueGenerator {
public:
  DialogueGenerator() {}
  DialogueGenerator(std::optional<MissionType> missionType): missionType(missionType) {}
  std::optional<MissionType> missionType;
  Mission* mission = nullptr;
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
    ar & missionType;
    ar & mission;
	}
};

#endif /* DIALOGUE_GENERATOR_HPP */
