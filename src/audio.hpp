#ifndef AUDIO_HPP
#define AUDIO_HPP

#include <SFML/Audio.hpp>

class AudioPlayer {
public:
	AudioPlayer();
private:
	sf::Music music;
};

#endif /* AUDIO_HPP */
