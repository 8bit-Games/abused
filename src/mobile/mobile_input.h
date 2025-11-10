/*
 * Mobile Input Handler for Abused
 * Translates touch input from React Native to game engine commands
 */

#ifndef ABUSED_MOBILE_INPUT_H
#define ABUSED_MOBILE_INPUT_H

class view;
class game_object;

namespace mobile_input {

// Touch input state
struct InputState {
    // Movement joystick (-1.0 to 1.0)
    float joystick_x;
    float joystick_y;

    // Aim position (screen coordinates)
    float aim_screen_x;
    float aim_screen_y;

    // Button states
    bool fire_pressed;
    bool jump_pressed;
    bool special_pressed;

    InputState() :
        joystick_x(0.0f),
        joystick_y(0.0f),
        aim_screen_x(0.0f),
        aim_screen_y(0.0f),
        fire_pressed(false),
        jump_pressed(false),
        special_pressed(false) {}
};

// Initialize mobile input system
void initialize();

// Shutdown mobile input system
void shutdown();

// Movement joystick input (x, y range: -1.0 to 1.0)
void set_movement_input(float x, float y);

// Aim position input (screen coordinates)
void set_aim_position(float screen_x, float screen_y);

// Button inputs
void set_fire_button(bool pressed);
void set_jump_button(bool pressed);
void set_special_button(bool pressed);

// Weapon switching
void next_weapon();
void prev_weapon();

// Get current input state (for debugging)
const InputState& get_input_state();

// Update game based on current input (called every frame)
void update(view* current_view);

// Clear all input (useful for pausing)
void clear_input();

} // namespace mobile_input

#endif // ABUSED_MOBILE_INPUT_H
