#ifndef MAIN_HPP
#define MAIN_HPP
#include <fstream>
#include <sstream>
#include <boost/archive/text_oarchive.hpp>                                                                    
#include <boost/archive/text_iarchive.hpp> 
#include <boost/serialization/export.hpp>
#include <boost/serialization/vector.hpp>

#include "libtcod.hpp"
#include "persistent.hpp"
#include "engine.hpp"

void save();
void load();
void showMenu();
#endif /* MAIN_HPP */
