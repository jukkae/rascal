#include "font.hpp"
#include <iostream>

#ifdef __APPLE__
#include "CoreFoundation/CoreFoundation.h"
#endif

sf::Font font::mainFont;
sf::Font font::squareFont;

extern void font::load() {
#ifdef BUNDLE_BUILD
	//std::cout << "BUNDLE BUILD DETECTED\n";
#endif
#ifdef __APPLE__
	// FIXME this ifdef only when building appbundle!
	CFBundleRef mainBundle = CFBundleGetMainBundle();
    CFURLRef resourcesURL = CFBundleCopyResourcesDirectoryURL(mainBundle);
    char path[PATH_MAX];
    if (!CFURLGetFileSystemRepresentation(resourcesURL, TRUE, (UInt8 *)path, PATH_MAX))
    {
    }
    CFRelease(resourcesURL);

    chdir(path);
    std::cout << "Current Path: " << path << std::endl;


	if(!font::mainFont.loadFromFile("../Resources/Assets/FSEX300.ttf")) {
		std::cout << "error loading main font: Not in bundle or bundle malformed\n";
		if(!font::mainFont.loadFromFile("assets/FSEX300.ttf")) {
			std::cout << "retry failed\n";
		}
	}
	if(!font::squareFont.loadFromFile("../Resources/Assets/square.ttf")) {
		std::cout << "error loading square font: Not in bundle or bundle malformed\n";
		if(!font::squareFont.loadFromFile("assets/square.ttf")) {
			std::cout << "retry failed\n";
		}
	}
#elif __linux__
	if(!font::mainFont.loadFromFile("assets/FSEX300.ttf")) {
		std::cout << "error loading main font\n";
	}
	if(!font::squareFont.loadFromFile("assets/square.ttf")) {
		std::cout << "error loading square font\n";
	}
#else
#    error "unknown compiler"
#endif
}
