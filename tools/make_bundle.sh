#!/bin/sh

rm -rf Rascal.app

mkdir Rascal.app
mkdir Rascal.app/Contents
mkdir Rascal.app/Contents/MacOS
mkdir Rascal.app/Contents/Resources
mkdir Rascal.app/Contents/Resources/Libs
mkdir Rascal.app/Contents/Resources/Frameworks
mkdir Rascal.app/Contents/Frameworks
mkdir Rascal.app/Contents/Resources/Assets

cp macosx/Info.plist Rascal.app/Contents
cp macosx/PkgInfo Rascal.app/Contents
cp macosx/Rascal.icns Rascal.app/Contents/Resources
cp build/rascal_for_bundle Rascal.app/Contents/MacOS/Rascal

cp -r ./assets/ Rascal.app/Contents/Resources/Assets/
cp -r ./frameworks/ Rascal.app/Contents/Resources/Frameworks/
cp -r ./frameworks/ Rascal.app/Contents/Frameworks/

install_name_tool -change @executable_path/../Frameworks/SFML.framework/Versions/2.4.2/SFML @executable_path/../Resources/Frameworks/SFML.framework/Versions/2.4.2/SFML ./Rascal.app/Contents/MacOS/Rascal
install_name_tool -change @executable_path/../Frameworks/sfml-window.framework/Versions/2.4.2/sfml-window @executable_path/../Resources/Frameworks/sfml-window.framework/Versions/2.4.2/sfml-window ./Rascal.app/Contents/MacOS/Rascal
install_name_tool -change @executable_path/../Frameworks/sfml-graphics.framework/Versions/2.4.2/sfml-graphics @executable_path/../Resources/Frameworks/sfml-graphics.framework/Versions/2.4.2/sfml-graphics ./Rascal.app/Contents/MacOS/Rascal
install_name_tool -change @executable_path/../Frameworks/sfml-system.framework/Versions/2.4.2/sfml-system @executable_path/../Resources/Frameworks/sfml-system.framework/Versions/2.4.2/sfml-system ./Rascal.app/Contents/MacOS/Rascal
install_name_tool -change @executable_path/../Frameworks/sfml-audio.framework/Versions/2.4.2/sfml-audio @executable_path/../Resources/Frameworks/sfml-audio.framework/Versions/2.4.2/sfml-audio ./Rascal.app/Contents/MacOS/Rascal
