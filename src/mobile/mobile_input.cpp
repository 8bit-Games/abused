/*
 * Mobile Input Handler Implementation
 */

#include "mobile_input.h"
#include "game.h"
#include "view.h"
#include "objects.h"
#include <cmath>
#include <algorithm>

namespace mobile_input {

// Global input state
static InputState g_input_state;

// Constants
static const float JOYSTICK_DEADZONE = 0.1f;
static const float CROUCH_THRESHOLD = 0.5f;
static const float JUMP_THRESHOLD = -0.5f;

// Helper function to clamp values
template<typename T>
static inline T clamp(T value, T min, T max) {
    return std::max(min, std::min(max, value));
}

void initialize() {
    g_input_state = InputState();
}

void shutdown() {
    clear_input();
}

void set_movement_input(float x, float y) {
    g_input_state.joystick_x = clamp(x, -1.0f, 1.0f);
    g_input_state.joystick_y = clamp(y, -1.0f, 1.0f);
}

void set_aim_position(float screen_x, float screen_y) {
    g_input_state.aim_screen_x = screen_x;
    g_input_state.aim_screen_y = screen_y;
}

void set_fire_button(bool pressed) {
    g_input_state.fire_pressed = pressed;
}

void set_jump_button(bool pressed) {
    g_input_state.jump_pressed = pressed;
}

void set_special_button(bool pressed) {
    g_input_state.special_pressed = pressed;
}

void next_weapon() {
    extern game *the_game;
    if (the_game && the_game->first_view && the_game->first_view->focus) {
        // Send next weapon command to player
        the_game->first_view->focus->next_weapon();
    }
}

void prev_weapon() {
    extern game *the_game;
    if (the_game && the_game->first_view && the_game->first_view->focus) {
        // Send prev weapon command to player
        the_game->first_view->focus->prev_weapon();
    }
}

const InputState& get_input_state() {
    return g_input_state;
}

void clear_input() {
    g_input_state = InputState();
}

void update(view* current_view) {
    if (!current_view || !current_view->focus) {
        return;
    }

    extern game *the_game;
    if (!the_game) {
        return;
    }

    game_object* player = current_view->focus;

    // === Handle Movement ===

    float joy_x = g_input_state.joystick_x;
    float joy_y = g_input_state.joystick_y;

    // Apply deadzone
    if (std::abs(joy_x) < JOYSTICK_DEADZONE) {
        joy_x = 0.0f;
    }
    if (std::abs(joy_y) < JOYSTICK_DEADZONE) {
        joy_y = 0.0f;
    }

    // Horizontal movement
    if (joy_x != 0.0f) {
        // Convert normalized input to actual velocity
        // Positive x = move right, Negative x = move left
        int direction = (joy_x > 0.0f) ? 1 : -1;
        float speed_multiplier = std::abs(joy_x);

        // Set player direction and try to move
        if (direction > 0) {
            player->set_aistate(running_forward);
        } else {
            player->set_aistate(running_backward);
        }

        // Apply movement - the game object handles actual physics
        player->try_move(direction * (int)(speed_multiplier * 10), 0, 0);
    } else {
        // No horizontal input - stop
        player->set_aistate(stopped);
    }

    // Vertical movement (jump/crouch)
    if (joy_y < JUMP_THRESHOLD || g_input_state.jump_pressed) {
        // Jump
        player->try_move(0, -1, 1); // Jump command
    } else if (joy_y > CROUCH_THRESHOLD) {
        // Crouch or activate object below
        player->try_move(0, 1, 0); // Crouch/use command
    }

    // === Handle Aiming ===

    if (g_input_state.aim_screen_x != 0.0f || g_input_state.aim_screen_y != 0.0f) {
        // Convert screen coordinates to world coordinates
        int view_width = current_view->suggest.cx2 - current_view->suggest.cx1 + 1;
        int view_height = current_view->suggest.cy2 - current_view->suggest.cy1 + 1;

        // Calculate world position from screen position
        int world_x = current_view->xoff() + (int)g_input_state.aim_screen_x;
        int world_y = current_view->yoff() + (int)g_input_state.aim_screen_y;

        // Calculate angle from player to aim point
        int player_x = player->x >> 16;
        int player_y = player->y >> 16;

        int dx = world_x - player_x;
        int dy = world_y - player_y;

        // Calculate angle in degrees (0-359)
        float angle_rad = std::atan2((float)dy, (float)dx);
        int angle_deg = (int)(angle_rad * 180.0f / 3.14159265f);
        if (angle_deg < 0) angle_deg += 360;

        // Set player's aim angle
        player->set_angle(angle_deg);
    }

    // === Handle Firing ===

    if (g_input_state.fire_pressed) {
        // Fire weapon
        player->fire();
    }

    // === Handle Special Ability ===

    if (g_input_state.special_pressed) {
        // Activate special ability
        player->do_special();
        // Clear special button to prevent continuous activation
        g_input_state.special_pressed = false;
    }
}

} // namespace mobile_input
