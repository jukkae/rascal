#include <boost/serialization/unique_ptr.hpp>

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
