# Abused - Dark Sci-Fi Mobile Shooter

A reimagined mobile-first dark sci-fi side-scrolling shooter, built on the classic Abuse engine with modern mobile enhancements.

## Project Overview

**Abused** is a hybrid mobile game combining a high-performance C++ game engine with a modern mobile UI layer. Featuring intense run-and-gun action, mouse/touch-aimed shooting mechanics, and a dark sci-fi atmosphere.

### Key Features

- **Mobile-First Design** - Optimized for iOS and Android with touch controls
- **Virtual Joystick Controls** - Left joystick for movement, tap-to-aim on right side
- **Hybrid Architecture** - C++ engine with React Native UI layer
- **Classic Gameplay** - Skill-based platformer shooter mechanics
- **Lisp Scripting** - Extensible AI and game logic system
- **Multiplayer Ready** - Client/server networking built-in
- **Level Editor** - Create and share custom levels

## Architecture

```
┌──────────────────────────────────────────┐
│  React Native UI Layer (JS/TypeScript)   │
│  • Touch controls (virtual joystick)     │
│  • Mobile-optimized menus & HUD          │
│  • Settings, level selection             │
└──────────────┬───────────────────────────┘
               │ Native Bridge
               │ (JNI on Android, Obj-C++ on iOS)
┌──────────────┴───────────────────────────┐
│  C++ Game Engine (SDL2 + OpenGL ES)      │
│  • Core gameplay engine                  │
│  • Rendering pipeline                    │
│  • Lisp scripting system                 │
│  • Physics & collision detection         │
│  • Audio system                          │
└──────────────────────────────────────────┘
```

## Mobile Controls (Option A - Virtual Joystick)

- **Left Side**: Virtual D-pad for character movement
- **Right Side**: Drag/tap to aim, tap to shoot
- **Floating Buttons**:
  - Jump button
  - Weapon switch buttons
  - Special ability button

## Requirements

### Desktop Development (for testing)
- SDL2 2.0.3 or above
- SDL_mixer 2.0.0 or above
- CMake 3.16+

### Mobile Development
- **Android**: Android Studio, NDK r25+, CMake
- **iOS**: Xcode 14+, CocoaPods
- **React Native**: Node.js 18+, React Native 0.72+

## Project Structure

```
abused/
├── src/               # C++ game engine
│   ├── lisp/          # Embedded Lisp interpreter
│   ├── imlib/         # Image/UI library
│   ├── net/           # Networking layer
│   ├── sdlport/       # SDL2 platform abstraction
│   └── mobile/        # Mobile-specific C++ code (NEW)
├── mobile/            # Mobile app projects (NEW)
│   ├── AbusedMobile/  # React Native project
│   │   ├── android/   # Android native code
│   │   ├── ios/       # iOS native code
│   │   └── src/       # React Native components
│   └── docs/          # Mobile development documentation
├── data/              # Game assets
│   ├── art/           # Sprites
│   ├── levels/        # Level files
│   └── lisp/          # Game scripts
└── doc/               # Documentation
```

## Building

### Desktop (for development/testing)
```bash
mkdir build && cd build
cmake ..
make
./abused
```

### Mobile
See [mobile/README.md](mobile/README.md) for detailed mobile build instructions.

## Development Status

- [x] Core engine rebrand to "Abused"
- [x] Architecture planning
- [ ] Mobile touch control implementation
- [ ] React Native bridge
- [ ] Android build configuration
- [ ] iOS build configuration
- [ ] Performance optimization for mobile
- [ ] Touch control UI/UX polish

## Version

Current Version: **1.0.0** (Mobile Development Branch)

## Credits

- **Original Abuse**: Crack dot Com (1995)
- **SDL Port**: Sam Hocevar and contributors
- **Mobile Reimagining**: 8bit-Games

## License

Original Abuse code is public domain. Mobile enhancements and modifications are licensed under MIT License.

## Feedback

For issues, features, or questions, please use the GitHub issue tracker.

---

**Abused** - Reimagined for Mobile
