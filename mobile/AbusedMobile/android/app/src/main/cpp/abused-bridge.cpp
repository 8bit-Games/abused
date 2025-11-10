/*
 * Android JNI Bridge for Abused Game Engine
 * Connects React Native to C++ game engine via JNI
 */

#include <jni.h>
#include <string>
#include <android/log.h>

// Include game engine headers
#include "game.h"
#include "mobile/mobile_input.h"
#include "view.h"

// Logging macros
#define LOG_TAG "AbusedEngine"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

// Global game instance
static game* g_game = nullptr;
static bool g_initialized = false;

extern "C" {

// ===== Lifecycle Methods =====

JNIEXPORT jboolean JNICALL
Java_com_abused_AbusedEngineModule_nativeInitialize(
    JNIEnv* env,
    jobject /* this */,
    jstring assetPath) {

    if (g_initialized) {
        LOGI("Engine already initialized");
        return JNI_TRUE;
    }

    const char* path = env->GetStringUTFChars(assetPath, nullptr);
    LOGI("Initializing Abused engine with asset path: %s", path);

    try {
        // Initialize mobile input system
        mobile_input::initialize();

        // Initialize SDL
        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
            LOGE("SDL initialization failed: %s", SDL_GetError());
            env->ReleaseStringUTFChars(assetPath, path);
            return JNI_FALSE;
        }

        // Create game instance
        // Note: Actual game initialization will depend on your game class constructor
        // This is a placeholder - adjust based on your actual game initialization
        g_game = new game();

        if (!g_game) {
            LOGE("Failed to create game instance");
            SDL_Quit();
            env->ReleaseStringUTFChars(assetPath, path);
            return JNI_FALSE;
        }

        // Set asset path for loading resources
        // TODO: Configure game to load assets from this path

        g_initialized = true;
        LOGI("Engine initialized successfully");

    } catch (const std::exception& e) {
        LOGE("Exception during initialization: %s", e.what());
        env->ReleaseStringUTFChars(assetPath, path);
        return JNI_FALSE;
    }

    env->ReleaseStringUTFChars(assetPath, path);
    return JNI_TRUE;
}

JNIEXPORT void JNICALL
Java_com_abused_AbusedEngineModule_nativeShutdown(
    JNIEnv* /* env */,
    jobject /* this */) {

    if (!g_initialized) {
        return;
    }

    LOGI("Shutting down engine");

    mobile_input::shutdown();

    if (g_game) {
        delete g_game;
        g_game = nullptr;
    }

    SDL_Quit();
    g_initialized = false;

    LOGI("Engine shutdown complete");
}

JNIEXPORT void JNICALL
Java_com_abused_AbusedEngineModule_nativePause(
    JNIEnv* /* env */,
    jobject /* this */) {

    LOGI("Pausing game");
    mobile_input::clear_input();

    if (g_game) {
        // TODO: Call game pause method
        // g_game->pause();
    }
}

JNIEXPORT void JNICALL
Java_com_abused_AbusedEngineModule_nativeResume(
    JNIEnv* /* env */,
    jobject /* this */) {

    LOGI("Resuming game");

    if (g_game) {
        // TODO: Call game resume method
        // g_game->resume();
    }
}

// ===== Game Loop =====

JNIEXPORT void JNICALL
Java_com_abused_AbusedEngineModule_nativeUpdate(
    JNIEnv* /* env */,
    jobject /* this */,
    jfloat deltaTimeMs) {

    if (!g_initialized || !g_game) {
        return;
    }

    // Update mobile input
    if (g_game->first_view) {
        mobile_input::update(g_game->first_view);
    }

    // Update game logic
    // TODO: Call your actual game update method
    // g_game->update(deltaTimeMs);
}

// ===== Rendering =====

JNIEXPORT jint JNICALL
Java_com_abused_AbusedEngineModule_nativeGetRenderTexture(
    JNIEnv* /* env */,
    jobject /* this */) {

    if (!g_initialized || !g_game) {
        return 0;
    }

    // TODO: Return OpenGL texture ID
    // This will be the framebuffer texture that React Native can display
    return 0; // Placeholder
}

JNIEXPORT void JNICALL
Java_com_abused_AbusedEngineModule_nativeSetViewport(
    JNIEnv* /* env */,
    jobject /* this */,
    jint width,
    jint height) {

    LOGI("Setting viewport: %dx%d", width, height);

    if (!g_initialized || !g_game) {
        return;
    }

    // TODO: Update game viewport
    // if (g_game->first_view) {
    //     g_game->first_view->resize(width, height);
    // }
}

// ===== Input Methods =====

JNIEXPORT void JNICALL
Java_com_abused_AbusedEngineModule_nativeSetMovementInput(
    JNIEnv* /* env */,
    jobject /* this */,
    jfloat x,
    jfloat y) {

    mobile_input::set_movement_input(x, y);
}

JNIEXPORT void JNICALL
Java_com_abused_AbusedEngineModule_nativeSetAimPosition(
    JNIEnv* /* env */,
    jobject /* this */,
    jfloat screenX,
    jfloat screenY) {

    mobile_input::set_aim_position(screenX, screenY);
}

JNIEXPORT void JNICALL
Java_com_abused_AbusedEngineModule_nativeSetFireButton(
    JNIEnv* /* env */,
    jobject /* this */,
    jboolean pressed) {

    mobile_input::set_fire_button(pressed == JNI_TRUE);
}

JNIEXPORT void JNICALL
Java_com_abused_AbusedEngineModule_nativeSetJumpButton(
    JNIEnv* /* env */,
    jobject /* this */,
    jboolean pressed) {

    mobile_input::set_jump_button(pressed == JNI_TRUE);
}

JNIEXPORT void JNICALL
Java_com_abused_AbusedEngineModule_nativeNextWeapon(
    JNIEnv* /* env */,
    jobject /* this */) {

    LOGI("Next weapon");
    mobile_input::next_weapon();
}

JNIEXPORT void JNICALL
Java_com_abused_AbusedEngineModule_nativePrevWeapon(
    JNIEnv* /* env */,
    jobject /* this */) {

    LOGI("Previous weapon");
    mobile_input::prev_weapon();
}

JNIEXPORT void JNICALL
Java_com_abused_AbusedEngineModule_nativeUseSpecialAbility(
    JNIEnv* /* env */,
    jobject /* this */) {

    LOGI("Special ability");
    mobile_input::set_special_button(true);
}

// ===== Game State =====

JNIEXPORT jstring JNICALL
Java_com_abused_AbusedEngineModule_nativeGetGameState(
    JNIEnv* env,
    jobject /* this */) {

    if (!g_initialized || !g_game || !g_game->first_view || !g_game->first_view->focus) {
        // Return empty JSON
        return env->NewStringUTF("{}");
    }

    game_object* player = g_game->first_view->focus;

    // Build JSON string with game state
    // TODO: Get actual values from player object
    char json[512];
    snprintf(json, sizeof(json),
        "{"
        "\"health\":%d,"
        "\"maxHealth\":%d,"
        "\"currentWeapon\":%d,"
        "\"ammo\":%d,"
        "\"score\":%d,"
        "\"lives\":%d,"
        "\"level\":\"%s\","
        "\"isPaused\":%s,"
        "\"isGameOver\":%s"
        "}",
        100, // TODO: player->health
        100, // TODO: player->max_health
        0,   // TODO: player->current_weapon
        0,   // TODO: player->ammo
        0,   // TODO: game->score
        3,   // TODO: game->lives
        "level01",
        "false",
        "false"
    );

    return env->NewStringUTF(json);
}

// ===== Settings =====

JNIEXPORT void JNICALL
Java_com_abused_AbusedEngineModule_nativeSetVolume(
    JNIEnv* /* env */,
    jobject /* this */,
    jfloat music,
    jfloat sfx) {

    LOGI("Setting volume - Music: %.2f, SFX: %.2f", music, sfx);

    // TODO: Set audio volumes
    // if (g_game) {
    //     g_game->set_music_volume(music);
    //     g_game->set_sfx_volume(sfx);
    // }
}

JNIEXPORT void JNICALL
Java_com_abused_AbusedEngineModule_nativeSetGraphicsQuality(
    JNIEnv* /* env */,
    jobject /* this */,
    jint quality) {

    LOGI("Setting graphics quality: %d", quality);

    // TODO: Adjust graphics settings based on quality level
    // 0 = Low, 1 = Medium, 2 = High, 3 = Ultra
}

} // extern "C"
