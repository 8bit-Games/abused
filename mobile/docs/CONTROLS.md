# Abused Mobile Touch Controls Specification

## Control Scheme: Virtual Joystick (Option A)

This document specifies the touch control interface for Abused mobile.

## Screen Layout

```
┌─────────────────────────────────────────────┐
│                                             │
│  [❮ Prev]                      [Next ❯]     │  ← Weapon Switch
│                                             │
│           ╔═══════════════════╗             │
│           ║                   ║             │
│           ║   GAME VIEWPORT   ║             │
│           ║                   ║             │
│           ║                   ║             │
│           ╚═══════════════════╝             │
│                                             │
│                                    [🎯]     │  ← Special Ability
│    ╭───╮                                    │
│    │ ↑ │                           [JUMP]  │
│  ╭─┼───┼─╮                                 │
│  │←│ ● │→│         [TAP TO AIM/FIRE]       │  ← Aim Zone
│  ╰─┼───┼─╯                                 │
│    │ ↓ │                                   │
│    ╰───╯                                   │
│   Movement                                  │
└─────────────────────────────────────────────┘
```

## Control Zones

### 1. Movement Joystick (Bottom-Left)

**Position**:
- Origin: 20% from left, 80% from top
- Radius: 80px (outer), 40px (inner knob)
- Zone: Left 40% of screen, bottom 30%

**Behavior**:
- Touch anywhere in zone to activate
- Joystick appears at touch point
- Drag to move character
- Release to center (character stops)
- Visual feedback: Semi-transparent circle with inner knob

**Input Mapping**:
```
Touch Delta (x, y) → Normalized (-1 to 1, -1 to 1)
│
├─ X-axis: Left/Right movement
│   -1.0 = Full left
│    0.0 = No horizontal movement
│   +1.0 = Full right
│
└─ Y-axis: Up/Down (jump/crouch)
   -1.0 = Jump (up)
    0.0 = Normal
   +1.0 = Crouch/Use (down)
```

**Code Interface**:
```cpp
void AbusedEngine::setMovementInput(float x, float y) {
    // x, y in range [-1.0, 1.0]
    player_horizontal_velocity = x * MAX_SPEED;
    if (y < -0.5) player_jump();
    if (y > 0.5) player_crouch();
}
```

### 2. Aim Zone (Right Side)

**Position**:
- Right 50% of screen
- Full height
- Excludes button areas

**Behavior**:
- **Tap**: Fire weapon at tap position
- **Drag**: Continuous aiming, auto-fire while touching
- **Multi-touch**: Hold position to aim, second finger fires
- Visual feedback: Crosshair appears at aim position

**Input Mapping**:
```
Screen Coordinates (screenX, screenY) → World Coordinates
│
├─ Convert to viewport coordinates
├─ Transform to world space
└─ Calculate angle from player position

Angle = atan2(worldY - playerY, worldX - playerX)
```

**Code Interface**:
```cpp
void AbusedEngine::setAimPosition(float screenX, float screenY) {
    // Screen coordinates → World coordinates
    vec2 worldPos = screenToWorld(screenX, screenY);
    player_aim_at(worldPos.x, worldPos.y);
}

void AbusedEngine::setFireButton(bool pressed) {
    if (pressed) player_fire();
}
```

### 3. Jump Button (Bottom-Right)

**Position**:
- 85% from left, 75% from top
- Size: 70x70px
- Circle shape

**Behavior**:
- Tap to jump
- Hold for higher jump (if game supports)
- Visual: Semi-transparent circle with "⬆" icon

**Code Interface**:
```cpp
void AbusedEngine::onJumpPressed() {
    player_jump();
}

void AbusedEngine::onJumpReleased() {
    player_jump_released();
}
```

### 4. Weapon Switch Buttons (Top)

**Position**:
- **Prev Weapon**: 10% from left, 5% from top
- **Next Weapon**: 90% from left, 5% from top
- Size: 60x50px each

**Behavior**:
- Tap to cycle weapons
- Shows current weapon icon
- Cooldown: 250ms between switches

**Code Interface**:
```cpp
void AbusedEngine::nextWeapon() {
    player_next_weapon();
}

void AbusedEngine::prevWeapon() {
    player_prev_weapon();
}
```

### 5. Special Ability Button (Right Side)

**Position**:
- 85% from left, 50% from top
- Size: 60x60px
- Circle shape

**Behavior**:
- Tap to activate special ability
- Shows cooldown timer
- Visual: Icon changes based on current ability

**Code Interface**:
```cpp
void AbusedEngine::useSpecialAbility() {
    player_special_ability();
}
```

## Touch Event Handling

### Multi-Touch Support

The game supports **simultaneous 3-finger touch**:

1. **Primary Touch**: Movement joystick OR aim
2. **Secondary Touch**: Fire button OR aim (if primary is movement)
3. **Tertiary Touch**: Jump/Special buttons

**Priority System**:
```
Touch Priority:
1. UI Buttons (Jump, Weapon Switch, Special) - Highest
2. Movement Joystick Zone (left side)
3. Aim Zone (right side)
4. Fallback: Ignored
```

### Touch State Machine

```
IDLE
  ├─ Touch in Movement Zone → MOVEMENT_ACTIVE
  ├─ Touch in Aim Zone → AIMING_ACTIVE
  └─ Touch on Button → BUTTON_PRESSED

MOVEMENT_ACTIVE
  ├─ Move within zone → Update joystick
  ├─ Release → IDLE
  └─ Second touch in Aim Zone → MOVEMENT_AND_AIMING

AIMING_ACTIVE
  ├─ Move → Update aim position + Auto-fire
  ├─ Release → IDLE
  └─ Second touch in Movement Zone → MOVEMENT_AND_AIMING

MOVEMENT_AND_AIMING
  ├─ Either touch released → MOVEMENT_ACTIVE or AIMING_ACTIVE
  └─ Both released → IDLE

BUTTON_PRESSED
  ├─ Release in button → Execute action → IDLE
  └─ Release outside → Cancel → IDLE
```

## Visual Feedback

### Movement Joystick
```typescript
<View style={styles.joystickOuter}>
  <Animated.View style={[styles.joystickInner, {
    transform: [
      { translateX: joystickX },
      { translateY: joystickY }
    ]
  }]} />
</View>

styles.joystickOuter = {
  width: 160,
  height: 160,
  borderRadius: 80,
  backgroundColor: 'rgba(255, 255, 255, 0.2)',
  borderWidth: 2,
  borderColor: 'rgba(255, 255, 255, 0.4)',
}

styles.joystickInner = {
  width: 80,
  height: 80,
  borderRadius: 40,
  backgroundColor: 'rgba(255, 255, 255, 0.6)',
}
```

### Aim Crosshair
```typescript
<Animated.View style={[styles.crosshair, {
  left: aimX,
  top: aimY,
}]}>
  <View style={styles.crosshairH} />
  <View style={styles.crosshairV} />
</Animated.View>

styles.crosshair = {
  position: 'absolute',
  width: 40,
  height: 40,
}

styles.crosshairH = {
  position: 'absolute',
  width: 40,
  height: 2,
  backgroundColor: 'red',
  top: 19,
}

styles.crosshairV = {
  position: 'absolute',
  width: 2,
  height: 40,
  backgroundColor: 'red',
  left: 19,
}
```

### Buttons
- **Default**: 50% opacity white
- **Pressed**: 80% opacity with scale(0.95)
- **Disabled**: 20% opacity gray

## Haptic Feedback

| Action | Haptic Type | Platforms |
|--------|-------------|-----------|
| Fire weapon | Light impact | iOS, Android 10+ |
| Weapon switch | Selection | iOS, Android 10+ |
| Jump | Medium impact | iOS, Android 10+ |
| Special ability | Heavy impact | iOS, Android 10+ |
| Take damage | Heavy impact x2 | iOS, Android 10+ |

**Code**:
```typescript
import { Vibration } from 'react-native';

// Light impact
Vibration.vibrate(10);

// Heavy impact
Vibration.vibrate([0, 50, 20, 50]);
```

## Accessibility

### Options

1. **Joystick Size**: 80% / 100% / 120% / 150%
2. **Button Size**: 80% / 100% / 120% / 150%
3. **Joystick Opacity**: 30% / 50% / 70% / 90%
4. **Button Opacity**: 30% / 50% / 70% / 90%
5. **Haptic Feedback**: On / Off
6. **Auto-Fire**: On (hold to fire) / Off (tap to fire)
7. **Aim Assist**: Off / Low / Medium / High

### Color Blind Modes
- **Deuteranopia**: Green → Blue
- **Protanopia**: Red → Orange
- **Tritanopia**: Blue → Purple

## Performance Requirements

- **Touch Latency**: < 16ms (60 FPS)
- **Visual Feedback**: < 8ms
- **Haptic Delay**: < 10ms
- **Joystick Update Rate**: 60 Hz
- **Aim Update Rate**: 60 Hz

## Testing Checklist

- [ ] Joystick responds to touch in full zone
- [ ] Joystick returns to center on release
- [ ] Aim tracks finger position accurately
- [ ] Multi-touch works (move + aim simultaneously)
- [ ] Buttons don't interfere with joystick/aim zones
- [ ] Haptic feedback triggers correctly
- [ ] Works on 4.7" to 6.7" screens
- [ ] Works in landscape orientation
- [ ] No input lag on 60 FPS devices
- [ ] Touch zones don't overlap incorrectly

## Future Enhancements

- [ ] Customizable button positions
- [ ] Gesture controls (swipe to switch weapons)
- [ ] Controller support (Bluetooth gamepad)
- [ ] Tilt controls (accelerometer aiming)
- [ ] Voice commands (accessibility)

---

**Note**: All measurements are in logical pixels. Adapt to physical screen size using PixelRatio and Dimensions APIs.
