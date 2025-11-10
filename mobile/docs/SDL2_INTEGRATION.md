# SDL2 Integration Guide for Abused Mobile

Complete guide to integrating SDL2 and SDL2_mixer into the Abused mobile project for both Android and iOS.

## Overview

Abused uses SDL2 for cross-platform graphics, input, and audio. The mobile version requires SDL2 2.28.5+ and SDL2_mixer 2.6.3+ configured specifically for mobile platforms.

## Android Integration

### Method 1: Automated Setup (Recommended)

Run the provided setup script:

```bash
cd mobile/AbusedMobile
./setup-sdl2-android.sh
```

This script will:
1. Download SDL2 2.28.5 source
2. Download SDL2_mixer 2.6.3 source
3. Extract to `android/app/jni/` directory
4. Configure Android.mk and Application.mk
5. Set up CMake integration

### Method 2: Manual Setup

If you prefer manual installation:

**Step 1: Download SDL2**
```bash
cd mobile/AbusedMobile/android/app/jni
curl -L https://github.com/libsdl-org/SDL/releases/download/release-2.28.5/SDL2-2.28.5.tar.gz -o SDL2.tar.gz
tar -xzf SDL2.tar.gz
mv SDL2-2.28.5 SDL2
rm SDL2.tar.gz
```

**Step 2: Download SDL2_mixer**
```bash
curl -L https://github.com/libsdl-org/SDL_mixer/releases/download/release-2.6.3/SDL2_mixer-2.6.3.tar.gz -o SDL2_mixer.tar.gz
tar -xzf SDL2_mixer.tar.gz
mv SDL2_mixer-2.6.3 SDL2_mixer
rm SDL2_mixer.tar.gz
```

**Step 3: Verify Directory Structure**
```
android/app/jni/
├── SDL2/
│   ├── include/
│   ├── src/
│   └── CMakeLists.txt
└── SDL2_mixer/
    ├── include/
    ├── src/
    └── CMakeLists.txt
```

### Android Build Configuration

The CMakeLists.txt is already configured to:
- Include SDL2 headers from `jni/SDL2/include`
- Include SDL2_mixer headers from `jni/SDL2_mixer/include`
- Build SDL2 and SDL2_mixer from source
- Link them to the abused-engine library

**Build Command:**
```bash
cd android
./gradlew assembleDebug
```

### Android Build Verification

After building, verify SDL2 libraries are included:

```bash
# List libraries in APK
unzip -l app/build/outputs/apk/debug/app-debug.apk | grep "\.so$"

# You should see:
# lib/arm64-v8a/libSDL2.so
# lib/arm64-v8a/libSDL2_mixer.so
# lib/arm64-v8a/libabused-engine.so
# (and similar for other ABIs)
```

## iOS Integration

### Method 1: Automated Setup (Recommended)

Run the provided setup script (macOS only):

```bash
cd mobile/AbusedMobile
./setup-sdl2-ios.sh
```

This script will:
1. Download SDL2 2.28.5 DMG
2. Download SDL2_mixer 2.6.3 DMG
3. Extract frameworks to `ios/Frameworks/` directory
4. Provide instructions for adding to Xcode

### Method 2: Manual Setup

**Step 1: Download Frameworks**

Download from official releases:
- [SDL2-2.28.5.dmg](https://github.com/libsdl-org/SDL/releases/download/release-2.28.5/SDL2-2.28.5.dmg)
- [SDL2_mixer-2.6.3.dmg](https://github.com/libsdl-org/SDL_mixer/releases/download/release-2.6.3/SDL2_mixer-2.6.3.dmg)

**Step 2: Extract Frameworks**

1. Mount each DMG
2. Copy `SDL2.framework` to `ios/Frameworks/`
3. Copy `SDL2_mixer.framework` to `ios/Frameworks/`

**Step 3: Add to Xcode Project**

1. Open `ios/AbusedMobile.xcworkspace` in Xcode
2. Select project in navigator
3. Select "AbusedMobile" target
4. Go to "General" tab
5. Under "Frameworks, Libraries, and Embedded Content":
   - Click "+" button
   - Click "Add Other..." → "Add Files..."
   - Navigate to `ios/Frameworks/`
   - Select `SDL2.framework`
   - Set "Embed" to "Embed & Sign"
   - Repeat for `SDL2_mixer.framework`

**Step 4: Verify Framework Search Paths**

1. Go to "Build Settings" tab
2. Search for "Framework Search Paths"
3. Add (if not present): `$(PROJECT_DIR)/Frameworks`

**Step 5: Update Info.plist (Optional)**

Add SDL2-specific keys if needed:
```xml
<key>SDL_IOS_ORIENTATIONS</key>
<string>LandscapeLeft LandscapeRight</string>
```

### iOS Build Verification

Build the project:
```bash
cd ios
xcodebuild -workspace AbusedMobile.xcworkspace -scheme AbusedMobile -configuration Debug
```

Or build in Xcode (Cmd+B).

Verify frameworks are embedded:
```bash
ls -la DerivedData/AbusedMobile-*/Build/Products/Debug-iphoneos/AbusedMobile.app/Frameworks/

# Should show:
# SDL2.framework/
# SDL2_mixer.framework/
```

## Mobile SDL Wrapper

The project includes `src/mobile/mobile_sdl.h/cpp` which provides:

### Initialization
```cpp
#include "mobile/mobile_sdl.h"

// Initialize SDL with video and audio
mobile_sdl::initialize(mobile_sdl::INIT_ALL);

// Create fullscreen window
mobile_sdl::create_window(1920, 1080, true);
```

### Platform Detection
```cpp
if (mobile_sdl::is_android()) {
    // Android-specific code
} else if (mobile_sdl::is_ios()) {
    // iOS-specific code
}

if (mobile_sdl::is_tablet()) {
    // Use tablet UI layout
}
```

### Asset Paths
```cpp
std::string asset_path = mobile_sdl::get_asset_path();
// Android: "" (use SDL_RWFromFile with "assets/" prefix)
// iOS: "/path/to/bundle/resources/"

std::string save_path = mobile_sdl::get_internal_storage_path();
// Android: "/data/data/com.abused.mobile/files/"
// iOS: "/path/to/Documents/"
```

### App Lifecycle
```cpp
// In your main loop
mobile_sdl::poll_events();  // Handles pause/resume automatically

// Manual control
mobile_sdl::on_pause();     // Called when app backgrounds
mobile_sdl::on_resume();    // Called when app foregrounds
```

## Usage in Game Engine

Update your game initialization code:

```cpp
#include "mobile/mobile_sdl.h"
#include "mobile/mobile_input.h"

int main(int argc, char* argv[]) {
    // Initialize mobile SDL
    if (!mobile_sdl::initialize()) {
        return 1;
    }

    // Create window
    if (!mobile_sdl::create_window(1920, 1080, true)) {
        mobile_sdl::shutdown();
        return 1;
    }

    // Initialize mobile input
    mobile_input::initialize();

    // Get asset path
    std::string assets = mobile_sdl::get_asset_path();

    // Create game instance
    game* g = new game(assets.c_str());

    // Main game loop
    bool running = true;
    while (running) {
        // Poll SDL events (handles lifecycle)
        mobile_sdl::poll_events();

        // Update game with mobile input
        if (g->first_view) {
            mobile_input::update(g->first_view);
        }

        // Update game logic
        g->update();

        // Render
        SDL_Renderer* renderer = mobile_sdl::get_renderer();
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // ... render game ...

        SDL_RenderPresent(renderer);
    }

    // Cleanup
    delete g;
    mobile_input::shutdown();
    mobile_sdl::destroy_window();
    mobile_sdl::shutdown();

    return 0;
}
```

## Troubleshooting

### Android Issues

**CMake can't find SDL2:**
```
Error: SDL2 not found at .../jni/SDL2
```
**Solution:** Run `./setup-sdl2-android.sh` to download SDL2

**Linker error: undefined reference to SDL_**:
```
Solution:** Ensure CMakeLists.txt links SDL2:
```cmake
target_link_libraries(abused-engine SDL2::SDL2 SDL2_mixer::SDL2_mixer)
```

**App crashes on launch:**
- Check logcat: `adb logcat | grep SDL`
- Verify all .so files are in APK
- Ensure SDL2 is initialized before use

**No audio:**
- Check AndroidManifest.xml has RECORD_AUDIO permission (if using mic)
- Verify SDL_mixer initialized: `Mix_Init(MIX_INIT_OGG | MIX_INIT_MP3)`

### iOS Issues

**Framework not found:**
```
ld: framework not found SDL2
```
**Solution:**
1. Verify `ios/Frameworks/SDL2.framework` exists
2. Check Framework Search Paths includes `$(PROJECT_DIR)/Frameworks`
3. Clean build (Product → Clean Build Folder)

**Dyld error: Library not loaded:**
```
dyld: Library not loaded: @rpath/SDL2.framework/SDL2
```
**Solution:** In Xcode target settings:
1. General → Frameworks section
2. Change from "Do Not Embed" to "Embed & Sign"

**Orientation issues:**
- Set `SDL_HINT_ORIENTATIONS` in mobile_sdl::initialize()
- Update Info.plist with supported orientations
- Restart app after changes

**Black screen on iOS:**
- Check SDL_CreateWindow succeeded
- Verify SDL_CreateRenderer succeeded
- Ensure SDL_RenderPresent() is called every frame

## Performance Tips

### Android
1. **Enable hardware acceleration:**
   ```cpp
   SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengles2");
   ```

2. **Reduce overdraw:**
   - Don't clear screen if rendering fills entire viewport
   - Use sprite batching

3. **Use correct ABI:**
   - arm64-v8a for modern devices (best performance)
   - armeabi-v7a for older devices

### iOS
1. **Use Metal rendering:**
   ```cpp
   SDL_SetHint(SDL_HINT_RENDER_DRIVER, "metal");
   ```

2. **Enable Retina support:**
   ```cpp
   mobile_sdl::create_window(width, height, true);
   // Window will automatically use @2x or @3x resolution
   ```

3. **Optimize for battery:**
   ```cpp
   mobile_sdl::enable_power_saving();  // Enables VSync
   ```

## Testing SDL2 Integration

### Quick Test Code

Add to your bridge initialization:

```cpp
bool test_sdl() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        LOGE("SDL_Init failed: %s", SDL_GetError());
        return false;
    }

    SDL_Window* window = SDL_CreateWindow(
        "SDL Test",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        800, 600,
        SDL_WINDOW_SHOWN
    );

    if (!window) {
        LOGE("SDL_CreateWindow failed: %s", SDL_GetError());
        SDL_Quit();
        return false;
    }

    LOGI("SDL2 initialized successfully!");
    SDL_DestroyWindow(window);
    SDL_Quit();
    return true;
}
```

### Expected Output

**Android (logcat):**
```
I/AbusedSDL: Initializing SDL for mobile platform
I/AbusedSDL: Audio initialized: 44100 Hz, stereo
I/AbusedSDL: SDL initialization complete
I/AbusedSDL: Creating window: 1920x1080, fullscreen=1
I/AbusedSDL: Window created: 1920x1080
I/AbusedSDL: Display: 2340x1080 @ 60Hz
```

**iOS (Console):**
```
Initializing SDL for mobile platform
Audio initialized: 44100 Hz, stereo
SDL initialization complete
Creating window: 2436x1125, fullscreen=1
Window created: 2436x1125
Display: 2436x1125 @ 60Hz
```

## Version Compatibility

| Component | Minimum Version | Recommended | Tested |
|-----------|----------------|-------------|--------|
| SDL2 | 2.0.0 | 2.28.5 | 2.28.5 |
| SDL2_mixer | 2.0.0 | 2.6.3 | 2.6.3 |
| Android NDK | r21 | r25 | r25.1.8937393 |
| iOS Deployment | 11.0 | 13.0 | 13.0+ |
| CMake | 3.10 | 3.18 | 3.22 |

## Additional Resources

- [SDL2 Documentation](https://wiki.libsdl.org/SDL2/)
- [SDL2 Android README](https://github.com/libsdl-org/SDL/blob/main/docs/README-android.md)
- [SDL2 iOS README](https://github.com/libsdl-org/SDL/blob/main/docs/README-ios.md)
- [SDL2_mixer Documentation](https://www.libsdl.org/projects/SDL_mixer/docs/)

## Next Steps

After integrating SDL2:

1. [ ] Run setup script for your platform
2. [ ] Build project and verify no errors
3. [ ] Test SDL2 initialization code
4. [ ] Copy game assets to appropriate directories
5. [ ] Update game engine to use mobile_sdl wrapper
6. [ ] Test on physical device

---

**Abused SDL2 Integration** - v1.0.0
