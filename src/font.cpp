#include "font.hpp"
#include "log.hpp"

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

	if(!font::mainFont.loadFromFile("../Resources/Assets/" + mainFontFileName)) {
		if(!font::mainFont.loadFromFile("assets/" + mainFontFileName)) {
			log::info("font loading failed");
		}
	}
	if(!font::squareFont.loadFromFile("../Resources/Assets/" + squareFontFileName)) {
		if(!font::squareFont.loadFromFile("assets/" + squareFontFileName)) {
			log::info("font loading failed");
		}
	}
# else /* BUNDLE_BUILD */
	if(!font::mainFont.loadFromFile("assets/" + mainFontFileName)) {
		log::info("error loading main font");
	}
	if(!font::squareFont.loadFromFile("assets/" + squareFontFileName)) {
		log::info("error loading square font");
	}
# endif /* __APPLE__ */
#elif __linux__
	if(!font::mainFont.loadFromFile("assets/" + mainFontFileName)) {
		log::info("error loading main font");
	}
	if(!font::squareFont.loadFromFile("assets/" + squareFontFileName)) {
		log::info("error loading square font");
	}
#else
#    error "unknown compiler"
#endif
}
