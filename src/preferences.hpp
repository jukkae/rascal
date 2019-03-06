#ifndef PREFERENCES_HPP
#define PREFERENCES_HPP

#include <sstream>

struct Preferences {
	std::pair<std::string, bool> music = std::make_pair("music", true);
	std::pair<std::string, bool> dummy = std::make_pair("dummy", false);
	std::pair<std::string, bool> longer_dummy_preference = std::make_pair("longer_dummy_preference", true);
	//Remember to update stream operators
};

inline std::ostream& operator<<(std::ostream& os, const Preferences& p)
{
	os << p.music.first << ": " << (p.music.second ? "on" : "off") << "\n";
	os << p.dummy.first << ": " << (p.dummy.second ? "on" : "off") << "\n";
	os << p.longer_dummy_preference.first << ": " << (p.longer_dummy_preference.second ? "on" : "off") << "\n";
	// write obj to stream
	return os;
}

inline std::istream& operator>>(std::istream& is, Preferences& p)
{
	std::string line;
	int lineIndex = 0;
	while(std::getline(is, line)) {
		for (int i = 0; i < line.length(); ++i) { if (line[i] == ':') { line[i] = ' '; } }
		std::istringstream iss(line);
		std::string temp;
		switch (lineIndex) {
			case 0:
				iss >> p.music.first >> temp;
				if(temp == "on") p.music.second = true;
				else p.music.second = false;
				break;
			case 1:
				iss >> p.dummy.first >> temp;
				if(temp == "on") p.dummy.second = true;
				else p.dummy.second = false;
				break;
			case 2:
				iss >> p.longer_dummy_preference.first >> temp;
				if(temp == "on") p.longer_dummy_preference.second = true;
				else p.longer_dummy_preference.second = false;
				break;
			default: break;
		}
		++lineIndex;
	}
	return is;
}
#endif /* PREFERENCES_HPP */
