//
//  AbusedEngine.mm
//  iOS Objective-C++ Bridge Implementation for Abused Game Engine
//

#import "AbusedEngine.h"
#import <React/RCTLog.h>

// Include C++ game engine headers
#include "game.h"
#include "mobile/mobile_input.h"
#include "view.h"
#include <SDL2/SDL.h>

// Global game instance
static game* g_game = nullptr;
static bool g_initialized = false;

@implementation AbusedEngine

RCT_EXPORT_MODULE();

// ===== Event Emitter Support =====

- (NSArray<NSString *> *)supportedEvents {
    return @[
        @"onHealthChanged",
        @"onWeaponChanged",
        @"onScoreChanged",
        @"onLevelComplete",
        @"onGameOver"
    ];
}

// ===== Lifecycle Methods =====

RCT_EXPORT_METHOD(initialize:(NSString *)assetPath
                  resolver:(RCTPromiseResolveBlock)resolve
                  rejecter:(RCTPromiseRejectBlock)reject) {

    if (g_initialized) {
        RCTLogInfo(@"Engine already initialized");
        resolve(@YES);
        return;
    }

    const char* path = [assetPath UTF8String];
    RCTLogInfo(@"Initializing Abused engine with asset path: %s", path);

    @try {
        // Initialize mobile input system
        mobile_input::initialize();

        // Initialize SDL
        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
            NSString *error = [NSString stringWithFormat:@"SDL initialization failed: %s", SDL_GetError()];
            RCTLogError(@"%@", error);
            reject(@"init_failed", error, nil);
            return;
        }

        // Create game instance
        g_game = new game();

        if (!g_game) {
            reject(@"init_failed", @"Failed to create game instance", nil);
            SDL_Quit();
            return;
        }

        // TODO: Set asset path and initialize game

        g_initialized = true;
        RCTLogInfo(@"Engine initialized successfully");
        resolve(@YES);

    } @catch (NSException *exception) {
        NSString *error = [NSString stringWithFormat:@"Exception during initialization: %@", exception.reason];
        RCTLogError(@"%@", error);
        reject(@"init_exception", error, nil);
    }
}

RCT_EXPORT_METHOD(shutdown) {
    if (!g_initialized) {
        return;
    }

    RCTLogInfo(@"Shutting down engine");

    mobile_input::shutdown();

    if (g_game) {
        delete g_game;
        g_game = nullptr;
    }

    SDL_Quit();
    g_initialized = false;

    RCTLogInfo(@"Engine shutdown complete");
}

RCT_EXPORT_METHOD(pause) {
    RCTLogInfo(@"Pausing game");
    mobile_input::clear_input();

    if (g_game) {
        // TODO: Call game pause method
        // g_game->pause();
    }
}

RCT_EXPORT_METHOD(resume) {
    RCTLogInfo(@"Resuming game");

    if (g_game) {
        // TODO: Call game resume method
        // g_game->resume();
    }
}

// ===== Game Loop =====

RCT_EXPORT_METHOD(update:(float)deltaTimeMs) {
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

RCT_EXPORT_BLOCKING_SYNCHRONOUS_METHOD(getRenderTexture) {
    if (!g_initialized || !g_game) {
        return @0;
    }

    // TODO: Return OpenGL texture ID
    return @0; // Placeholder
}

RCT_EXPORT_METHOD(setViewport:(int)width height:(int)height) {
    RCTLogInfo(@"Setting viewport: %dx%d", width, height);

    if (!g_initialized || !g_game) {
        return;
    }

    // TODO: Update game viewport
    // if (g_game->first_view) {
    //     g_game->first_view->resize(width, height);
    // }
}

// ===== Input Methods =====

RCT_EXPORT_METHOD(setMovementInput:(float)x y:(float)y) {
    mobile_input::set_movement_input(x, y);
}

RCT_EXPORT_METHOD(setAimPosition:(float)screenX y:(float)screenY) {
    mobile_input::set_aim_position(screenX, screenY);
}

RCT_EXPORT_METHOD(setFireButton:(BOOL)pressed) {
    mobile_input::set_fire_button(pressed);
}

RCT_EXPORT_METHOD(setJumpButton:(BOOL)pressed) {
    mobile_input::set_jump_button(pressed);
}

RCT_EXPORT_METHOD(nextWeapon) {
    RCTLogInfo(@"Next weapon");
    mobile_input::next_weapon();
}

RCT_EXPORT_METHOD(prevWeapon) {
    RCTLogInfo(@"Previous weapon");
    mobile_input::prev_weapon();
}

RCT_EXPORT_METHOD(useSpecialAbility) {
    RCTLogInfo(@"Special ability");
    mobile_input::set_special_button(true);
}

// ===== Game State =====

RCT_EXPORT_METHOD(getGameState:(RCTPromiseResolveBlock)resolve
                  rejecter:(RCTPromiseRejectBlock)reject) {

    if (!g_initialized || !g_game || !g_game->first_view || !g_game->first_view->focus) {
        resolve(@{});
        return;
    }

    game_object* player = g_game->first_view->focus;

    // Build dictionary with game state
    NSDictionary *state = @{
        @"health": @100,        // TODO: @(player->health)
        @"maxHealth": @100,     // TODO: @(player->max_health)
        @"currentWeapon": @0,   // TODO: @(player->current_weapon)
        @"ammo": @0,            // TODO: @(player->ammo)
        @"score": @0,           // TODO: @(game->score)
        @"lives": @3,           // TODO: @(game->lives)
        @"level": @"level01",
        @"isPaused": @NO,
        @"isGameOver": @NO
    };

    resolve(state);
}

// ===== Settings =====

RCT_EXPORT_METHOD(setVolume:(float)music sfx:(float)sfx) {
    RCTLogInfo(@"Setting volume - Music: %.2f, SFX: %.2f", music, sfx);

    // TODO: Set audio volumes
    // if (g_game) {
    //     g_game->set_music_volume(music);
    //     g_game->set_sfx_volume(sfx);
    // }
}

RCT_EXPORT_METHOD(setGraphicsQuality:(int)quality) {
    RCTLogInfo(@"Setting graphics quality: %d", quality);

    // TODO: Adjust graphics settings based on quality level
    // 0 = Low, 1 = Medium, 2 = High, 3 = Ultra
}

// ===== Helper Methods =====

// Method to send events to JavaScript
- (void)sendGameEvent:(NSString *)eventName data:(NSDictionary *)data {
    if (self.bridge) {
        [self sendEventWithName:eventName body:data];
    }
}

@end
