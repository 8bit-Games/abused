/*
 * Mobile SDL2 Implementation
 */

#include "mobile_sdl.h"
#include <SDL2/SDL_mixer.h>

#ifdef __ANDROID__
    #include <android/log.h>
    #define LOG_TAG "AbusedSDL"
    #define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
    #define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#elif defined(__APPLE__)
    #include <TargetConditionals.h>
    #if TARGET_OS_IOS
        #include <Foundation/Foundation.h>
        #define LOGI(...) NSLog(@__VA_ARGS__)
        #define LOGE(...) NSLog(@"ERROR: " __VA_ARGS__)
    #endif
#else
    #define LOGI(...) printf(__VA_ARGS__)
    #define LOGE(...) fprintf(stderr, __VA_ARGS__)
#endif

namespace mobile_sdl {

// Global state
static SDL_Window* g_window = nullptr;
static SDL_Renderer* g_renderer = nullptr;
static bool g_initialized = false;
static bool g_paused = false;

bool initialize(int flags) {
    if (g_initialized) {
        LOGI("SDL already initialized");
        return true;
    }

    LOGI("Initializing SDL for mobile platform");

    // Convert custom flags to SDL flags
    Uint32 sdl_flags = 0;
    if (flags & INIT_VIDEO) sdl_flags |= SDL_INIT_VIDEO;
    if (flags & INIT_AUDIO) sdl_flags |= SDL_INIT_AUDIO;
    if (flags & INIT_GAMECONTROLLER) sdl_flags |= SDL_INIT_GAMECONTROLLER;

    // Initialize SDL
    if (SDL_Init(sdl_flags) < 0) {
        LOGE("SDL_Init failed: %s", SDL_GetError());
        return false;
    }

    // Initialize SDL_mixer for audio
    if (flags & INIT_AUDIO) {
        int mixer_flags = MIX_INIT_OGG | MIX_INIT_MP3;
        if (Mix_Init(mixer_flags) != mixer_flags) {
            LOGE("Mix_Init failed: %s", Mix_GetError());
            // Continue anyway, audio is not critical
        }

        // Open audio device
        if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
            LOGE("Mix_OpenAudio failed: %s", Mix_GetError());
        } else {
            LOGI("Audio initialized: 44100 Hz, stereo");
        }
    }

    // Mobile-specific hints
    SDL_SetHint(SDL_HINT_ORIENTATIONS, "LandscapeLeft LandscapeRight");
    SDL_SetHint(SDL_HINT_ACCELEROMETER_AS_JOYSTICK, "0");
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"); // Linear filtering

#ifdef __ANDROID__
    // Android-specific hints
    SDL_SetHint(SDL_HINT_ANDROID_SEPARATE_MOUSE_AND_TOUCH, "1");
    SDL_SetHint(SDL_HINT_ANDROID_TRAP_BACK_BUTTON, "1");
#endif

#if TARGET_OS_IOS
    // iOS-specific hints
    SDL_SetHint(SDL_HINT_IOS_HIDE_HOME_INDICATOR, "2"); // Auto-hide
#endif

    g_initialized = true;
    LOGI("SDL initialization complete");
    return true;
}

void shutdown() {
    if (!g_initialized) {
        return;
    }

    LOGI("Shutting down SDL");

    destroy_window();

    Mix_CloseAudio();
    Mix_Quit();
    SDL_Quit();

    g_initialized = false;
    LOGI("SDL shutdown complete");
}

bool create_window(int width, int height, bool fullscreen) {
    if (!g_initialized) {
        LOGE("SDL not initialized");
        return false;
    }

    if (g_window) {
        LOGI("Window already created");
        return true;
    }

    LOGI("Creating window: %dx%d, fullscreen=%d", width, height, fullscreen);

    Uint32 window_flags = SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL;

    if (fullscreen) {
        window_flags |= SDL_WINDOW_FULLSCREEN;
    }

    // Allow high DPI
    window_flags |= SDL_WINDOW_ALLOW_HIGHDPI;

    // Create window
    g_window = SDL_CreateWindow(
        "Abused",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        width,
        height,
        window_flags
    );

    if (!g_window) {
        LOGE("SDL_CreateWindow failed: %s", SDL_GetError());
        return false;
    }

    // Create renderer with hardware acceleration
    g_renderer = SDL_CreateRenderer(
        g_window,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );

    if (!g_renderer) {
        LOGE("SDL_CreateRenderer failed: %s", SDL_GetError());
        SDL_DestroyWindow(g_window);
        g_window = nullptr;
        return false;
    }

    // Enable alpha blending
    SDL_SetRenderDrawBlendMode(g_renderer, SDL_BLENDMODE_BLEND);

    // Get actual window size (may differ from requested)
    int actual_width, actual_height;
    SDL_GetWindowSize(g_window, &actual_width, &actual_height);
    LOGI("Window created: %dx%d", actual_width, actual_height);

    // Get display info
    SDL_DisplayMode mode;
    if (SDL_GetCurrentDisplayMode(0, &mode) == 0) {
        LOGI("Display: %dx%d @ %dHz", mode.w, mode.h, mode.refresh_rate);
    }

    return true;
}

void destroy_window() {
    if (g_renderer) {
        SDL_DestroyRenderer(g_renderer);
        g_renderer = nullptr;
    }

    if (g_window) {
        SDL_DestroyWindow(g_window);
        g_window = nullptr;
    }
}

SDL_Window* get_window() {
    return g_window;
}

SDL_Renderer* get_renderer() {
    return g_renderer;
}

void get_screen_size(int* width, int* height) {
    if (g_window) {
        SDL_GetWindowSize(g_window, width, height);
    } else {
        SDL_DisplayMode mode;
        if (SDL_GetCurrentDisplayMode(0, &mode) == 0) {
            *width = mode.w;
            *height = mode.h;
        } else {
            *width = 1920; // Default
            *height = 1080;
        }
    }
}

void get_display_dpi(float* ddpi, float* hdpi, float* vdpi) {
    if (SDL_GetDisplayDPI(0, ddpi, hdpi, vdpi) != 0) {
        LOGE("SDL_GetDisplayDPI failed: %s", SDL_GetError());
        if (ddpi) *ddpi = 160.0f; // Default Android DPI
        if (hdpi) *hdpi = 160.0f;
        if (vdpi) *vdpi = 160.0f;
    }
}

void set_orientation(Orientation orientation) {
#ifdef __ANDROID__
    const char* hint_value = nullptr;
    switch (orientation) {
        case LANDSCAPE_LEFT:
            hint_value = "LandscapeLeft";
            break;
        case LANDSCAPE_RIGHT:
            hint_value = "LandscapeRight";
            break;
        case PORTRAIT:
            hint_value = "Portrait";
            break;
        case PORTRAIT_UPSIDE_DOWN:
            hint_value = "PortraitUpsideDown";
            break;
        case SENSOR_LANDSCAPE:
            hint_value = "LandscapeLeft LandscapeRight";
            break;
        case SENSOR_PORTRAIT:
            hint_value = "Portrait PortraitUpsideDown";
            break;
    }
    if (hint_value) {
        SDL_SetHint(SDL_HINT_ORIENTATIONS, hint_value);
    }
#elif TARGET_OS_IOS
    // iOS orientation is typically handled by Info.plist
    // but can be changed programmatically via UIKit
    LOGI("Orientation change requested (iOS requires UIKit integration)");
#endif
}

void on_pause() {
    LOGI("Application paused");
    g_paused = true;

    // Pause audio
    Mix_PauseMusic();
    Mix_Pause(-1); // Pause all channels

    // Clear input state
    // (handled by mobile_input::clear_input())
}

void on_resume() {
    LOGI("Application resumed");
    g_paused = false;

    // Resume audio
    Mix_ResumeMusic();
    Mix_Resume(-1); // Resume all channels
}

void on_low_memory() {
    LOGI("Low memory warning received");

    // Clear any caches
    // This should be handled by the game's cache system
}

void poll_events() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_APP_TERMINATING:
                LOGI("App terminating");
                break;

            case SDL_APP_LOWMEMORY:
                on_low_memory();
                break;

            case SDL_APP_WILLENTERBACKGROUND:
                LOGI("App will enter background");
                on_pause();
                break;

            case SDL_APP_DIDENTERBACKGROUND:
                LOGI("App did enter background");
                break;

            case SDL_APP_WILLENTERFOREGROUND:
                LOGI("App will enter foreground");
                break;

            case SDL_APP_DIDENTERFOREGROUND:
                LOGI("App did enter foreground");
                on_resume();
                break;

            case SDL_QUIT:
                LOGI("Quit requested");
                break;
        }
    }
}

std::string get_asset_path() {
#ifdef __ANDROID__
    // Android assets are in the APK
    return "";  // Use SDL_RWFromFile with asset: prefix
#elif TARGET_OS_IOS
    // iOS assets in bundle
    NSString* resourcePath = [[NSBundle mainBundle] resourcePath];
    return std::string([resourcePath UTF8String]) + "/";
#else
    return "./";
#endif
}

std::string get_internal_storage_path() {
#ifdef __ANDROID__
    const char* path = SDL_AndroidGetInternalStoragePath();
    return path ? std::string(path) + "/" : "";
#elif TARGET_OS_IOS
    NSArray* paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString* documentsDirectory = [paths objectAtIndex:0];
    return std::string([documentsDirectory UTF8String]) + "/";
#else
    return SDL_GetPrefPath("8bit-Games", "Abused");
#endif
}

std::string get_external_storage_path() {
#ifdef __ANDROID__
    const char* path = SDL_AndroidGetExternalStoragePath();
    return path ? std::string(path) + "/" : "";
#else
    return get_internal_storage_path(); // iOS doesn't have external storage
#endif
}

bool is_android() {
#ifdef __ANDROID__
    return true;
#else
    return false;
#endif
}

bool is_ios() {
#if TARGET_OS_IOS
    return true;
#else
    return false;
#endif
}

bool is_tablet() {
#ifdef __ANDROID__
    // Check if device is a tablet (screen size >= 7 inches)
    float ddpi;
    get_display_dpi(&ddpi, nullptr, nullptr);

    int width, height;
    get_screen_size(&width, &height);

    // Calculate diagonal in inches
    float diagonal = sqrtf(width * width + height * height) / ddpi;
    return diagonal >= 7.0f;
#elif TARGET_OS_IOS
    // Check if iPad
    return UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad;
#else
    return false;
#endif
}

void set_performance_mode(bool high_performance) {
    if (high_performance) {
        SDL_SetHint(SDL_HINT_RENDER_VSYNC, "0");
        LOGI("Performance mode: HIGH (vsync off)");
    } else {
        SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1");
        LOGI("Performance mode: BALANCED (vsync on)");
    }
}

void enable_power_saving() {
    SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1");
    // Additional power saving could be implemented here
    LOGI("Power saving enabled");
}

void vibrate(int duration_ms) {
#ifdef __ANDROID__
    // Use SDL's haptic if available, otherwise use Android JNI
    // For now, just log
    LOGI("Vibrate: %d ms", duration_ms);
    // TODO: Implement via JNI call to Vibrator service
#elif TARGET_OS_IOS
    // Use UIKit haptics
    // TODO: Implement via Objective-C
    LOGI("Vibrate: %d ms (iOS)", duration_ms);
#endif
}

} // namespace mobile_sdl
