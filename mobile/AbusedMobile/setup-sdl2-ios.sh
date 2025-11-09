#!/bin/bash

# SDL2 Setup Script for iOS
# Downloads and configures SDL2 and SDL2_mixer frameworks for iOS builds

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
IOS_DIR="$SCRIPT_DIR/ios"

SDL2_VERSION="2.28.5"
SDL2_MIXER_VERSION="2.6.3"

# Using prebuilt frameworks for iOS
SDL2_IOS_URL="https://github.com/libsdl-org/SDL/releases/download/release-${SDL2_VERSION}/SDL2-${SDL2_VERSION}.dmg"
SDL2_MIXER_IOS_URL="https://github.com/libsdl-org/SDL_mixer/releases/download/release-${SDL2_MIXER_VERSION}/SDL2_mixer-${SDL2_MIXER_VERSION}.dmg"

echo "================================================"
echo "SDL2 Setup for Abused Mobile (iOS)"
echo "================================================"
echo ""

# Create frameworks directory
mkdir -p "$IOS_DIR/Frameworks"

cd "$IOS_DIR/Frameworks"

echo "Downloading SDL2 ${SDL2_VERSION} for iOS..."
if [ ! -d "SDL2.framework" ]; then
    # Download and extract SDL2
    curl -L "$SDL2_IOS_URL" -o "SDL2.dmg"

    # Mount DMG
    hdiutil attach SDL2.dmg -mountpoint /Volumes/SDL2

    # Copy framework
    cp -R "/Volumes/SDL2/SDL2.framework" .

    # Unmount and cleanup
    hdiutil detach /Volumes/SDL2
    rm SDL2.dmg

    echo "✓ SDL2 framework installed"
else
    echo "✓ SDL2 framework already present"
fi

echo "Downloading SDL2_mixer ${SDL2_MIXER_VERSION} for iOS..."
if [ ! -d "SDL2_mixer.framework" ]; then
    # Download and extract SDL2_mixer
    curl -L "$SDL2_MIXER_IOS_URL" -o "SDL2_mixer.dmg"

    # Mount DMG
    hdiutil attach SDL2_mixer.dmg -mountpoint /Volumes/SDL2_mixer

    # Copy framework
    cp -R "/Volumes/SDL2_mixer/SDL2_mixer.framework" .

    # Unmount and cleanup
    hdiutil detach /Volumes/SDL2_mixer
    rm SDL2_mixer.dmg

    echo "✓ SDL2_mixer framework installed"
else
    echo "✓ SDL2_mixer framework already present"
fi

echo ""
echo "================================================"
echo "SDL2 Setup Complete!"
echo "================================================"
echo ""
echo "Next steps:"
echo "1. Open Xcode workspace:"
echo "   open ios/AbusedMobile.xcworkspace"
echo ""
echo "2. Add frameworks to Xcode project:"
echo "   - Select project in navigator"
echo "   - Go to 'Frameworks, Libraries, and Embedded Content'"
echo "   - Click '+' and add:"
echo "     • SDL2.framework (from ios/Frameworks/)"
echo "     • SDL2_mixer.framework (from ios/Frameworks/)"
echo "   - Set both to 'Embed & Sign'"
echo ""
echo "3. Update Framework Search Paths (if needed):"
echo "   Build Settings → Framework Search Paths"
echo "   Add: \$(PROJECT_DIR)/Frameworks"
echo ""
