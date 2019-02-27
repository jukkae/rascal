#ifndef DIALOGUE_GENERATOR_HPP
#define DIALOGUE_GENERATOR_HPP
#include "persistent.hpp"
#include <optional>
#include "mission.hpp"

class DialogueGenerator {
public:
  DialogueGenerator() {}
  DialogueGenerator(std::experimental::optional<MissionType> missionType): missionType(missionType) {}
  std::experimental::optional<MissionType> missionType;
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
    ar & missionType;
	}
};

#endif /* DIALOGUE_GENERATOR_HPP */
