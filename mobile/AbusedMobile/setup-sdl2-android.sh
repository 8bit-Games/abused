#!/bin/bash

# SDL2 Setup Script for Android
# Downloads and configures SDL2 and SDL2_mixer for Android builds

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ANDROID_DIR="$SCRIPT_DIR/android"

SDL2_VERSION="2.28.5"
SDL2_MIXER_VERSION="2.6.3"

SDL2_URL="https://github.com/libsdl-org/SDL/releases/download/release-${SDL2_VERSION}/SDL2-${SDL2_VERSION}.tar.gz"
SDL2_MIXER_URL="https://github.com/libsdl-org/SDL_mixer/releases/download/release-${SDL2_MIXER_VERSION}/SDL2_mixer-${SDL2_MIXER_VERSION}.tar.gz"

echo "================================================"
echo "SDL2 Setup for Abused Mobile (Android)"
echo "================================================"
echo ""

# Create directories
mkdir -p "$ANDROID_DIR/app/jni"
mkdir -p "$ANDROID_DIR/app/src/main/jniLibs"

cd "$ANDROID_DIR/app/jni"

# Download SDL2
echo "Downloading SDL2 ${SDL2_VERSION}..."
if [ ! -d "SDL2-${SDL2_VERSION}" ]; then
    curl -L "$SDL2_URL" -o "SDL2-${SDL2_VERSION}.tar.gz"
    tar -xzf "SDL2-${SDL2_VERSION}.tar.gz"
    mv "SDL2-${SDL2_VERSION}" SDL2
    rm "SDL2-${SDL2_VERSION}.tar.gz"
    echo "✓ SDL2 downloaded and extracted"
else
    echo "✓ SDL2 already present"
fi

# Download SDL2_mixer
echo "Downloading SDL2_mixer ${SDL2_MIXER_VERSION}..."
if [ ! -d "SDL2_mixer-${SDL2_MIXER_VERSION}" ]; then
    curl -L "$SDL2_MIXER_URL" -o "SDL2_mixer-${SDL2_MIXER_VERSION}.tar.gz"
    tar -xzf "SDL2_mixer-${SDL2_MIXER_VERSION}.tar.gz"
    mv "SDL2_mixer-${SDL2_MIXER_VERSION}" SDL2_mixer
    rm "SDL2_mixer-${SDL2_MIXER_VERSION}.tar.gz"
    echo "✓ SDL2_mixer downloaded and extracted"
else
    echo "✓ SDL2_mixer already present"
fi

# Create Android.mk for SDL2
cat > "$ANDROID_DIR/app/jni/Android.mk" << 'EOF'
LOCAL_PATH := $(call my-dir)

# SDL2
include $(CLEAR_VARS)
LOCAL_MODULE := SDL2
LOCAL_SRC_FILES := SDL2/src/main/android/SDL_android_main.c
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/SDL2/include
include $(BUILD_STATIC_LIBRARY)

# SDL2_mixer
include $(CLEAR_VARS)
LOCAL_MODULE := SDL2_mixer
LOCAL_SRC_FILES := SDL2_mixer/src/*.c
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/SDL2_mixer/include
LOCAL_SHARED_LIBRARIES := SDL2
include $(BUILD_SHARED_LIBRARY)
EOF

# Create Application.mk
cat > "$ANDROID_DIR/app/jni/Application.mk" << 'EOF'
APP_ABI := armeabi-v7a arm64-v8a x86 x86_64
APP_PLATFORM := android-23
APP_STL := c++_shared
APP_CPPFLAGS := -std=c++14 -frtti -fexceptions
EOF

echo ""
echo "================================================"
echo "SDL2 Setup Complete!"
echo "================================================"
echo ""
echo "Next steps:"
echo "1. Build SDL2 native libraries:"
echo "   cd android && ./gradlew buildSDL2"
echo ""
echo "2. Build Abused:"
echo "   ./gradlew assembleDebug"
echo ""
