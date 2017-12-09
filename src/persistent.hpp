#ifndef PERSISTENT_HPP
#define PERSISTENT_HPP

#include <boost/serialization/unique_ptr.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/utility.hpp>
#include <SFML/Graphics.hpp>
#include <experimental/optional>

namespace boost {
namespace serialization {

// sf::Color
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

// std::experimental::optional<T>
template<class Archive, class T>
void save (
	Archive & ar,
	const std::experimental::optional<T> & t,
	const unsigned int file_version
) {
// TODO
}

template<class Archive, class T>
void load (
	Archive & ar,
	std::experimental::optional<T> & t,
	const unsigned int file_version
) {
// TODO
}

template<class Archive, class T>
void serialize (
	Archive & ar,
	std::experimental::optional<T> & t,
	const unsigned int file_version
) {
	boost::serialization::split_free(ar, t, file_version);
}

} // namespace serialization
} // namespace boost

#endif /* PERSISTENT_HPP */
