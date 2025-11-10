# Abused Native Bridge API

This document specifies the interface between the React Native UI layer and the C++ game engine.

## Architecture

```
React Native (JavaScript/TypeScript)
          ↕ [Native Module Bridge]
Native Layer (Java/Kotlin on Android, Objective-C++ on iOS)
          ↕ [JNI / C++ Direct Call]
C++ Game Engine (SDL2 + OpenGL ES)
```

## Bridge Components

### 1. React Native Native Module

**File**: `AbusedEngine.ts`

```typescript
// TypeScript interface for the native module
export interface IAbusedEngine {
  // Lifecycle
  initialize(assetPath: string): Promise<boolean>;
  shutdown(): void;
  pause(): void;
  resume(): void;

  // Game loop
  update(deltaTimeMs: number): void;

  // Rendering
  getRenderTexture(): number;  // Returns OpenGL texture ID
  setViewport(width: number, height: number): void;

  // Input
  setMovementInput(x: number, y: number): void;  // -1 to 1
  setAimPosition(screenX: number, screenY: number): void;
  setFireButton(pressed: boolean): void;
  setJumpButton(pressed: boolean): void;
  nextWeapon(): void;
  prevWeapon(): void;
  useSpecialAbility(): void;

  // Game state
  getGameState(): Promise<GameState>;
  loadLevel(levelName: string): Promise<boolean>;
  saveGame(slotId: number): Promise<boolean>;
  loadGame(slotId: number): Promise<boolean>;

  // Settings
  setVolume(music: number, sfx: number): void;  // 0.0 to 1.0
  setGraphicsQuality(quality: GraphicsQuality): void;

  // Events (callbacks)
  addEventListener(event: string, callback: (data: any) => void): void;
  removeEventListener(event: string, callback: (data: any) => void): void;
}

export enum GraphicsQuality {
  LOW = 0,
  MEDIUM = 1,
  HIGH = 2,
  ULTRA = 3
}

export interface GameState {
  health: number;
  maxHealth: number;
  currentWeapon: number;
  ammo: number;
  score: number;
  lives: number;
  level: string;
  isPaused: boolean;
  isGameOver: boolean;
}
```

### 2. Android Native Bridge

**File**: `android/app/src/main/java/com/abused/AbusedEngineModule.java`

```java
package com.abused;

import com.facebook.react.bridge.ReactApplicationContext;
import com.facebook.react.bridge.ReactContextBaseJavaModule;
import com.facebook.react.bridge.ReactMethod;
import com.facebook.react.bridge.Promise;

public class AbusedEngineModule extends ReactContextBaseJavaModule {
    static {
        // Load native C++ library
        System.loadLibrary("abused-engine");
    }

    public AbusedEngineModule(ReactApplicationContext context) {
        super(context);
    }

    @Override
    public String getName() {
        return "AbusedEngine";
    }

    // Native method declarations (implemented in C++)
    @ReactMethod
    public void initialize(String assetPath, Promise promise) {
        boolean success = nativeInitialize(assetPath);
        promise.resolve(success);
    }

    @ReactMethod
    public void setMovementInput(float x, float y) {
        nativeSetMovementInput(x, y);
    }

    @ReactMethod
    public void setAimPosition(float screenX, float screenY) {
        nativeSetAimPosition(screenX, screenY);
    }

    // ... other methods

    // Native JNI declarations
    private native boolean nativeInitialize(String assetPath);
    private native void nativeSetMovementInput(float x, float y);
    private native void nativeSetAimPosition(float x, float y);
    // ... other native methods
}
```

**File**: `android/app/src/main/cpp/abused-bridge.cpp`

```cpp
#include <jni.h>
#include <string>
#include "game.h"  // Main game header

// Global game instance
static game* g_game = nullptr;

extern "C" {

JNIEXPORT jboolean JNICALL
Java_com_abused_AbusedEngineModule_nativeInitialize(
    JNIEnv* env,
    jobject /* this */,
    jstring assetPath) {

    const char* path = env->GetStringUTFChars(assetPath, nullptr);

    // Initialize SDL and game engine
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        env->ReleaseStringUTFChars(assetPath, path);
        return JNI_FALSE;
    }

    // Create game instance
    g_game = new game(path);
    bool success = g_game->initialize();

    env->ReleaseStringUTFChars(assetPath, path);
    return success ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT void JNICALL
Java_com_abused_AbusedEngineModule_nativeSetMovementInput(
    JNIEnv* /* env */,
    jobject /* this */,
    jfloat x,
    jfloat y) {

    if (g_game && g_game->current_view) {
        // Convert joystick input to player movement
        g_game->current_view->set_movement_input(x, y);
    }
}

JNIEXPORT void JNICALL
Java_com_abused_AbusedEngineModule_nativeSetAimPosition(
    JNIEnv* /* env */,
    jobject /* this */,
    jfloat screenX,
    jfloat screenY) {

    if (g_game && g_game->current_view) {
        // Convert screen coordinates to world coordinates
        g_game->current_view->set_aim_position(screenX, screenY);
    }
}

// ... other JNI implementations

} // extern "C"
```

**CMake Configuration**: `android/app/CMakeLists.txt`

```cmake
cmake_minimum_required(VERSION 3.18.1)

project(abused-engine)

# Add the game engine source directory
set(ABUSED_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../../src)

# Include directories
include_directories(
    ${ABUSED_SRC_DIR}
    ${ABUSED_SRC_DIR}/lisp
    ${ABUSED_SRC_DIR}/imlib
    ${ABUSED_SRC_DIR}/net
    ${ABUSED_SRC_DIR}/sdlport
)

# Add all game engine source files
file(GLOB_RECURSE ENGINE_SOURCES
    ${ABUSED_SRC_DIR}/*.cpp
    ${ABUSED_SRC_DIR}/lisp/*.cpp
    ${ABUSED_SRC_DIR}/imlib/*.cpp
    ${ABUSED_SRC_DIR}/net/*.cpp
    ${ABUSED_SRC_DIR}/sdlport/*.cpp
)

# Add bridge code
set(BRIDGE_SOURCES
    src/main/cpp/abused-bridge.cpp
)

# Create shared library
add_library(abused-engine SHARED
    ${ENGINE_SOURCES}
    ${BRIDGE_SOURCES}
)

# Find and link SDL2
find_library(SDL2_LIBRARY SDL2)
find_library(SDL2_MIXER_LIBRARY SDL2_mixer)
find_library(GLES_LIBRARY GLESv2)
find_library(LOG_LIBRARY log)

target_link_libraries(abused-engine
    ${SDL2_LIBRARY}
    ${SDL2_MIXER_LIBRARY}
    ${GLES_LIBRARY}
    ${LOG_LIBRARY}
    android
)
```

### 3. iOS Native Bridge

**File**: `ios/AbusedMobile/Bridge/AbusedEngine.h`

```objc
#import <React/RCTBridgeModule.h>

@interface AbusedEngine : NSObject <RCTBridgeModule>

@end
```

**File**: `ios/AbusedMobile/Bridge/AbusedEngine.mm` (Objective-C++)

```objc++
#import "AbusedEngine.h"
#import "game.h"  // C++ game header

// Global game instance
static game* g_game = nullptr;

@implementation AbusedEngine

RCT_EXPORT_MODULE();

RCT_EXPORT_METHOD(initialize:(NSString *)assetPath
                  resolver:(RCTPromiseResolveBlock)resolve
                  rejecter:(RCTPromiseRejectBlock)reject) {

    const char* path = [assetPath UTF8String];

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        reject(@"init_failed", @"SDL initialization failed", nil);
        return;
    }

    // Create game instance
    g_game = new game(path);
    bool success = g_game->initialize();

    if (success) {
        resolve(@YES);
    } else {
        reject(@"init_failed", @"Game initialization failed", nil);
    }
}

RCT_EXPORT_METHOD(setMovementInput:(float)x y:(float)y) {
    if (g_game && g_game->current_view) {
        g_game->current_view->set_movement_input(x, y);
    }
}

RCT_EXPORT_METHOD(setAimPosition:(float)screenX y:(float)screenY) {
    if (g_game && g_game->current_view) {
        g_game->current_view->set_aim_position(screenX, screenY);
    }
}

// ... other method exports

@end
```

## C++ Engine Modifications

### Add Mobile Input Interface

**File**: `src/mobile/mobile_input.h`

```cpp
#ifndef ABUSED_MOBILE_INPUT_H
#define ABUSED_MOBILE_INPUT_H

#include "view.h"

class mobile_input {
public:
    // Touch input
    static void set_movement_input(float x, float y);
    static void set_aim_position(float screen_x, float screen_y);
    static void set_fire_button(bool pressed);
    static void set_jump_button(bool pressed);

    // Virtual buttons
    static void next_weapon();
    static void prev_weapon();
    static void use_special_ability();

    // Update (called every frame)
    static void update(view* current_view);

private:
    static float joystick_x;
    static float joystick_y;
    static float aim_screen_x;
    static float aim_screen_y;
    static bool fire_pressed;
    static bool jump_pressed;
};

#endif
```

**File**: `src/mobile/mobile_input.cpp`

```cpp
#include "mobile_input.h"
#include "game.h"

// Static member initialization
float mobile_input::joystick_x = 0.0f;
float mobile_input::joystick_y = 0.0f;
float mobile_input::aim_screen_x = 0.0f;
float mobile_input::aim_screen_y = 0.0f;
bool mobile_input::fire_pressed = false;
bool mobile_input::jump_pressed = false;

void mobile_input::set_movement_input(float x, float y) {
    joystick_x = clamp(x, -1.0f, 1.0f);
    joystick_y = clamp(y, -1.0f, 1.0f);
}

void mobile_input::set_aim_position(float screen_x, float screen_y) {
    aim_screen_x = screen_x;
    aim_screen_y = screen_y;
}

void mobile_input::set_fire_button(bool pressed) {
    fire_pressed = pressed;
}

void mobile_input::set_jump_button(bool pressed) {
    jump_pressed = pressed;
}

void mobile_input::next_weapon() {
    if (the_game && the_game->player_list) {
        the_game->player_list->next_weapon();
    }
}

void mobile_input::prev_weapon() {
    if (the_game && the_game->player_list) {
        the_game->player_list->prev_weapon();
    }
}

void mobile_input::use_special_ability() {
    if (the_game && the_game->player_list) {
        the_game->player_list->use_special();
    }
}

void mobile_input::update(view* current_view) {
    if (!current_view || !current_view->focus) {
        return;
    }

    game_object* player = current_view->focus;

    // Apply movement input
    if (fabs(joystick_x) > 0.1f) {
        player->set_horizontal_velocity(joystick_x * player->max_speed());
    } else {
        player->set_horizontal_velocity(0);
    }

    // Handle jump
    if (joystick_y < -0.5f || jump_pressed) {
        player->try_jump();
    }

    // Handle crouch/use
    if (joystick_y > 0.5f) {
        player->try_use();
    }

    // Convert screen aim to world coordinates
    int world_x, world_y;
    current_view->screen_to_world(
        (int)aim_screen_x,
        (int)aim_screen_y,
        world_x,
        world_y
    );

    // Update player aim angle
    player->aim_at(world_x, world_y);

    // Handle firing
    if (fire_pressed) {
        player->fire_weapon();
    }
}
```

## Event System (Game → React Native)

Send events from C++ to JavaScript:

**Events**:
- `onHealthChanged`: Player health updated
- `onWeaponChanged`: Player switched weapon
- `onLevelComplete`: Level finished
- `onGameOver`: Player died
- `onScoreChanged`: Score updated

**Android (JNI)**:

```cpp
#include <jni.h>

void send_event_to_js(const char* event_name, const char* json_data) {
    JNIEnv* env = /* get JNI env */;

    jclass module_class = env->FindClass("com/abused/AbusedEngineModule");
    jmethodID send_event = env->GetMethodID(
        module_class,
        "sendEventToJS",
        "(Ljava/lang/String;Ljava/lang/String;)V"
    );

    jstring j_event = env->NewStringUTF(event_name);
    jstring j_data = env->NewStringUTF(json_data);

    env->CallVoidMethod(/* module instance */, send_event, j_event, j_data);
}
```

**iOS (Objective-C++)**:

```objc++
#import <React/RCTEventEmitter.h>

@interface AbusedEngine : RCTEventEmitter <RCTBridgeModule>
@end

@implementation AbusedEngine

- (NSArray<NSString *> *)supportedEvents {
    return @[@"onHealthChanged", @"onWeaponChanged", @"onLevelComplete"];
}

void send_event_to_js(const char* event_name, const char* json_data) {
    // Get module instance and send event
    [[AbusedEngine sharedInstance] sendEventWithName:@(event_name)
                                              body:@(json_data)];
}

@end
```

## Testing

### Unit Tests (React Native)

```typescript
import { AbusedEngine } from './AbusedEngine';

describe('AbusedEngine', () => {
  it('should initialize successfully', async () => {
    const result = await AbusedEngine.initialize('/path/to/assets');
    expect(result).toBe(true);
  });

  it('should handle movement input', () => {
    AbusedEngine.setMovementInput(1.0, 0.0);
    // Verify player moves right
  });
});
```

### Integration Tests (Native)

**Android**: Use Android Instrumentation Tests
**iOS**: Use XCTest

## Performance Considerations

- **Input Latency**: Process touch events on main thread, < 16ms
- **Render Thread**: C++ rendering on separate thread
- **Memory**: Share OpenGL textures between React Native and C++
- **Battery**: Target 60 FPS, use vsync, minimize overdraw

## Troubleshooting

### Android
- **JNI crashes**: Check method signatures match exactly
- **Library not found**: Verify `System.loadLibrary("abused-engine")`
- **CMake errors**: Ensure NDK version matches

### iOS
- **Undefined symbols**: Check Objective-C++ bridging header
- **Module not found**: Verify module is in `RCTBridgeModule` registry
- **C++ exceptions**: Wrap in try-catch, convert to Objective-C errors

## Next Steps

1. [ ] Implement Android JNI bridge
2. [ ] Implement iOS Objective-C++ bridge
3. [ ] Add mobile input handler to C++ engine
4. [ ] Test touch input → game response latency
5. [ ] Implement event system (C++ → JavaScript)
6. [ ] Performance profiling

---

**Abused Bridge API** - Connecting mobile UI to game engine
