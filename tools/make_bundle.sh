#!/bin/sh

# Generate iconset
rm -rf macosx/Rascal.iconset
mkdir macosx/Rascal.iconset
sips -z 16 16     macosx/RascalIcon.png --out macosx/Rascal.iconset/icon_16x16.png
sips -z 32 32     macosx/RascalIcon.png --out macosx/Rascal.iconset/icon_16x16@2x.png
sips -z 32 32     macosx/RascalIcon.png --out macosx/Rascal.iconset/icon_32x32.png
sips -z 64 64     macosx/RascalIcon.png --out macosx/Rascal.iconset/icon_32x32@2x.png
sips -z 128 128   macosx/RascalIcon.png --out macosx/Rascal.iconset/icon_128x128.png
sips -z 256 256   macosx/RascalIcon.png --out macosx/Rascal.iconset/icon_128x128@2x.png
sips -z 256 256   macosx/RascalIcon.png --out macosx/Rascal.iconset/icon_256x256.png
sips -z 512 512   macosx/RascalIcon.png --out macosx/Rascal.iconset/icon_256x256@2x.png
sips -z 512 512   macosx/RascalIcon.png --out macosx/Rascal.iconset/icon_512x512.png
cp macosx/RascalIcon.png macosx/Rascal.iconset/icon_512x512@2x.png
iconutil -c icns -o macosx/Rascal.icns macosx/Rascal.iconset
rm -r macosx/Rascal.iconset

# Delete previous version
rm -rf Rascal.app

# Build directory structure
mkdir Rascal.app
mkdir Rascal.app/Contents
mkdir Rascal.app/Contents/MacOS
mkdir Rascal.app/Contents/Resources
mkdir Rascal.app/Contents/Resources/Libs
mkdir Rascal.app/Contents/Resources/Frameworks
mkdir Rascal.app/Contents/Frameworks
mkdir Rascal.app/Contents/Resources/Assets

# Copy bundle-required metadata
cp macosx/Info.plist Rascal.app/Contents
cp macosx/PkgInfo Rascal.app/Contents
cp macosx/Rascal.icns Rascal.app/Contents/Resources

# Copy executable
cp build/rascal_for_bundle Rascal.app/Contents/MacOS/Rascal


# Bundle assets and dependencies
cp -r ./assets/ Rascal.app/Contents/Resources/Assets/
cp -r ./frameworks/ Rascal.app/Contents/Resources/Frameworks/
cp -r ./frameworks/ Rascal.app/Contents/Frameworks/

# Fix rpath
../mac-third-party-libs-tool/third_party_libs_tool ./Rascal.app

# Zip bundle
zip -qr rascal.zip Rascal.app/ -x "*.DS_Store"
