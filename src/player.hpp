#ifndef PLAYER_HPP
#define PLAYER_HPP

struct Event;
class Player {
public:
int score = 0;

void notify(Event& event);

private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & score;
	}
};

#endif /* PLAYER_HPP */
