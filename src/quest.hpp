#ifndef QUEST_HPP
#define QUEST_HPP

class Quest {
public:
  Quest(std::string name = "", std::string description = ""):
    name(name), description(description) {}
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

#endif /* QUEST_HPP */
