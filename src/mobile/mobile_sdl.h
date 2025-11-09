/*
 * Mobile SDL2 Initialization and Management
 * Handles platform-specific SDL2 setup for iOS and Android
 */

#ifndef ABUSED_MOBILE_SDL_H
#define ABUSED_MOBILE_SDL_H

#include <SDL2/SDL.h>
#include <string>

namespace mobile_sdl {

// Initialization flags
enum InitFlags {
    INIT_VIDEO = 0x01,
    INIT_AUDIO = 0x02,
    INIT_GAMECONTROLLER = 0x04,
    INIT_ALL = INIT_VIDEO | INIT_AUDIO | INIT_GAMECONTROLLER
};

// Screen orientation
enum Orientation {
    LANDSCAPE_LEFT,
    LANDSCAPE_RIGHT,
    PORTRAIT,
    PORTRAIT_UPSIDE_DOWN,
    SENSOR_LANDSCAPE,
    SENSOR_PORTRAIT
};

// Initialize SDL2 for mobile
// Returns true on success, false on failure
bool initialize(int flags = INIT_ALL);

// Shutdown SDL2
void shutdown();

// Create mobile-optimized window and renderer
// width, height: requested size (may be adjusted for device)
// fullscreen: true to use native resolution
bool create_window(int width, int height, bool fullscreen = true);

// Destroy window and renderer
void destroy_window();

// Get current window
SDL_Window* get_window();

// Get current renderer
SDL_Renderer* get_renderer();

// Get actual screen dimensions
void get_screen_size(int* width, int* height);

// Get display DPI
void get_display_dpi(float* ddpi, float* hdpi, float* vdpi);

// Set screen orientation (mobile only)
void set_orientation(Orientation orientation);

// Pause/resume (called by app lifecycle)
void on_pause();
void on_resume();

// Low memory warning (called by OS)
void on_low_memory();

// Handle app lifecycle events
void poll_events();

// Asset path helpers
std::string get_asset_path();
std::string get_internal_storage_path();
std::string get_external_storage_path();

// Platform detection
bool is_android();
bool is_ios();
bool is_tablet();

// Performance hints
void set_performance_mode(bool high_performance);
void enable_power_saving();

// Haptic feedback (uses SDL or native APIs)
void vibrate(int duration_ms);

} // namespace mobile_sdl

#endif // ABUSED_MOBILE_SDL_H
