#include <fstream>
#include <sstream>
#include <boost/archive/text_oarchive.hpp>                                                                    
#include <boost/archive/text_iarchive.hpp> 
#include <boost/serialization/export.hpp>
#include <boost/serialization/vector.hpp>

#include "libtcod.hpp"
class Actor;
#include "persistent.hpp"
#include "destructible.hpp"
#include "attacker.hpp"
#include "ai.hpp"
#include "pickable.hpp"
#include "container.hpp"
#include "actor.hpp"
#include "map.hpp"
#include "gui.hpp"
#include "engine.hpp"

void save();
void load();
void showMenu();
