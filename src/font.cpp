#include "font.hpp"
#include <iostream>

#ifdef __APPLE__
# ifdef BUNDLE_BUILD
# include "CoreFoundation/CoreFoundation.h"
# endif /* BUNDLE_BULD */
#endif /* __APPLE__ */

sf::Font font::mainFont;
sf::Font font::squareFont;

namespace {
std::string squareFontFileName = "square.ttf";
std::string mainFontFileName   = "FSEX300.ttf";
} // namespace

extern void font::load() {
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

	if(!font::mainFont.loadFromFile("../Resources/Assets/" + mainFontFileName)) {
		if(!font::mainFont.loadFromFile("assets/" + mainFontFileName)) {
			std::cout << "font loading failed\n";
		}
	}
	if(!font::squareFont.loadFromFile("../Resources/Assets/" + squareFontFileName)) {
		if(!font::squareFont.loadFromFile("assets/" + squareFontFileName)) {
			std::cout << "font loading failed\n";
		}
	}
# else /* BUNDLE_BUILD */
	if(!font::mainFont.loadFromFile("assets/" + mainFontFileName)) {
		std::cout << "error loading main font\n";
	}
	if(!font::squareFont.loadFromFile("assets/" + squareFontFileName)) {
		std::cout << "error loading square font\n";
	}
# endif /* __APPLE__ */
#elif __linux__
	if(!font::mainFont.loadFromFile("assets/" + mainFontFileName)) {
		std::cout << "error loading main font\n";
	}
	if(!font::squareFont.loadFromFile("assets/" + squareFontFileName)) {
		std::cout << "error loading square font\n";
	}
#else
#    error "unknown compiler"
#endif
}
