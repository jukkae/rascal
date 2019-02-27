#ifndef DIALOGUE_GENERATOR_HPP
#define DIALOGUE_GENERATOR_HPP

class DialogueGenerator {
public:
  DialogueGenerator() {}
  DialogueGenerator(bool createMission): createMission(createMission) {}
  bool createMission;
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & createMission;
	}
};

#endif /* DIALOGUE_GENERATOR_HPP */
