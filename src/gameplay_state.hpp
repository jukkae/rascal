#ifndef GAMEPLAY_STATE_HPP
#define GAMEPLAY_STATE_HPP
class Actor;
class Engine;
#include "state.hpp"
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

class GameplayState : public State {
public:
	void init() override;
	void cleanup() override;

	void update(Engine* engine) override;
	void render(Engine* engine) override;
private:
	int time = 0;
	int level = 0;
	std::vector<Actor*> actors; // TODO should be std::vector<std::unique_ptr<Actor>>
	Actor* player; // TODO fix reliance on explicitly pointing to player
	Actor* stairs; // likewise, this feels bad

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) // TODO serialize ABC
	{
		ar & level;
		ar & time;
		// ar & map;
		ar & stairs;
		ar & player;
		ar & actors;
		// ar & gui;
	}
};
#endif /* GAMEPLAY_STATE_HPP */
