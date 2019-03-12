#include "audio.hpp"

#ifdef __APPLE__
# ifdef BUNDLE_BUILD
#  include <iostream>
#  include "CoreFoundation/CoreFoundation.h"
# endif /* BUNDLE_BULD */
#endif /* __APPLE__ */

AudioPlayer::AudioPlayer() {
#ifdef __APPLE__
# ifdef BUNDLE_BUILD
	CFBundleRef mainBundle = CFBundleGetMainBundle();
    CFURLRef resourcesURL = CFBundleCopyResourcesDirectoryURL(mainBundle);
    char path[PATH_MAX];
    if (!CFURLGetFileSystemRepresentation(resourcesURL, TRUE, (UInt8 *)path, PATH_MAX))
    {
    }
    CFRelease(resourcesURL);
    chdir(path);
    //std::cout << "Current Path: " << path << std::endl;

	if(!music.openFromFile("../Resources/Assets/main_theme.ogg")) {
		if(!music.openFromFile("assets/main_theme.ogg")) {
			std::cout << "couldn't open music file" << "\n";
		}
	}
# endif /* BUNDLE_BUILD */
	if(!music.openFromFile("assets/main_theme.ogg")) { ;/* error */ }
#else /* __APPLE__ */
	if(!music.openFromFile("assets/main_theme.ogg")) { ;/* error */ }
#endif
	music.setLoop(true);
}
