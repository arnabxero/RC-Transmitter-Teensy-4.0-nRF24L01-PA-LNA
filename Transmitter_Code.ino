/*
  RC Transmitter for Teensy 4.0 - V3 with Advanced Menu System and Factory Reset
  Modular design with calibration and menu system
  
  Files:
  - Tx_Code_Teensy.ino (this file): Main setup and loop
  - menu.h: Advanced menu system with calibration and factory reset
  - display.h: Display functions and UI
  - controls.h: Button and joystick handling  
  - radio.h: NRF24 communication
  - config.h: Pin definitions and constants
  
  Features:
  - Hold OK button for 2 seconds to enter menu
  - Arrow keys and joysticks for navigation
  - Calibration system for joysticks, potentiometers, and MPU6500
  - EEPROM storage for calibration data
  - Auto-exit menu after 30 seconds
  - Calibrated control values for better precision
  - Fixed LED settings that properly respect user preferences
  - Teensy 4.0 optimized pin layout
  - Factory Reset with multi-step confirmation and animated progress
*/

#include "config.h"
#include "radio.h"
#include "display.h" 
#include "controls.h"
#include "menu.h"
#include "radio_test.h" // Added for radio testing

// Global variables
RCData data;
unsigned long lastTransmit = 0;
unsigned long lastDisplayUpdate = 0;

// LED update tracking to prevent excessive calls
unsigned long lastLEDUpdate = 0;
bool lastArmedState = false;
bool lastMenuState = false;

void setup() {
  Serial.begin(115200);
  delay(1000); // Give serial time to initialize
  
  Serial.println("=====================================");
  Serial.println("RC Transmitter V3 for Teensy 4.0 Starting...");
  Serial.println("Features: Menu System + Calibration + Factory Reset");
  Serial.println("=====================================");
  
  // Initialize all modules in order
  Serial.println("1. Initializing Display...");
  initDisplay();
  
  Serial.println("2. Initializing Controls...");
  initControls();
  
  Serial.println("3. Initializing Radio...");
  initRadio();
  
  Serial.println("4. Initializing Menu System...");
  initMenu();
  
  // Initialize data structure
  data.throttle = 0;
  data.steering = 0;
  data.counter = 0;
  
  // Show ready screen with menu instructions
  displayReady();
  
  // Apply LED settings after everything is initialized
  extern void applyLEDSettings();
  applyLEDSettings();
  
  Serial.println("=====================================");
  Serial.println("Setup Complete! Ready to transmit.");
  Serial.println("Hold OK button for 2 seconds to enter menu");
  Serial.println("Left trigger down = ARM system");
  Serial.println("Factory Reset available in main menu");
  Serial.println("=====================================");
}

void loop() {
  // Update menu system first (handles OK button long press and factory reset)
  updateMenu();
  
  // Read controls (includes calibrated joystick values)
  readJoysticks();
  
  // Transmit data every 20ms (50Hz) - only if not in active calibration
  if (millis() - lastTransmit >= TRANSMIT_INTERVAL) {
    transmitData();
    lastTransmit = millis();
  }
  
  // Update display every 50ms (20Hz)
  if (millis() - lastDisplayUpdate >= DISPLAY_INTERVAL) {
    updateDisplay(); // Automatically switches between main and menu display
    lastDisplayUpdate = millis();
  }
  
  // Check buttons (includes arming system)
  checkButtons();
  
  // Only update LEDs when state actually changes
  bool currentArmedState = getArmedStatus();
  bool currentMenuState = isMenuActive();
  
  if (millis() - lastLEDUpdate > 1000 || // Update every second as backup
      currentArmedState != lastArmedState || // Armed state changed
      currentMenuState != lastMenuState) {   // Menu state changed
    
    extern void applyLEDSettings();
    applyLEDSettings();
    
    lastLEDUpdate = millis();
    lastArmedState = currentArmedState;
    lastMenuState = currentMenuState;
  }
  
  // Optional debug output
  static unsigned long lastDebug = 0;
  if (millis() - lastDebug > 10000) { // Every 10 seconds (reduced frequency)
    printSystemStatus();
    lastDebug = millis();
  }
}

void printSystemStatus() {
  Serial.println("--- System Status ---");
  Serial.print("Armed: "); Serial.println(getArmedStatus() ? "YES" : "NO");
  Serial.print("Radio: "); Serial.println(isRadioOK() ? "OK" : "FAILED");
  Serial.print("Menu Active: "); Serial.println(isMenuActive() ? "YES" : "NO");
  Serial.print("Factory Reset Active: "); Serial.println(isFactoryResetActive() ? "YES" : "NO");
  Serial.print("Throttle: "); Serial.print(data.throttle); 
  Serial.print(" Steering: "); Serial.println(data.steering);
  Serial.print("Packets sent: "); Serial.println(data.counter);
  
  // LED status debug
  extern SettingsData settings;
  Serial.print("LED Enabled: "); Serial.println(settings.ledEnabled ? "YES" : "NO");
  Serial.print("Armed Color: R:");
  Serial.print(settings.ledArmedColor[0] ? "1" : "0");
  Serial.print(" G:");
  Serial.print(settings.ledArmedColor[1] ? "1" : "0");
  Serial.print(" B:");
  Serial.println(settings.ledArmedColor[2] ? "1" : "0");
  
  Serial.println("Hold OK for menu, Left trigger to arm");
  Serial.println("Factory Reset: Main Menu > Factory Reset");
  Serial.println("--------------------");
}