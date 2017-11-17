#ifndef PERSISTENT_HPP
#define PERSISTENT_HPP

#include <boost/serialization/unique_ptr.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/utility.hpp>
#include <SFML/Graphics.hpp>

namespace boost {
namespace serialization {

template<class Archive>
inline void serialize (
	Archive & ar,
	sf::Color & c,
	const unsigned int file_version
) {
	ar & c.r;
	ar & c.g;
	ar & c.b;
	ar & c.a;
}

} // namespace serialization
} // namespace boost

#endif /* PERSISTENT_HPP */
