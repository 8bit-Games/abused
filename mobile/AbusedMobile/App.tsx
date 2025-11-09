/**
 * Abused Mobile - Main App Component
 * Combines game engine with touch controls
 */

import React, { useEffect, useRef, useState } from 'react';
import {
  StyleSheet,
  View,
  Text,
  TouchableOpacity,
  Dimensions,
  StatusBar,
  Alert,
} from 'react-native';
import { VirtualJoystick } from './src/components/VirtualJoystick';
import { TouchAim } from './src/components/TouchAim';
import { AbusedEngine, GameState } from './src/native/AbusedEngine';

const { width, height } = Dimensions.get('window');

const App: React.FC = () => {
  const [initialized, setInitialized] = useState(false);
  const [gameState, setGameState] = useState<GameState | null>(null);
  const [isPaused, setIsPaused] = useState(false);
  const gameLoopRef = useRef<number | null>(null);
  const lastFrameTimeRef = useRef<number>(Date.now());

  // Initialize game engine
  useEffect(() => {
    const initializeEngine = async () => {
      try {
        // TODO: Replace with actual asset path
        const assetPath = '/path/to/game/assets';
        const success = await AbusedEngine.initialize(assetPath);

        if (success) {
          console.log('Game engine initialized successfully');
          setInitialized(true);

          // Set viewport size
          AbusedEngine.setViewport(width, height - 200); // Reserve space for controls

          // Start game loop
          startGameLoop();
        } else {
          Alert.alert('Error', 'Failed to initialize game engine');
        }
      } catch (error) {
        console.error('Initialization error:', error);
        Alert.alert('Error', 'Failed to initialize game engine');
      }
    };

    initializeEngine();

    // Cleanup on unmount
    return () => {
      stopGameLoop();
      AbusedEngine.shutdown();
    };
  }, []);

  // Game loop
  const startGameLoop = () => {
    lastFrameTimeRef.current = Date.now();

    const loop = () => {
      const now = Date.now();
      const deltaTime = now - lastFrameTimeRef.current;
      lastFrameTimeRef.current = now;

      if (!isPaused) {
        // Update game engine
        AbusedEngine.update(deltaTime);

        // Update game state (every 100ms to avoid overhead)
        if (now % 100 < deltaTime) {
          updateGameState();
        }
      }

      gameLoopRef.current = requestAnimationFrame(loop);
    };

    gameLoopRef.current = requestAnimationFrame(loop);
  };

  const stopGameLoop = () => {
    if (gameLoopRef.current !== null) {
      cancelAnimationFrame(gameLoopRef.current);
      gameLoopRef.current = null;
    }
  };

  const updateGameState = async () => {
    try {
      const state = await AbusedEngine.getGameState();
      setGameState(state);
    } catch (error) {
      console.error('Failed to get game state:', error);
    }
  };

  // Event handlers
  const handleMovementChange = (x: number, y: number) => {
    AbusedEngine.setMovementInput(x, y);
  };

  const handleAimMove = (screenX: number, screenY: number) => {
    AbusedEngine.setAimPosition(screenX, screenY);
  };

  const handleFireStart = () => {
    AbusedEngine.setFireButton(true);
  };

  const handleFireEnd = () => {
    AbusedEngine.setFireButton(false);
  };

  const handleJumpPress = () => {
    AbusedEngine.setJumpButton(true);
  };

  const handleJumpRelease = () => {
    AbusedEngine.setJumpButton(false);
  };

  const handleNextWeapon = () => {
    AbusedEngine.nextWeapon();
  };

  const handlePrevWeapon = () => {
    AbusedEngine.prevWeapon();
  };

  const handleSpecial = () => {
    AbusedEngine.useSpecialAbility();
  };

  const handlePauseToggle = () => {
    if (isPaused) {
      AbusedEngine.resume();
      startGameLoop();
    } else {
      AbusedEngine.pause();
      stopGameLoop();
    }
    setIsPaused(!isPaused);
  };

  if (!initialized) {
    return (
      <View style={styles.loadingContainer}>
        <Text style={styles.loadingText}>Loading Abused...</Text>
      </View>
    );
  }

  return (
    <View style={styles.container}>
      <StatusBar hidden />

      {/* Game Viewport */}
      <View style={styles.gameViewport}>
        {/* TODO: Render game texture here using OpenGL texture sharing */}
        <View style={styles.placeholder}>
          <Text style={styles.placeholderText}>ABUSED</Text>
          <Text style={styles.subText}>Game Rendering Here</Text>
        </View>

        {/* Aim Zone (covers entire game viewport) */}
        <TouchAim
          onAimMove={handleAimMove}
          onFireStart={handleFireStart}
          onFireEnd={handleFireEnd}
          style={StyleSheet.absoluteFill}
        />
      </View>

      {/* HUD Overlay */}
      <View style={styles.hudOverlay}>
        {/* Weapon Switch Buttons (Top) */}
        <View style={styles.topBar}>
          <TouchableOpacity style={styles.weaponButton} onPress={handlePrevWeapon}>
            <Text style={styles.buttonText}>❮ PREV</Text>
          </TouchableOpacity>

          <TouchableOpacity style={styles.pauseButton} onPress={handlePauseToggle}>
            <Text style={styles.buttonText}>{isPaused ? '▶' : '❚❚'}</Text>
          </TouchableOpacity>

          <TouchableOpacity style={styles.weaponButton} onPress={handleNextWeapon}>
            <Text style={styles.buttonText}>NEXT ❯</Text>
          </TouchableOpacity>
        </View>

        {/* Game Stats */}
        {gameState && (
          <View style={styles.statsBar}>
            <Text style={styles.statText}>HP: {gameState.health}/{gameState.maxHealth}</Text>
            <Text style={styles.statText}>Ammo: {gameState.ammo}</Text>
            <Text style={styles.statText}>Score: {gameState.score}</Text>
          </View>
        )}
      </View>

      {/* Controls Overlay */}
      <View style={styles.controlsOverlay}>
        {/* Left Side - Movement Joystick */}
        <View style={styles.leftControls}>
          <VirtualJoystick
            onJoystickMove={handleMovementChange}
            size={160}
            opacity={0.6}
            style={styles.joystick}
          />
        </View>

        {/* Right Side - Action Buttons */}
        <View style={styles.rightControls}>
          <TouchableOpacity
            style={styles.specialButton}
            onPress={handleSpecial}
          >
            <Text style={styles.buttonText}>🎯</Text>
          </TouchableOpacity>

          <TouchableOpacity
            style={styles.jumpButton}
            onPressIn={handleJumpPress}
            onPressOut={handleJumpRelease}
          >
            <Text style={styles.buttonText}>JUMP</Text>
          </TouchableOpacity>
        </View>
      </View>
    </View>
  );
};

const styles = StyleSheet.create({
  container: {
    flex: 1,
    backgroundColor: '#000000',
  },
  loadingContainer: {
    flex: 1,
    justifyContent: 'center',
    alignItems: 'center',
    backgroundColor: '#000000',
  },
  loadingText: {
    color: '#FFFFFF',
    fontSize: 24,
    fontWeight: 'bold',
  },
  gameViewport: {
    flex: 1,
    backgroundColor: '#1a1a1a',
  },
  placeholder: {
    flex: 1,
    justifyContent: 'center',
    alignItems: 'center',
  },
  placeholderText: {
    color: '#FF0000',
    fontSize: 48,
    fontWeight: 'bold',
    letterSpacing: 4,
  },
  subText: {
    color: '#888888',
    fontSize: 16,
    marginTop: 10,
  },
  hudOverlay: {
    position: 'absolute',
    top: 0,
    left: 0,
    right: 0,
    pointerEvents: 'box-none',
  },
  topBar: {
    flexDirection: 'row',
    justifyContent: 'space-between',
    alignItems: 'center',
    paddingHorizontal: 20,
    paddingTop: 40,
  },
  weaponButton: {
    backgroundColor: 'rgba(255, 255, 255, 0.2)',
    paddingHorizontal: 20,
    paddingVertical: 10,
    borderRadius: 8,
    borderWidth: 1,
    borderColor: 'rgba(255, 255, 255, 0.3)',
  },
  pauseButton: {
    backgroundColor: 'rgba(255, 255, 255, 0.2)',
    width: 50,
    height: 50,
    borderRadius: 25,
    justifyContent: 'center',
    alignItems: 'center',
    borderWidth: 1,
    borderColor: 'rgba(255, 255, 255, 0.3)',
  },
  buttonText: {
    color: '#FFFFFF',
    fontSize: 14,
    fontWeight: '600',
  },
  statsBar: {
    flexDirection: 'row',
    justifyContent: 'space-around',
    paddingHorizontal: 20,
    paddingVertical: 10,
    marginTop: 10,
    backgroundColor: 'rgba(0, 0, 0, 0.5)',
  },
  statText: {
    color: '#FFFFFF',
    fontSize: 14,
    fontWeight: '600',
  },
  controlsOverlay: {
    position: 'absolute',
    bottom: 0,
    left: 0,
    right: 0,
    height: 200,
    flexDirection: 'row',
    justifyContent: 'space-between',
    alignItems: 'flex-end',
    paddingHorizontal: 20,
    paddingBottom: 20,
    pointerEvents: 'box-none',
  },
  leftControls: {
    justifyContent: 'flex-end',
    pointerEvents: 'auto',
  },
  joystick: {
    marginBottom: 10,
  },
  rightControls: {
    alignItems: 'flex-end',
    justifyContent: 'flex-end',
    gap: 15,
    pointerEvents: 'auto',
  },
  specialButton: {
    width: 60,
    height: 60,
    borderRadius: 30,
    backgroundColor: 'rgba(255, 100, 100, 0.6)',
    justifyContent: 'center',
    alignItems: 'center',
    borderWidth: 2,
    borderColor: 'rgba(255, 255, 255, 0.4)',
  },
  jumpButton: {
    width: 70,
    height: 70,
    borderRadius: 35,
    backgroundColor: 'rgba(255, 255, 255, 0.6)',
    justifyContent: 'center',
    alignItems: 'center',
    borderWidth: 2,
    borderColor: 'rgba(255, 255, 255, 0.4)',
  },
});

export default App;
