#ifndef SCHEDULER_HPP
#define SCHEDULER_HPP
#include <vector>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/export.hpp>
class Actor;
class Scheduler {
public:
	float getCurrentTime() const { return time; }
	Actor* getNextActor() const { return actorsQueue.at(0).second; }
	void updateNextActor();
	void insertActor(Actor* actor);
private:
	std::vector<std::pair<int, Actor*>> actorsQueue; // TODO better container
	int time;

	friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
		ar & time;
		ar & actorsQueue;
    }
};
#endif /* SCHEDULER_HPP */
