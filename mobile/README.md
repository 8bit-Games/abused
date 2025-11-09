# Abused Mobile Development

This directory contains the React Native mobile application and native bridge code for iOS and Android.

## Architecture Overview

The mobile version uses a hybrid architecture:

1. **React Native Layer** (JavaScript/TypeScript)
   - Touch controls UI
   - Mobile menus and HUD
   - Settings and configuration
   - Level selection

2. **Native Bridge** (C++/Objective-C++/JNI)
   - Bridges React Native to C++ game engine
   - Handles touch input translation
   - Manages game lifecycle

3. **C++ Game Engine** (../src/)
   - Core game logic (unchanged from desktop)
   - Rendering via SDL2/OpenGL ES
   - Physics, collision, AI

## Directory Structure

```
mobile/
├── AbusedMobile/          # React Native project
│   ├── android/           # Android-specific code
│   │   ├── app/
│   │   │   ├── src/main/
│   │   │   │   ├── cpp/   # Android JNI bridge
│   │   │   │   └── java/  # Android Java code
│   │   │   └── CMakeLists.txt  # Native build config
│   │   └── build.gradle
│   ├── ios/               # iOS-specific code
│   │   ├── AbusedMobile/
│   │   │   ├── Bridge/    # Objective-C++ bridge
│   │   │   └── Info.plist
│   │   └── Podfile
│   ├── src/               # React Native components
│   │   ├── components/
│   │   │   ├── VirtualJoystick.tsx
│   │   │   ├── TouchAim.tsx
│   │   │   ├── GameHUD.tsx
│   │   │   └── MobileMenu.tsx
│   │   ├── screens/
│   │   └── utils/
│   ├── package.json
│   └── App.tsx
├── docs/
│   ├── ARCHITECTURE.md    # Detailed architecture
│   ├── BRIDGE_API.md      # Native bridge API
│   ├── CONTROLS.md        # Touch control design
│   └── BUILD.md           # Build instructions
└── README.md              # This file
```

## Setup Instructions

### Prerequisites

1. **Node.js and npm**
   ```bash
   # Install Node.js 18+ and npm
   node --version  # Should be 18.x or higher
   npm --version
   ```

2. **React Native CLI**
   ```bash
   npm install -g react-native-cli
   ```

3. **Android Development** (for Android builds)
   - Android Studio (latest stable)
   - Android SDK API 33+
   - Android NDK r25+
   - Add to PATH: `ANDROID_HOME`, `ANDROID_NDK_HOME`

4. **iOS Development** (for iOS builds, macOS only)
   - Xcode 14+
   - CocoaPods: `sudo gem install cocoapods`
   - Command Line Tools: `xcode-select --install`

### Initial Setup

1. **Initialize React Native Project**
   ```bash
   cd mobile
   npx react-native@latest init AbusedMobile --skip-install
   cd AbusedMobile
   npm install
   ```

2. **Link Native Dependencies**
   ```bash
   # iOS
   cd ios && pod install && cd ..

   # Android (automatic via gradle)
   ```

3. **Configure Native Bridge**
   See [docs/BRIDGE_API.md](docs/BRIDGE_API.md) for setting up the C++ bridge.

## Building

### Android

```bash
cd AbusedMobile

# Debug build
npm run android

# Release build
cd android
./gradlew assembleRelease
```

Output: `android/app/build/outputs/apk/release/app-release.apk`

### iOS

```bash
cd AbusedMobile

# Debug build
npm run ios

# Release build (Xcode)
# 1. Open ios/AbusedMobile.xcworkspace in Xcode
# 2. Select "Generic iOS Device"
# 3. Product > Archive
```

## Running on Device

### Android
```bash
# Enable USB debugging on your Android device
adb devices  # Verify device is connected
npm run android
```

### iOS
```bash
# Connect iOS device via USB
# Trust the computer on your device
npm run ios --device
```

## Touch Controls

The mobile version implements **Virtual Joystick (Option A)**:

- **Left Joystick**: Character movement (up/down/left/right)
- **Right Side Tap Zone**: Aim and shoot
  - Drag to aim
  - Tap to fire
- **Floating Buttons**:
  - Jump (bottom-right)
  - Next Weapon (top-right)
  - Prev Weapon (top-right)
  - Special Ability (bottom-right)

See [docs/CONTROLS.md](docs/CONTROLS.md) for detailed control specifications.

## Development Workflow

1. **Start Metro Bundler**
   ```bash
   cd AbusedMobile
   npm start
   ```

2. **Run on Emulator/Device** (separate terminal)
   ```bash
   npm run android  # or npm run ios
   ```

3. **Hot Reload**
   - Shake device or press `Cmd+D` (iOS) / `Cmd+M` (Android)
   - Select "Enable Hot Reloading"

## Native Bridge API

The C++ game engine is exposed to React Native via a native module:

```typescript
import { AbusedEngine } from './native/AbusedEngine';

// Initialize engine
AbusedEngine.initialize(assetPath);

// Update game state (called every frame)
AbusedEngine.update(deltaTime);

// Handle touch input
AbusedEngine.setJoystickInput(x, y);  // -1 to 1
AbusedEngine.setAimPosition(screenX, screenY);
AbusedEngine.setFireButton(pressed);

// Render to texture (displayed in React Native)
const textureId = AbusedEngine.getRenderTexture();
```

See [docs/BRIDGE_API.md](docs/BRIDGE_API.md) for complete API reference.

## Performance Optimization

- C++ engine runs at 60 FPS
- React Native UI layer runs at 60 FPS (separate thread)
- Rendering uses OpenGL ES texture sharing
- Touch input processed with < 16ms latency

## Debugging

### React Native Debugger
```bash
# Install
npm install -g react-devtools

# Run
react-devtools
```

### Native Debugging

**Android (logcat)**:
```bash
adb logcat | grep Abused
```

**iOS (Xcode)**:
1. Open `ios/AbusedMobile.xcworkspace`
2. Set breakpoints in `.mm` files
3. Run via Xcode (Cmd+R)

## Troubleshooting

### Android NDK Build Fails
- Verify `ANDROID_NDK_HOME` is set
- Check `android/app/build.gradle` NDK version
- Clean: `cd android && ./gradlew clean`

### iOS CocoaPods Issues
```bash
cd ios
pod deintegrate
pod install
```

### Metro Bundler Port Conflict
```bash
# Kill existing process on port 8081
lsof -ti:8081 | xargs kill -9
npm start
```

## Next Steps

1. [ ] Initialize React Native project
2. [ ] Implement virtual joystick component
3. [ ] Create native bridge (JNI for Android, Obj-C++ for iOS)
4. [ ] Integrate C++ game engine
5. [ ] Test on physical devices
6. [ ] Performance profiling and optimization

## Resources

- [React Native Docs](https://reactnative.dev/docs/getting-started)
- [Android NDK](https://developer.android.com/ndk/guides)
- [iOS Native Modules](https://reactnative.dev/docs/native-modules-ios)
- [SDL2 Mobile](https://wiki.libsdl.org/SDL2/README/android)

---

**Abused Mobile** - Bringing dark sci-fi action to your pocket
