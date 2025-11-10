package com.abused;

import com.facebook.react.bridge.ReactApplicationContext;
import com.facebook.react.bridge.ReactContextBaseJavaModule;
import com.facebook.react.bridge.ReactMethod;
import com.facebook.react.bridge.Promise;

/**
 * React Native Native Module for Abused Game Engine
 * Provides JavaScript interface to C++ game engine via JNI
 */
public class AbusedEngineModule extends ReactContextBaseJavaModule {

    private static final String MODULE_NAME = "AbusedEngine";

    static {
        // Load native C++ library
        System.loadLibrary("abused-engine");
    }

    public AbusedEngineModule(ReactApplicationContext context) {
        super(context);
    }

    @Override
    public String getName() {
        return MODULE_NAME;
    }

    // ===== Lifecycle Methods =====

    @ReactMethod
    public void initialize(String assetPath, Promise promise) {
        try {
            boolean success = nativeInitialize(assetPath);
            promise.resolve(success);
        } catch (Exception e) {
            promise.reject("INIT_ERROR", "Failed to initialize engine: " + e.getMessage());
        }
    }

    @ReactMethod
    public void shutdown() {
        nativeShutdown();
    }

    @ReactMethod
    public void pause() {
        nativePause();
    }

    @ReactMethod
    public void resume() {
        nativeResume();
    }

    // ===== Game Loop =====

    @ReactMethod
    public void update(float deltaTimeMs) {
        nativeUpdate(deltaTimeMs);
    }

    // ===== Rendering =====

    @ReactMethod
    public int getRenderTexture() {
        return nativeGetRenderTexture();
    }

    @ReactMethod
    public void setViewport(int width, int height) {
        nativeSetViewport(width, height);
    }

    // ===== Input Methods =====

    @ReactMethod
    public void setMovementInput(float x, float y) {
        nativeSetMovementInput(x, y);
    }

    @ReactMethod
    public void setAimPosition(float screenX, float screenY) {
        nativeSetAimPosition(screenX, screenY);
    }

    @ReactMethod
    public void setFireButton(boolean pressed) {
        nativeSetFireButton(pressed);
    }

    @ReactMethod
    public void setJumpButton(boolean pressed) {
        nativeSetJumpButton(pressed);
    }

    @ReactMethod
    public void nextWeapon() {
        nativeNextWeapon();
    }

    @ReactMethod
    public void prevWeapon() {
        nativePrevWeapon();
    }

    @ReactMethod
    public void useSpecialAbility() {
        nativeUseSpecialAbility();
    }

    // ===== Game State =====

    @ReactMethod
    public void getGameState(Promise promise) {
        try {
            String jsonState = nativeGetGameState();
            promise.resolve(jsonState);
        } catch (Exception e) {
            promise.reject("STATE_ERROR", "Failed to get game state: " + e.getMessage());
        }
    }

    // ===== Settings =====

    @ReactMethod
    public void setVolume(float music, float sfx) {
        nativeSetVolume(music, sfx);
    }

    @ReactMethod
    public void setGraphicsQuality(int quality) {
        nativeSetGraphicsQuality(quality);
    }

    // ===== Native JNI Method Declarations =====

    private native boolean nativeInitialize(String assetPath);
    private native void nativeShutdown();
    private native void nativePause();
    private native void nativeResume();

    private native void nativeUpdate(float deltaTimeMs);

    private native int nativeGetRenderTexture();
    private native void nativeSetViewport(int width, int height);

    private native void nativeSetMovementInput(float x, float y);
    private native void nativeSetAimPosition(float screenX, float screenY);
    private native void nativeSetFireButton(boolean pressed);
    private native void nativeSetJumpButton(boolean pressed);
    private native void nativeNextWeapon();
    private native void nativePrevWeapon();
    private native void nativeUseSpecialAbility();

    private native String nativeGetGameState();

    private native void nativeSetVolume(float music, float sfx);
    private native void nativeSetGraphicsQuality(int quality);
}
