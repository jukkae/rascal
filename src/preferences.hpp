#ifndef PREFERENCES_HPP
#define PREFERENCES_HPP
struct Preferences {
	std::pair<std::string, bool> music = std::make_pair("music", true);
	std::pair<std::string, bool> dummy = std::make_pair("dummy", true);
};
#endif /* PREFERENCES_HPP */
