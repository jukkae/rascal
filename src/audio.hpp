#ifndef AUDIO_HPP
#define AUDIO_HPP

#include <SFML/Audio.hpp>

class AudioPlayer {
public:
	AudioPlayer();
	sf::Music music; //FIXME let's have this public for now
private:
};

#endif /* AUDIO_HPP */
