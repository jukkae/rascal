#ifndef PREFERENCES_HPP
#define PREFERENCES_HPP
struct Preferences {
	std::pair<std::string, bool> music = std::make_pair("music", true);
	std::pair<std::string, bool> dummy = std::make_pair("dummy", false);
	std::pair<std::string, bool> longer_dummy_preference = std::make_pair("longer_dummy_preference", true);
};
#endif /* PREFERENCES_HPP */
