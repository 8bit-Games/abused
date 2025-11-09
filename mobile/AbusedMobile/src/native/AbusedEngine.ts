/**
 * Abused Native Engine Bridge
 * TypeScript interface for the native C++ game engine
 */

import { NativeModules, NativeEventEmitter } from 'react-native';

const { AbusedEngine: NativeAbusedEngine } = NativeModules;

export enum GraphicsQuality {
  LOW = 0,
  MEDIUM = 1,
  HIGH = 2,
  ULTRA = 3,
}

export interface GameState {
  health: number;
  maxHealth: number;
  currentWeapon: number;
  ammo: number;
  score: number;
  lives: number;
  level: string;
  isPaused: boolean;
  isGameOver: boolean;
}

export interface AbusedEngineEvents {
  onHealthChanged: (data: { health: number; maxHealth: number }) => void;
  onWeaponChanged: (data: { weaponId: number; ammo: number }) => void;
  onScoreChanged: (data: { score: number }) => void;
  onLevelComplete: (data: { level: string; score: number }) => void;
  onGameOver: (data: { finalScore: number; reason: string }) => void;
}

class AbusedEngineWrapper {
  private eventEmitter: NativeEventEmitter;
  private listeners: Map<string, any[]> = new Map();

  constructor() {
    this.eventEmitter = new NativeEventEmitter(NativeAbusedEngine);
  }

  // ===== Lifecycle =====

  async initialize(assetPath: string): Promise<boolean> {
    try {
      return await NativeAbusedEngine.initialize(assetPath);
    } catch (error) {
      console.error('Failed to initialize engine:', error);
      return false;
    }
  }

  shutdown(): void {
    NativeAbusedEngine.shutdown();
  }

  pause(): void {
    NativeAbusedEngine.pause();
  }

  resume(): void {
    NativeAbusedEngine.resume();
  }

  // ===== Game Loop =====

  update(deltaTimeMs: number): void {
    NativeAbusedEngine.update(deltaTimeMs);
  }

  // ===== Rendering =====

  getRenderTexture(): number {
    return NativeAbusedEngine.getRenderTexture();
  }

  setViewport(width: number, height: number): void {
    NativeAbusedEngine.setViewport(width, height);
  }

  // ===== Input =====

  setMovementInput(x: number, y: number): void {
    // Clamp to -1..1 range
    const clampedX = Math.max(-1, Math.min(1, x));
    const clampedY = Math.max(-1, Math.min(1, y));
    NativeAbusedEngine.setMovementInput(clampedX, clampedY);
  }

  setAimPosition(screenX: number, screenY: number): void {
    NativeAbusedEngine.setAimPosition(screenX, screenY);
  }

  setFireButton(pressed: boolean): void {
    NativeAbusedEngine.setFireButton(pressed);
  }

  setJumpButton(pressed: boolean): void {
    NativeAbusedEngine.setJumpButton(pressed);
  }

  nextWeapon(): void {
    NativeAbusedEngine.nextWeapon();
  }

  prevWeapon(): void {
    NativeAbusedEngine.prevWeapon();
  }

  useSpecialAbility(): void {
    NativeAbusedEngine.useSpecialAbility();
  }

  // ===== Game State =====

  async getGameState(): Promise<GameState> {
    try {
      return await NativeAbusedEngine.getGameState();
    } catch (error) {
      console.error('Failed to get game state:', error);
      return {
        health: 0,
        maxHealth: 100,
        currentWeapon: 0,
        ammo: 0,
        score: 0,
        lives: 0,
        level: '',
        isPaused: true,
        isGameOver: true,
      };
    }
  }

  async loadLevel(levelName: string): Promise<boolean> {
    try {
      return await NativeAbusedEngine.loadLevel(levelName);
    } catch (error) {
      console.error('Failed to load level:', error);
      return false;
    }
  }

  async saveGame(slotId: number): Promise<boolean> {
    try {
      return await NativeAbusedEngine.saveGame(slotId);
    } catch (error) {
      console.error('Failed to save game:', error);
      return false;
    }
  }

  async loadGame(slotId: number): Promise<boolean> {
    try {
      return await NativeAbusedEngine.loadGame(slotId);
    } catch (error) {
      console.error('Failed to load game:', error);
      return false;
    }
  }

  // ===== Settings =====

  setVolume(music: number, sfx: number): void {
    const clampedMusic = Math.max(0, Math.min(1, music));
    const clampedSfx = Math.max(0, Math.min(1, sfx));
    NativeAbusedEngine.setVolume(clampedMusic, clampedSfx);
  }

  setGraphicsQuality(quality: GraphicsQuality): void {
    NativeAbusedEngine.setGraphicsQuality(quality);
  }

  // ===== Events =====

  addEventListener<K extends keyof AbusedEngineEvents>(
    event: K,
    callback: AbusedEngineEvents[K]
  ): void {
    const subscription = this.eventEmitter.addListener(event, callback);

    if (!this.listeners.has(event)) {
      this.listeners.set(event, []);
    }
    this.listeners.get(event)!.push(subscription);
  }

  removeEventListener<K extends keyof AbusedEngineEvents>(
    event: K,
    callback: AbusedEngineEvents[K]
  ): void {
    const subs = this.listeners.get(event);
    if (subs) {
      subs.forEach((sub) => sub.remove());
      this.listeners.delete(event);
    }
  }

  removeAllListeners(): void {
    this.listeners.forEach((subs) => {
      subs.forEach((sub) => sub.remove());
    });
    this.listeners.clear();
  }
}

export const AbusedEngine = new AbusedEngineWrapper();
