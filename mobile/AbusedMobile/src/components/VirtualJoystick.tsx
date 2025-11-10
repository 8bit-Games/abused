/**
 * Virtual Joystick Component
 * Provides touch-based movement control for mobile
 */

import React, { useRef, useState, useCallback } from 'react';
import {
  View,
  StyleSheet,
  PanResponder,
  Animated,
  ViewStyle,
} from 'react-native';

export interface VirtualJoystickProps {
  onJoystickMove: (x: number, y: number) => void;
  size?: number;
  color?: string;
  opacity?: number;
  style?: ViewStyle;
}

export const VirtualJoystick: React.FC<VirtualJoystickProps> = ({
  onJoystickMove,
  size = 160,
  color = '#FFFFFF',
  opacity = 0.5,
  style,
}) => {
  const [active, setActive] = useState(false);
  const panX = useRef(new Animated.Value(0)).current;
  const panY = useRef(new Animated.Value(0)).current;
  const maxDistance = size / 2 - 20; // Inner knob stays within outer circle

  const panResponder = useRef(
    PanResponder.create({
      onStartShouldSetPanResponder: () => true,
      onMoveShouldSetPanResponder: () => true,

      onPanResponderGrant: () => {
        setActive(true);
      },

      onPanResponderMove: (_, gestureState) => {
        const { dx, dy } = gestureState;
        const distance = Math.sqrt(dx * dx + dy * dy);

        let finalX = dx;
        let finalY = dy;

        // Constrain to circle
        if (distance > maxDistance) {
          const angle = Math.atan2(dy, dx);
          finalX = Math.cos(angle) * maxDistance;
          finalY = Math.sin(angle) * maxDistance;
        }

        // Update visual position
        panX.setValue(finalX);
        panY.setValue(finalY);

        // Normalize to -1..1 range and send to parent
        const normalizedX = finalX / maxDistance;
        const normalizedY = finalY / maxDistance;
        onJoystickMove(normalizedX, normalizedY);
      },

      onPanResponderRelease: () => {
        setActive(false);

        // Spring back to center
        Animated.spring(panX, {
          toValue: 0,
          useNativeDriver: true,
          tension: 50,
          friction: 8,
        }).start();

        Animated.spring(panY, {
          toValue: 0,
          useNativeDriver: true,
          tension: 50,
          friction: 8,
        }).start();

        // Reset input
        onJoystickMove(0, 0);
      },
    })
  ).current;

  return (
    <View
      style={[
        styles.container,
        {
          width: size,
          height: size,
          borderRadius: size / 2,
        },
        style,
      ]}
      {...panResponder.panHandlers}
    >
      {/* Outer circle */}
      <View
        style={[
          styles.outerCircle,
          {
            width: size,
            height: size,
            borderRadius: size / 2,
            borderColor: color,
            backgroundColor: `${color}${Math.floor(opacity * 0.3 * 255).toString(16).padStart(2, '0')}`,
            borderWidth: active ? 3 : 2,
          },
        ]}
      >
        {/* Inner knob */}
        <Animated.View
          style={[
            styles.innerKnob,
            {
              width: size / 2,
              height: size / 2,
              borderRadius: size / 4,
              backgroundColor: `${color}${Math.floor(opacity * 255).toString(16).padStart(2, '0')}`,
              transform: [{ translateX: panX }, { translateY: panY }],
            },
          ]}
        />

        {/* Directional indicators */}
        {!active && (
          <>
            {/* Up arrow */}
            <View style={[styles.arrow, styles.arrowUp, { borderBottomColor: color }]} />
            {/* Right arrow */}
            <View style={[styles.arrow, styles.arrowRight, { borderLeftColor: color }]} />
            {/* Down arrow */}
            <View style={[styles.arrow, styles.arrowDown, { borderTopColor: color }]} />
            {/* Left arrow */}
            <View style={[styles.arrow, styles.arrowLeft, { borderRightColor: color }]} />
          </>
        )}
      </View>
    </View>
  );
};

const styles = StyleSheet.create({
  container: {
    justifyContent: 'center',
    alignItems: 'center',
  },
  outerCircle: {
    justifyContent: 'center',
    alignItems: 'center',
    borderWidth: 2,
  },
  innerKnob: {
    position: 'absolute',
  },
  arrow: {
    position: 'absolute',
    width: 0,
    height: 0,
    backgroundColor: 'transparent',
    borderStyle: 'solid',
  },
  arrowUp: {
    top: 15,
    borderLeftWidth: 8,
    borderRightWidth: 8,
    borderBottomWidth: 12,
    borderLeftColor: 'transparent',
    borderRightColor: 'transparent',
  },
  arrowRight: {
    right: 15,
    borderTopWidth: 8,
    borderBottomWidth: 8,
    borderLeftWidth: 12,
    borderTopColor: 'transparent',
    borderBottomColor: 'transparent',
  },
  arrowDown: {
    bottom: 15,
    borderLeftWidth: 8,
    borderRightWidth: 8,
    borderTopWidth: 12,
    borderLeftColor: 'transparent',
    borderRightColor: 'transparent',
  },
  arrowLeft: {
    left: 15,
    borderTopWidth: 8,
    borderBottomWidth: 8,
    borderRightWidth: 12,
    borderTopColor: 'transparent',
    borderBottomColor: 'transparent',
  },
});
