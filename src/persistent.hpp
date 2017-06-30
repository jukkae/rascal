#ifndef PERSISTENT_HPP
#define PERSISTENT_HPP

#include <boost/serialization/unique_ptr.hpp>
#include <boost/serialization/utility.hpp>
#include "libtcod.hpp"

// Non-intrusive serializations for TCOD classes
template<class Archive>
inline void serialize (
    Archive & ar,
    TCODColor & t,
    const unsigned int file_version
) {
	ar & t.r;
	ar & t.g;
	ar & t.b;
}

#endif /* PERSISTENT_HPP */
