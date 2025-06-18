/*
  Enhanced config.h - Data Structure with Range Settings
  RC Transmitter for Teensy 4.0
*/

#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// Enhanced data structure - MUST match receiver exactly
struct RCData {
  int16_t throttle;           // -1000 to +1000
  int16_t steering;           // -1000 to +1000
  uint32_t counter;           // Packet counter
  
  // NEW: Range configuration data
  int16_t throttle_min_pwm;     // Minimum throttle PWM (1000-2000)
  int16_t throttle_max_pwm;     // Maximum throttle PWM (1000-2000)
  int16_t steer_min_degree;     // Minimum steering angle (-90 to +90)
  int16_t steer_neutral_degree; // Neutral/center steering angle (-90 to +90)
  int16_t steer_max_degree;     // Maximum steering angle (-90 to +90)
  
  uint8_t config_changed;       // Flag to indicate settings changed
};

// External data variable
extern RCData data;

// Pin definitions - Teensy 4.0 Optimized layout
#define RIGHT_JOY_X    A2    // Pin 15 - Steering
#define RIGHT_JOY_Y    A3    // Pin 14 
#define LEFT_JOY_Y     A0    // Pin 16 - Throttle  
#define LEFT_JOY_X     A1    // Pin 17
#define LEFT_POT       A6    // Pin 20
#define RIGHT_POT      A7    // Pin 21

#define RIGHT_JOY_BTN  2
#define LEFT_JOY_BTN   3
#define LEFT_TRIGGER_DOWN   28
#define LEFT_TRIGGER_UP     25
#define RIGHT_TRIGGER_DOWN  1
#define RIGHT_TRIGGER_UP    0

#define BUTTON_LEFT    5
#define BUTTON_OK      8
#define BUTTON_DOWN    4
#define BUTTON_UP      7
#define BUTTON_RIGHT   6

#define LED_RED   30
#define LED_GREEN 27
#define LED_BLUE  26

#define RADIO_CE   9
#define RADIO_CSN  10

// Pin definitions - Display (I2C)
// Teensy 4.0 I2C pins
#define DISPLAY_SDA 18  // I2C SDA (default for Teensy 4.0)
#define DISPLAY_SCL 19  // I2C SCL (default for Teensy 4.0)

// Display constants
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define SCREEN_ADDRESS 0x3C
#define YELLOW_AREA_HEIGHT 16  // Top 16 pixels are yellow
#define BLUE_AREA_HEIGHT 48    // Bottom 48 pixels are blue
#define BLUE_AREA_START 16     // Blue area starts at pixel 16

// Radio constants
#define RADIO_CHANNEL 76
#define RADIO_ADDRESS "BOAT1"

// Timing constants
#define TRANSMIT_INTERVAL 20    // 50Hz transmission
#define DISPLAY_INTERVAL 50     // 20Hz display update
#define DEADZONE_THRESHOLD 50   // Joystick deadzone

// Debug constants
#define DEBUG_INTERVAL 100      // Print debug every 100 packets

#endif