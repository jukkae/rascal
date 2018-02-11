#include "audio.hpp"

AudioPlayer::AudioPlayer() {
	if(!music.openFromFile("assets/main_theme.ogg")) { ;/* error */ }
	music.setLoop(true);
}
