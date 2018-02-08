#ifndef PREFERENCES_HPP
#define PREFERENCES_HPP
struct Preferences {
	std::pair<std::string, bool> music = std::make_pair("music", true);
	std::pair<std::string, bool> dummy = std::make_pair("dummy", false);
	std::pair<std::string, bool> longer_dummy_preference = std::make_pair("longer_dummy_preference", true);

};

inline std::ostream& operator<<(std::ostream& os, const Preferences& obj)
{ // TODO implement
	// write obj to stream
	return os;
}

inline std::istream& operator>>(std::istream& is, Preferences& obj)
{ // TODO implement
	// read obj from stream
	// if( /* no valid object of T found in stream */ ) is.setstate(std::ios::failbit);
	return is;
}
#endif /* PREFERENCES_HPP */
