# Compile for macOs

CXX=clang
CPPFLAGS=-Wall -std=c++1z
INCLUDEDIRS=-I./include
LDFLAGS=-lc++ -lboost_serialization -framework SFML -framework sfml-window -framework sfml-graphics -framework sfml-system -framework CoreFoundation -rpath @executable_path/../Resources/Frameworks
BUNDLE_BUILD=-DBUNDLE_BUILD

SRC := src
OBJ := obj

# g++ -framework sfml-window -framework sfml-graphics -framework sfml-system main.cpp -o main
# https://stackoverflow.com/questions/9054987/how-can-i-compile-sfml-project-via-command-line-on-mac
# maybe also -framework OpenGL -framework SFML

SOURCES := $(wildcard src/*.cpp)
OBJECTS := $(patsubst $(SRC)/%.cpp, $(OBJ)/%.o, $(SOURCES))
.PHONY: clean run bundle-deps

rascal: $(OBJECTS)
	$(CXX) $^ -o $@ $(CPPFLAGS) $(LDFLAGS)

$(OBJ)/%.o: $(SRC)/%.cpp
	$(CXX) -I$(SRC) -c $< -o $@ $(CPPFLAGS)

#compile: clean $(OBJS)
	#$(CXX) $(SOURCES) -o rascal $(CPPFLAGS) $(INCLUDEDIRS) -Iinclude $(LDFLAGS) -I/usr/local/include

compile-debug: clean $(OBJS)
	$(CXX) $(SOURCES) -o rascal $(CPPFLAGS) -Iinclude $(LDFLAGS) -I/usr/local/include -g -O0

build-for-bundle: clean $(OBJS)
	$(CXX) $(SOURCES) -o rascal $(CPPFLAGS) -Iinclude $(LDFLAGS) -I/usr/local/include $(BUNDLE_BUILD)

clean:
	rm -rf *.o rascal save.txt rascal.dSYM obj/*.o

run:
	./rascal

bundle: build-for-bundle appbundle bundle-deps

APPNAME=Rascal
APPBUNDLE=$(APPNAME).app
APPBUNDLECONTENTS=$(APPBUNDLE)/Contents
APPBUNDLEEXE=$(APPBUNDLECONTENTS)/MacOS
APPBUNDLERESOURCES=$(APPBUNDLECONTENTS)/Resources
APPBUNDLEICON=$(APPBUNDLECONTENTS)/Resources
OUTFILE=rascal
appbundle: macosx/$(APPNAME).icns
	rm -rf $(APPBUNDLE)
	mkdir $(APPBUNDLE)
	mkdir $(APPBUNDLE)/Contents
	mkdir $(APPBUNDLE)/Contents/MacOS
	mkdir $(APPBUNDLE)/Contents/Resources
	mkdir $(APPBUNDLE)/Contents/Resources/Libs
	mkdir $(APPBUNDLE)/Contents/Resources/Frameworks
	mkdir $(APPBUNDLE)/Contents/Frameworks
	mkdir $(APPBUNDLE)/Contents/Resources/Assets/
	cp macosx/Info.plist $(APPBUNDLECONTENTS)/
	cp macosx/PkgInfo $(APPBUNDLECONTENTS)/
	cp macosx/$(APPNAME).icns $(APPBUNDLEICON)/
	cp $(OUTFILE) $(APPBUNDLEEXE)/$(APPNAME)

macosx/$(APPNAME).icns: macosx/$(APPNAME)Icon.png
	rm -rf macosx/$(APPNAME).iconset
	mkdir macosx/$(APPNAME).iconset
	sips -z 16 16     macosx/$(APPNAME)Icon.png --out macosx/$(APPNAME).iconset/icon_16x16.png
	sips -z 32 32     macosx/$(APPNAME)Icon.png --out macosx/$(APPNAME).iconset/icon_16x16@2x.png
	sips -z 32 32     macosx/$(APPNAME)Icon.png --out macosx/$(APPNAME).iconset/icon_32x32.png
	sips -z 64 64     macosx/$(APPNAME)Icon.png --out macosx/$(APPNAME).iconset/icon_32x32@2x.png
	sips -z 128 128   macosx/$(APPNAME)Icon.png --out macosx/$(APPNAME).iconset/icon_128x128.png
	sips -z 256 256   macosx/$(APPNAME)Icon.png --out macosx/$(APPNAME).iconset/icon_128x128@2x.png
	sips -z 256 256   macosx/$(APPNAME)Icon.png --out macosx/$(APPNAME).iconset/icon_256x256.png
	sips -z 512 512   macosx/$(APPNAME)Icon.png --out macosx/$(APPNAME).iconset/icon_256x256@2x.png
	sips -z 512 512   macosx/$(APPNAME)Icon.png --out macosx/$(APPNAME).iconset/icon_512x512.png
	cp macosx/$(APPNAME)Icon.png macosx/$(APPNAME).iconset/icon_512x512@2x.png
	iconutil -c icns -o macosx/$(APPNAME).icns macosx/$(APPNAME).iconset
	rm -r macosx/$(APPNAME).iconset

bundle-deps:
	cp -r ./assets/ $(APPBUNDLE)/Contents/Resources/Assets/
	cp -r ./frameworks/ $(APPBUNDLE)/Contents/Resources/Frameworks/
	cp -r ./macosx/freetype.framework/ $(APPBUNDLE)/Contents/Resources/Frameworks/freetype.framework/
	cp -r ./macosx/freetype.framework/ $(APPBUNDLE)/Contents/Frameworks/freetype.framework/
	../macdylibbundler/dylibbundler -b -x ./Rascal.app/Contents/MacOS/Rascal -d ./Rascal.app/Contents/Resources/Libs -od -p @executable_path/../Resources/Libs
	install_name_tool -change @executable_path/../Frameworks/SFML.framework/Versions/2.4.2/SFML @executable_path/../Resources/Frameworks/SFML.framework/Versions/2.4.2/SFML ./Rascal.app/Contents/MacOS/Rascal
	install_name_tool -change @executable_path/../Frameworks/sfml-window.framework/Versions/2.4.2/sfml-window @executable_path/../Resources/Frameworks/sfml-window.framework/Versions/2.4.2/sfml-window ./Rascal.app/Contents/MacOS/Rascal
	install_name_tool -change @executable_path/../Frameworks/sfml-graphics.framework/Versions/2.4.2/sfml-graphics @executable_path/../Resources/Frameworks/sfml-graphics.framework/Versions/2.4.2/sfml-graphics ./Rascal.app/Contents/MacOS/Rascal
	install_name_tool -change @executable_path/../Frameworks/sfml-system.framework/Versions/2.4.2/sfml-system @executable_path/../Resources/Frameworks/sfml-system.framework/Versions/2.4.2/sfml-system ./Rascal.app/Contents/MacOS/Rascal
