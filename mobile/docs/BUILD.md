# Abused Mobile - Build Instructions

Complete guide to building Abused for iOS and Android.

## Prerequisites

### Required Software

**All Platforms:**
- Node.js 18+ and npm
- Git

**Android:**
- Android Studio (latest stable)
- Android SDK API Level 33
- Android NDK r25.1.8937393
- CMake 3.18.1+
- Java 11+

**iOS (macOS only):**
- Xcode 14+
- CocoaPods
- Command Line Tools

### Environment Setup

**Android:**
```bash
# Set environment variables in ~/.bashrc or ~/.zshrc
export ANDROID_HOME=$HOME/Android/Sdk
export ANDROID_NDK_HOME=$ANDROID_HOME/ndk/25.1.8937393
export PATH=$PATH:$ANDROID_HOME/emulator
export PATH=$PATH:$ANDROID_HOME/platform-tools
```

**iOS:**
```bash
# Install CocoaPods
sudo gem install cocoapods

# Install Xcode Command Line Tools
xcode-select --install
```

## Project Setup

### 1. Clone Repository

```bash
git clone https://github.com/8bit-Games/abused.git
cd abused/mobile/AbusedMobile
```

### 2. Install Dependencies

```bash
# Install Node.js dependencies
npm install

# iOS only: Install pods
cd ios && pod install && cd ..
```

### 3. SDL2 Setup

Abused requires SDL2 and SDL2_mixer. You need to integrate these libraries for mobile.

#### Android SDL2 Setup

**Option A: Use Prebuilt AAR (Recommended)**

1. Download SDL2 for Android:
   - [SDL2-2.28.5.aar](https://github.com/libsdl-org/SDL/releases)
   - [SDL2_mixer-2.6.3.aar](https://github.com/libsdl-org/SDL_mixer/releases)

2. Place AAR files:
   ```bash
   mkdir -p android/app/libs
   cp SDL2-2.28.5.aar android/app/libs/
   cp SDL2_mixer-2.6.3.aar android/app/libs/
   ```

3. Update `android/app/build.gradle`:
   ```gradle
   dependencies {
       implementation files('libs/SDL2-2.28.5.aar')
       implementation files('libs/SDL2_mixer-2.6.3.aar')
   }
   ```

**Option B: Build from Source**

1. Clone SDL2 repositories into your project
2. Add as CMake subprojects
3. See [SDL2 Android README](https://github.com/libsdl-org/SDL/blob/main/docs/README-android.md)

#### iOS SDL2 Setup

**Option A: Use Prebuilt Frameworks (Recommended)**

1. Download SDL2 for iOS:
   - [SDL2.framework](https://github.com/libsdl-org/SDL/releases)
   - [SDL2_mixer.framework](https://github.com/libsdl-org/SDL_mixer/releases)

2. Add frameworks to Xcode project:
   ```
   1. Open ios/AbusedMobile.xcworkspace in Xcode
   2. Right-click project → Add Files
   3. Select SDL2.framework and SDL2_mixer.framework
   4. Check "Copy items if needed"
   5. Add to target AbusedMobile
   ```

**Option B: Via CocoaPods**

Update `ios/Podfile`:
```ruby
pod 'SDL2', '~> 2.28'
pod 'SDL2_mixer', '~> 2.6'
```

Then:
```bash
cd ios && pod install
```

### 4. Configure Asset Path

Edit `App.tsx` and set the correct asset path:

```typescript
// For Android (example):
const assetPath = '/data/data/com.abused.mobile/files/assets';

// For iOS (example):
const assetPath = NSBundle.mainBundle.resourcePath + '/assets';
```

Copy game data to appropriate mobile location (see Asset Management below).

## Building

### Android Build

#### Debug Build

```bash
# Start Metro bundler
npm start

# In another terminal, build and run
npm run android

# Or manually:
cd android
./gradlew assembleDebug
adb install app/build/outputs/apk/debug/app-debug.apk
```

#### Release Build

1. Generate signing key (first time only):
   ```bash
   keytool -genkeypair -v -storetype PKCS12 -keystore abused-release.keystore \
     -alias abused -keyalg RSA -keysize 2048 -validity 10000
   ```

2. Configure signing in `android/app/build.gradle`:
   ```gradle
   android {
       signingConfigs {
           release {
               storeFile file('abused-release.keystore')
               storePassword 'YOUR_PASSWORD'
               keyAlias 'abused'
               keyPassword 'YOUR_PASSWORD'
           }
       }
       buildTypes {
           release {
               signingConfig signingConfigs.release
           }
       }
   }
   ```

3. Build:
   ```bash
   cd android
   ./gradlew bundleRelease  # For Google Play (AAB)
   # Or
   ./gradlew assembleRelease  # For APK
   ```

4. Output:
   - AAB: `android/app/build/outputs/bundle/release/app-release.aab`
   - APK: `android/app/build/outputs/apk/release/app-release.apk`

### iOS Build

#### Debug Build

```bash
# Start Metro bundler
npm start

# In another terminal
npm run ios

# Or specify device
npm run ios --device "iPhone 14 Pro"
```

#### Release Build (App Store)

1. Open Xcode:
   ```bash
   open ios/AbusedMobile.xcworkspace
   ```

2. Configure signing:
   - Select project in navigator
   - Select "AbusedMobile" target
   - Go to "Signing & Capabilities"
   - Select your Team
   - Ensure "Automatically manage signing" is checked

3. Archive:
   - Menu: Product → Destination → Any iOS Device
   - Menu: Product → Archive
   - Wait for build to complete
   - Organizer window opens automatically

4. Distribute:
   - Select archive
   - Click "Distribute App"
   - Choose distribution method:
     - App Store Connect (for TestFlight/App Store)
     - Ad Hoc (for internal testing)
     - Enterprise (if you have enterprise account)

## Asset Management

### Packaging Game Assets

The C++ game engine expects assets in a specific directory structure:

```
assets/
├── art/          # Sprites
├── levels/       # Level files
├── lisp/         # Scripts
├── sfx/          # Sound effects (optional)
└── music/        # Music (optional)
```

### Android Assets

**Method 1: APK Assets** (smaller download)

1. Copy assets to `android/app/src/main/assets/`:
   ```bash
   cp -r ../../data/* android/app/src/main/assets/
   ```

2. Access in code:
   ```typescript
   const assetPath = 'file:///android_asset/';
   ```

**Method 2: External Storage** (larger games)

1. Bundle assets separately
2. Download on first launch
3. Store in app-specific directory:
   ```typescript
   const assetPath = RNFS.DocumentDirectoryPath + '/assets';
   ```

### iOS Assets

1. Add to Xcode project:
   - Open `ios/AbusedMobile.xcworkspace`
   - Right-click project → Add Files
   - Select `../../data` folder
   - Check "Create folder references"
   - Add to target

2. Access in code:
   ```typescript
   const assetPath = RNFS.MainBundlePath + '/data';
   ```

## Testing

### On Emulator/Simulator

**Android Emulator:**
```bash
# List available AVDs
emulator -list-avds

# Start emulator
emulator -avd Pixel_5_API_33

# Run app
npm run android
```

**iOS Simulator:**
```bash
# List available simulators
xcrun simctl list devices

# Run app (React Native CLI handles launching simulator)
npm run ios
```

### On Physical Device

**Android (USB Debugging):**

1. Enable Developer Options on device:
   - Settings → About Phone → Tap "Build Number" 7 times

2. Enable USB Debugging:
   - Settings → Developer Options → USB Debugging

3. Connect via USB and verify:
   ```bash
   adb devices
   # Should show your device
   ```

4. Run:
   ```bash
   npm run android
   ```

**iOS (USB Connection):**

1. Connect device via USB

2. Trust computer on device

3. In Xcode, select your device from device menu

4. Run:
   ```bash
   npm run ios --device
   # Or build via Xcode (Cmd+R)
   ```

## Troubleshooting

### Common Android Issues

**"SDK location not found":**
```bash
# Create local.properties
echo "sdk.dir=$ANDROID_HOME" > android/local.properties
```

**"NDK not found":**
```bash
# Install NDK via Android Studio
# Or manually:
sdkmanager "ndk;25.1.8937393"
```

**CMake build fails:**
```bash
# Check CMakeLists.txt paths are correct
# Ensure all source files exist
# Try clean build:
cd android && ./gradlew clean
```

**Native library not found:**
```bash
# Check System.loadLibrary("abused-engine") matches CMake library name
# Verify .so files in APK:
unzip -l app/build/outputs/apk/debug/app-debug.apk | grep libabused-engine.so
```

### Common iOS Issues

**"Module 'AbusedEngine' not found":**
```bash
cd ios && pod install
# Then rebuild in Xcode
```

**Undefined symbols:**
```
# Check all C++ files are included in Xcode project
# Verify bridging header includes all necessary headers
# Check framework search paths in Build Settings
```

**Code signing error:**
```
# Select correct development team in Xcode
# Ensure provisioning profile is valid
# Try automatic signing
```

### React Native Issues

**Metro bundler won't start:**
```bash
# Kill existing process
lsof -ti:8081 | xargs kill -9

# Clear cache and restart
npm start -- --reset-cache
```

**Red screen errors:**
```bash
# Clear watchman
watchman watch-del-all

# Clear React Native cache
rm -rf $TMPDIR/react-*

# Reinstall
rm -rf node_modules
npm install
```

## Performance Optimization

### Android

1. **Enable R8 shrinking** (already in build.gradle)
2. **Use ProGuard rules** for native code
3. **Enable ABI splits** to reduce APK size:
   ```gradle
   android {
       splits {
           abi {
               enable true
               reset()
               include "armeabi-v7a", "arm64-v8a"
               universalApk false
           }
       }
   }
   ```

### iOS

1. **Enable Bitcode** in Build Settings
2. **Optimize for size** (Release builds)
3. **Strip debug symbols** in Archive

### Both Platforms

1. **Reduce asset sizes** (compress sprites, audio)
2. **Lazy load assets** (load levels on demand)
3. **Profile with native tools**:
   - Android: Android Profiler
   - iOS: Instruments

## Continuous Integration

See `.github/workflows/` for CI/CD examples (if available).

Basic GitHub Actions workflow:

```yaml
name: Build Mobile

on: [push, pull_request]

jobs:
  android:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v3
      - uses: actions/setup-node@v3
        with:
          node-version: 18
      - run: npm install
      - run: cd android && ./gradlew assembleDebug

  ios:
    runs-on: macos-latest
    steps:
      - uses: actions/checkout@v3
      - uses: actions/setup-node@v3
        with:
          node-version: 18
      - run: npm install
      - run: cd ios && pod install
      - run: xcodebuild -workspace ios/AbusedMobile.xcworkspace -scheme AbusedMobile
```

## Next Steps

- [ ] Set up SDL2 libraries for your platform
- [ ] Copy game assets to mobile project
- [ ] Test on emulator/simulator
- [ ] Test on physical device
- [ ] Configure signing for release builds
- [ ] Optimize performance
- [ ] Submit to app stores

## Support

For issues or questions:
- GitHub Issues: https://github.com/8bit-Games/abused/issues
- Documentation: https://github.com/8bit-Games/abused/tree/main/mobile/docs

---

**Abused Mobile Build Guide** - v1.0.0
