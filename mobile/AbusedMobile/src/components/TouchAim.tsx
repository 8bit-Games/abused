/**
 * Touch Aim Component
 * Provides touch-based aiming and firing control
 */

import React, { useRef, useState } from 'react';
import {
  View,
  StyleSheet,
  PanResponder,
  Animated,
  ViewStyle,
  Vibration,
} from 'react-native';

export interface TouchAimProps {
  onAimMove: (screenX: number, screenY: number) => void;
  onFireStart: () => void;
  onFireEnd: () => void;
  enableHaptics?: boolean;
  showCrosshair?: boolean;
  crosshairColor?: string;
  style?: ViewStyle;
}

export const TouchAim: React.FC<TouchAimProps> = ({
  onAimMove,
  onFireStart,
  onFireEnd,
  enableHaptics = true,
  showCrosshair = true,
  crosshairColor = '#FF0000',
  style,
}) => {
  const [aimPosition, setAimPosition] = useState({ x: 0, y: 0 });
  const [isFiring, setIsFiring] = useState(false);
  const crosshairOpacity = useRef(new Animated.Value(0)).current;

  const panResponder = useRef(
    PanResponder.create({
      onStartShouldSetPanResponder: () => true,
      onMoveShouldSetPanResponder: () => true,

      onPanResponderGrant: (evt) => {
        const { locationX, locationY } = evt.nativeEvent;

        setAimPosition({ x: locationX, y: locationY });
        setIsFiring(true);

        // Show crosshair
        Animated.timing(crosshairOpacity, {
          toValue: 1,
          duration: 100,
          useNativeDriver: true,
        }).start();

        // Haptic feedback for fire start
        if (enableHaptics) {
          Vibration.vibrate(10); // Light impact
        }

        onAimMove(locationX, locationY);
        onFireStart();
      },

      onPanResponderMove: (evt) => {
        const { locationX, locationY } = evt.nativeEvent;

        setAimPosition({ x: locationX, y: locationY });
        onAimMove(locationX, locationY);

        // Continuous firing
        if (!isFiring) {
          setIsFiring(true);
          onFireStart();
        }
      },

      onPanResponderRelease: () => {
        setIsFiring(false);

        // Hide crosshair
        Animated.timing(crosshairOpacity, {
          toValue: 0,
          duration: 200,
          useNativeDriver: true,
        }).start();

        onFireEnd();
      },

      onPanResponderTerminate: () => {
        setIsFiring(false);

        Animated.timing(crosshairOpacity, {
          toValue: 0,
          duration: 200,
          useNativeDriver: true,
        }).start();

        onFireEnd();
      },
    })
  ).current;

  return (
    <View style={[styles.container, style]} {...panResponder.panHandlers}>
      {/* Crosshair */}
      {showCrosshair && (
        <Animated.View
          style={[
            styles.crosshair,
            {
              left: aimPosition.x - 20,
              top: aimPosition.y - 20,
              opacity: crosshairOpacity,
            },
          ]}
        >
          {/* Horizontal line */}
          <View
            style={[
              styles.crosshairLineH,
              { backgroundColor: crosshairColor },
            ]}
          />
          {/* Vertical line */}
          <View
            style={[
              styles.crosshairLineV,
              { backgroundColor: crosshairColor },
            ]}
          />
          {/* Center dot */}
          <View
            style={[
              styles.crosshairDot,
              { backgroundColor: crosshairColor },
            ]}
          />
          {/* Outer ring */}
          <View
            style={[
              styles.crosshairRing,
              {
                borderColor: crosshairColor,
                transform: [{ scale: isFiring ? 0.8 : 1 }],
              },
            ]}
          />
        </Animated.View>
      )}

      {/* Tap instruction (shown when not aiming) */}
      {!isFiring && (
        <View style={styles.instructionContainer}>
          <View style={styles.instructionBox}>
            <View style={styles.tapIcon} />
            <Animated.Text
              style={[
                styles.instructionText,
                { opacity: Animated.subtract(1, crosshairOpacity) },
              ]}
            >
              TAP TO AIM & FIRE
            </Animated.Text>
          </View>
        </View>
      )}
    </View>
  );
};

const styles = StyleSheet.create({
  container: {
    flex: 1,
    backgroundColor: 'transparent',
  },
  crosshair: {
    position: 'absolute',
    width: 40,
    height: 40,
    justifyContent: 'center',
    alignItems: 'center',
  },
  crosshairLineH: {
    position: 'absolute',
    width: 40,
    height: 2,
  },
  crosshairLineV: {
    position: 'absolute',
    width: 2,
    height: 40,
  },
  crosshairDot: {
    position: 'absolute',
    width: 4,
    height: 4,
    borderRadius: 2,
  },
  crosshairRing: {
    position: 'absolute',
    width: 32,
    height: 32,
    borderRadius: 16,
    borderWidth: 2,
  },
  instructionContainer: {
    position: 'absolute',
    top: '50%',
    left: '50%',
    transform: [{ translateX: -100 }, { translateY: -30 }],
  },
  instructionBox: {
    alignItems: 'center',
    justifyContent: 'center',
  },
  tapIcon: {
    width: 40,
    height: 40,
    borderRadius: 20,
    borderWidth: 2,
    borderColor: 'rgba(255, 255, 255, 0.3)',
    marginBottom: 10,
    backgroundColor: 'rgba(255, 255, 255, 0.1)',
  },
  instructionText: {
    color: 'rgba(255, 255, 255, 0.5)',
    fontSize: 14,
    fontWeight: '600',
    letterSpacing: 1,
  },
});
