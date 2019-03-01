#ifndef PERSISTENT_HPP
#define PERSISTENT_HPP

#include <boost/serialization/unique_ptr.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/utility.hpp>
#include <SFML/Graphics.hpp>
#include <optional>

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

// std::optional<T>
template<class Archive, class T>
void save (
	Archive & ar,
	const std::optional<T> & t,
	const unsigned int file_version
) {
	const bool isInitialized = t ? true : false;
	ar << boost::serialization::make_nvp("initialized", isInitialized);
	if (isInitialized){
		const boost::serialization::item_version_type item_version(version<T>::value);
		ar << BOOST_SERIALIZATION_NVP(item_version);
		ar << boost::serialization::make_nvp("value", *t);
	}
}

template<class Archive, class T>
void load (
	Archive & ar,
	std::optional<T> & t,
	const unsigned int file_version
) {
	bool isInitialized;
	ar >> boost::serialization::make_nvp("initialized", isInitialized);
	if (isInitialized){
		boost::serialization::item_version_type item_version(0);
		boost::archive::library_version_type library_version(
			ar.get_library_version()
		);
		if(boost::archive::library_version_type(3) < library_version){
			// item_version is handled as an attribute so it doesnt need an NVP
			ar >> BOOST_SERIALIZATION_NVP(item_version);
		}
		detail::stack_construct<Archive, T> aux(ar, item_version);
		ar >> boost::serialization::make_nvp("value", aux.reference());
		t = aux.reference();
	}
	else {
		// t.reset(); // should be uninitialized anywaw
	}
}

template<class Archive, class T>
void serialize (
	Archive & ar,
	std::optional<T> & t,
	const unsigned int file_version
) {
	boost::serialization::split_free(ar, t, file_version);
}

} // namespace serialization
} // namespace boost

#endif /* PERSISTENT_HPP */
