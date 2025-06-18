/*
  menu_data.h - Enhanced Data Structures with Range Settings and Factory Reset
  RC Transmitter for Teensy 4.0
*/

#ifndef MENU_DATA_H
#define MENU_DATA_H

#include <EEPROM.h>
#include "config.h"

// Menu states (enhanced with range settings, MPU6500 removed, factory reset added)
enum MenuState {
  MENU_HIDDEN,
  MENU_MAIN,
  MENU_CALIBRATION,
  MENU_JOYSTICK_CAL,
  MENU_POTENTIOMETER_CAL,
  MENU_SETTINGS,
  MENU_DEADZONE_SETTING,
  MENU_BRIGHTNESS_SETTING,
  MENU_LED_SETTINGS,
  MENU_LED_COLOR_SETTING,
  MENU_RADIO_ADDRESS,
  MENU_FAILSAFE_SETTINGS,
  MENU_FAILSAFE_THROTTLE_SETTING,
  MENU_FAILSAFE_STEERING_SETTING,
  MENU_CHANNEL_SETTINGS,
  MENU_RANGE_SETTINGS,           // Range settings menu
  MENU_THROTTLE_MIN_SETTING,     // Throttle minimum PWM
  MENU_THROTTLE_MAX_SETTING,     // Throttle maximum PWM
  MENU_STEER_MIN_SETTING,        // Steering minimum degrees
  MENU_STEER_NEUTRAL_SETTING,    // Steering neutral/center degrees
  MENU_STEER_MAX_SETTING,        // Steering maximum degrees
  MENU_INFO,
  MENU_CAL_IN_PROGRESS,
  MENU_CANCEL_CONFIRM,
  MENU_RADIO_TEST,               // Added for radio test
  MENU_FACTORY_RESET_CONFIRM,    // First factory reset confirmation
  MENU_FACTORY_RESET_FINAL,      // Final factory reset confirmation
  MENU_FACTORY_RESET_PROGRESS,    // Factory reset progress animation
  MENU_DISPLAY_TEST
};

// LED Color modes
enum LEDColorMode {
  LED_COLOR_ARMED,
  LED_COLOR_DISARMED,
  LED_COLOR_TRANSMITTING,
  LED_COLOR_ERROR,
  LED_COLOR_MENU
};

// Calibration states (MPU6500 states removed)
enum CalibrationState {
  CAL_IDLE,
  CAL_NEUTRAL,
  CAL_MAX,
  CAL_MIN,
  CAL_COMPLETE
};

// Menu item structure
struct MenuItem {
  String title;
  bool enabled;
  bool hasSubmenu;
};

// Enhanced settings data structure
struct SettingsData {
  // Joystick settings
  int joystickDeadzone;  // 0-200
  
  // Display settings
  int displayBrightness;  // 0-255 (contrast)
  
  // LED settings
  bool ledEnabled;
  bool ledArmedColor[3];      // RGB for armed state
  bool ledDisarmedColor[3];   // RGB for disarmed state
  bool ledTransmitColor[3];   // RGB for transmitting
  bool ledErrorColor[3];      // RGB for error state
  bool ledMenuColor[3];       // RGB for menu mode
  
  // Radio settings
  char radioAddress[6];       // 5 characters + null terminator
  int radioChannel;           // 0-125
  
  // Failsafe settings
  int failsafeThrottle;       // -1000 to 1000
  int failsafeSteering;       // -1000 to 1000
  bool failsafeEnabled;
  
  // NEW: Range settings
  int throttleMinPWM;         // 1000-2000 microseconds
  int throttleMaxPWM;         // 1000-2000 microseconds
  int steerMinDegree;         // -90 to +90 degrees
  int steerNeutralDegree;     // -90 to +90 degrees (center position)
  int steerMaxDegree;         // -90 to +90 degrees
  
  // EEPROM signature
  uint16_t signature;
};

// Calibration data structure (MPU6500 fields removed)
struct CalibrationData {
  // Individual joystick axis calibration
  int rightJoyX_min, rightJoyX_neutral, rightJoyX_max;
  int rightJoyY_min, rightJoyY_neutral, rightJoyY_max;
  int leftJoyX_min, leftJoyX_neutral, leftJoyX_max;
  int leftJoyY_min, leftJoyY_neutral, leftJoyY_max;
  
  // Individual potentiometer calibration
  int leftPot_min, leftPot_neutral, leftPot_max;
  int rightPot_min, rightPot_neutral, rightPot_max;
  
  // Individual calibration validity flags
  bool rightJoyX_calibrated, rightJoyY_calibrated;
  bool leftJoyX_calibrated, leftJoyY_calibrated;
  bool leftPot_calibrated, rightPot_calibrated;
  
  // EEPROM signature
  uint16_t signature;
};

// Global data instances
SettingsData settings;
CalibrationData calData;

// Factory defaults instance
FactoryDefaults factoryDefaults;

// Factory reset variables
bool factoryResetActive = false;
int factoryResetStep = 0; // 0=clearing, 1=creating, 2=done
unsigned long factoryResetStartTime = 0;
unsigned long factoryResetStepTime = 0;
#define FACTORY_RESET_STEP_DURATION 2000  // 2 seconds per step

// EEPROM addresses - Teensy 4.0 has 4KB (4096 bytes) of emulated EEPROM
#define EEPROM_CAL_ADDRESS 0
#define EEPROM_SETTINGS_ADDRESS 512
#define EEPROM_SIGNATURE 0xCAFE

// Function declarations
void initMenuData();
void saveSettings();
void loadSettings();
void resetSettings();
void saveCalibration();
void loadCalibration();
void resetCalibration();
void applyLEDSettings();
void applyDisplayBrightness();
void updateDataPacketRanges();
int getCurrentDeadzone();
String getCalibrationStatus(String axis);
int getCalibratedValue(int rawValue, int minVal, int neutralVal, int maxVal);
int getCalibratedSteering();
int getCalibratedThrottle();
int getCalibratedRightJoyY();
int getCalibratedLeftJoyX();
int getCalibratedLeftPot();
int getCalibratedRightPot();
int freeMemory();

// Factory reset function declarations
void startFactoryReset();
void updateFactoryReset();
void drawFactoryResetScreen();
void performFactoryReset();
bool isFactoryResetActive();

// Forward declarations for external functions
extern bool getArmedStatus();
extern void setLED(bool red, bool green, bool blue);
extern bool menuActive;
extern Adafruit_SSD1306 display;

void initMenuData() {
  Serial.println("Loading data from EEPROM...");
  
  // Teensy 4.0 EEPROM doesn't need initialization like ESP boards
  // EEPROM.begin() is not needed on Teensy 4.0
  
  loadCalibration();
  loadSettings();
  applyDisplayBrightness();
  applyLEDSettings();
  updateDataPacketRanges();  // Initialize data packet with current ranges
}

void saveSettings() {
  Serial.println("Saving settings to EEPROM...");
  settings.signature = EEPROM_SIGNATURE;
  
  // Teensy 4.0 EEPROM doesn't need commit() - it writes immediately
  EEPROM.put(EEPROM_SETTINGS_ADDRESS, settings);
  
  Serial.println("Settings saved to EEPROM");
  
  // Apply settings immediately after saving
  applyLEDSettings();
  applyDisplayBrightness();
  updateDataPacketRanges();  // Update data packet when settings change
}

void loadSettings() {
  EEPROM.get(EEPROM_SETTINGS_ADDRESS, settings);
  
  if (settings.signature != EEPROM_SIGNATURE) {
    Serial.println("No valid settings found, using defaults");
    resetSettings();
  } else {
    Serial.println("Settings loaded from EEPROM");
  }
}

void resetSettings() {
  // Default settings
  settings.joystickDeadzone = 50;
  settings.displayBrightness = 150;
  settings.ledEnabled = true;
  
  // Default LED colors (RGB)
  settings.ledArmedColor[0] = false; settings.ledArmedColor[1] = true; settings.ledArmedColor[2] = false; // Green
  settings.ledDisarmedColor[0] = true; settings.ledDisarmedColor[1] = false; settings.ledDisarmedColor[2] = false; // Red
  settings.ledTransmitColor[0] = false; settings.ledTransmitColor[1] = false; settings.ledTransmitColor[2] = true; // Blue
  settings.ledErrorColor[0] = true; settings.ledErrorColor[1] = true; settings.ledErrorColor[2] = false; // Yellow
  settings.ledMenuColor[0] = true; settings.ledMenuColor[1] = false; settings.ledMenuColor[2] = true; // Magenta
  
  // Default radio settings
  strcpy(settings.radioAddress, "BOAT1");
  settings.radioChannel = 76;
  
  // Default failsafe settings
  settings.failsafeThrottle = 0;
  settings.failsafeSteering = 0;
  settings.failsafeEnabled = true;
  
  // Default range settings
  settings.throttleMinPWM = 1100;      // Conservative minimum
  settings.throttleMaxPWM = 1900;      // Conservative maximum
  settings.steerMinDegree = -65;       // Tested optimal left
  settings.steerNeutralDegree = 0;     // Proper center position
  settings.steerMaxDegree = 40;        // Tested optimal right
  
  settings.signature = EEPROM_SIGNATURE;
}

// Function to update data packet with current range settings
void updateDataPacketRanges() {
  data.throttle_min_pwm = settings.throttleMinPWM;
  data.throttle_max_pwm = settings.throttleMaxPWM;
  data.steer_min_degree = settings.steerMinDegree;
  data.steer_neutral_degree = settings.steerNeutralDegree;
  data.steer_max_degree = settings.steerMaxDegree;
  data.config_changed = 1;  // Flag that config has changed
  
  Serial.println("Data packet updated with range settings:");
  Serial.print("  Throttle PWM: ");
  Serial.print(settings.throttleMinPWM);
  Serial.print(" - ");
  Serial.println(settings.throttleMaxPWM);
  Serial.print("  Steering Degrees: ");
  Serial.print(settings.steerMinDegree);
  Serial.print(" | ");
  Serial.print(settings.steerNeutralDegree);
  Serial.print(" | ");
  Serial.println(settings.steerMaxDegree);
}

void saveCalibration() {
  calData.signature = EEPROM_SIGNATURE;
  
  // Teensy 4.0 EEPROM doesn't need commit() - it writes immediately
  EEPROM.put(EEPROM_CAL_ADDRESS, calData);
  
  Serial.println("Calibration saved to EEPROM");
}

void loadCalibration() {
  EEPROM.get(EEPROM_CAL_ADDRESS, calData);
  
  if (calData.signature != EEPROM_SIGNATURE) {
    Serial.println("No valid calibration found, using defaults");
    resetCalibration();
  } else {
    Serial.println("Calibration loaded from EEPROM");
  }
}

void resetCalibration() {
  // Set default values for all axes
  calData.rightJoyX_min = 0; calData.rightJoyX_neutral = 512; calData.rightJoyX_max = 1023;
  calData.rightJoyY_min = 0; calData.rightJoyY_neutral = 512; calData.rightJoyY_max = 1023;
  calData.leftJoyX_min = 0; calData.leftJoyX_neutral = 512; calData.leftJoyX_max = 1023;
  calData.leftJoyY_min = 0; calData.leftJoyY_neutral = 512; calData.leftJoyY_max = 1023;
  
  calData.leftPot_min = 0; calData.leftPot_neutral = 512; calData.leftPot_max = 1023;
  calData.rightPot_min = 0; calData.rightPot_neutral = 512; calData.rightPot_max = 1023;
  
  // Reset all calibration flags
  calData.rightJoyX_calibrated = false; calData.rightJoyY_calibrated = false;
  calData.leftJoyX_calibrated = false; calData.leftJoyY_calibrated = false;
  calData.leftPot_calibrated = false; calData.rightPot_calibrated = false;
  
  calData.signature = EEPROM_SIGNATURE;
}

void applyLEDSettings() {
  // Check if LEDs are disabled first
  if (!settings.ledEnabled) {
    setLED(false, false, false);  // Turn off all LEDs
    Serial.println("LEDs disabled - all LEDs turned off");
    return;
  }
  
  bool* color;
  
  // Determine which color to use based on context
  if (menuActive) {
    color = settings.ledMenuColor;
    Serial.print("LED: Menu mode - ");
  } else if (getArmedStatus()) {
    color = settings.ledArmedColor;
    Serial.print("LED: Armed mode - ");
  } else {
    color = settings.ledDisarmedColor;
    Serial.print("LED: Disarmed mode - ");
  }
  
  // Apply the selected color
  setLED(color[0], color[1], color[2]);
  
  // Enhanced debug output
  Serial.print("R:");
  Serial.print(color[0] ? "ON" : "OFF");
  Serial.print(" G:");
  Serial.print(color[1] ? "ON" : "OFF");
  Serial.print(" B:");
  Serial.print(color[2] ? "ON" : "OFF");
  Serial.print(" (Enabled: ");
  Serial.print(settings.ledEnabled ? "YES" : "NO");
  Serial.print(", Armed: ");
  Serial.print(getArmedStatus() ? "YES" : "NO");
  Serial.print(", Menu: ");
  Serial.print(menuActive ? "YES" : "NO");
  Serial.println(")");
}

void applyDisplayBrightness() {
  // Apply contrast setting to the display
  display.ssd1306_command(0x81); // Set contrast command
  display.ssd1306_command(settings.displayBrightness); // Contrast value (0-255)
  Serial.print("Display brightness set to: ");
  Serial.println(settings.displayBrightness);
}

int getCurrentDeadzone() {
  return settings.joystickDeadzone;
}

String getCalibrationStatus(String axis) {
  if (axis == "RIGHT_X") return calData.rightJoyX_calibrated ? "[OK]" : "[--]";
  if (axis == "RIGHT_Y") return calData.rightJoyY_calibrated ? "[OK]" : "[--]";
  if (axis == "LEFT_X") return calData.leftJoyX_calibrated ? "[OK]" : "[--]";
  if (axis == "LEFT_Y") return calData.leftJoyY_calibrated ? "[OK]" : "[--]";
  if (axis == "LEFT_POT") return calData.leftPot_calibrated ? "[OK]" : "[--]";
  if (axis == "RIGHT_POT") return calData.rightPot_calibrated ? "[OK]" : "[--]";
  return "[--]";
}

// Calibrated value functions
int getCalibratedValue(int rawValue, int minVal, int neutralVal, int maxVal) {
  if (rawValue <= neutralVal) {
    return map(rawValue, minVal, neutralVal, -1000, 0);
  } else {
    return map(rawValue, neutralVal, maxVal, 0, 1000);
  }
}

int getCalibratedSteering() {
  if (!calData.rightJoyX_calibrated) {
    return map(analogRead(RIGHT_JOY_X), 0, 1023, 1000, -1000);
  }
  int value = getCalibratedValue(analogRead(RIGHT_JOY_X), 
                                calData.rightJoyX_min, 
                                calData.rightJoyX_neutral, 
                                calData.rightJoyX_max);
  // Apply deadzone
  if (abs(value) < settings.joystickDeadzone) value = 0;
  return value;
}

int getCalibratedThrottle() {
  if (!calData.leftJoyY_calibrated) {
    return map(analogRead(LEFT_JOY_Y), 0, 1023, -1000, 1000);
  }
  int value = getCalibratedValue(analogRead(LEFT_JOY_Y), 
                                calData.leftJoyY_min, 
                                calData.leftJoyY_neutral, 
                                calData.leftJoyY_max);
  // Apply deadzone
  if (abs(value) < settings.joystickDeadzone) value = 0;
  return value;
}

// Additional calibrated functions for future use
int getCalibratedRightJoyY() {
  if (!calData.rightJoyY_calibrated) {
    return map(analogRead(RIGHT_JOY_Y), 0, 1023, -1000, 1000);
  }
  return getCalibratedValue(analogRead(RIGHT_JOY_Y), 
                           calData.rightJoyY_min, 
                           calData.rightJoyY_neutral, 
                           calData.rightJoyY_max);
}

int getCalibratedLeftJoyX() {
  if (!calData.leftJoyX_calibrated) {
    return map(analogRead(LEFT_JOY_X), 0, 1023, -1000, 1000);
  }
  return getCalibratedValue(analogRead(LEFT_JOY_X), 
                           calData.leftJoyX_min, 
                           calData.leftJoyX_neutral, 
                           calData.leftJoyX_max);
}

int getCalibratedLeftPot() {
  if (!calData.leftPot_calibrated) {
    return map(analogRead(LEFT_POT), 0, 1023, -1000, 1000);
  }
  return getCalibratedValue(analogRead(LEFT_POT), 
                           calData.leftPot_min, 
                           calData.leftPot_neutral, 
                           calData.leftPot_max);
}

int getCalibratedRightPot() {
  if (!calData.rightPot_calibrated) {
    return map(analogRead(RIGHT_POT), 0, 1023, -1000, 1000);
  }
  return getCalibratedValue(analogRead(RIGHT_POT), 
                           calData.rightPot_min, 
                           calData.rightPot_neutral, 
                           calData.rightPot_max);
}

// Utility function to get free memory - Teensy specific implementation
int freeMemory() {
  // Teensy 4.0 has much more RAM (1MB) than Arduino Mega
  // This is an approximate implementation for compatibility
  uint32_t stackTop;
  uint32_t heapTop;
  
  // Current position of the stack
  stackTop = (uint32_t)&stackTop;
  
  // Current position of heap
  void* hTop = malloc(1);
  heapTop = (uint32_t)hTop;
  free(hTop);
  
  // The free space is approximately the difference between heap and stack
  return (stackTop - heapTop);
}

// Factory reset functions
void startFactoryReset() {
  Serial.println("Starting factory reset process...");
  factoryResetActive = true;
  factoryResetStep = 0;
  factoryResetStartTime = millis();
  factoryResetStepTime = millis();
  
  // Start the actual reset process
  performFactoryReset();
}

void updateFactoryReset() {
  if (!factoryResetActive) return;
  
  // Update progress steps
  if (millis() - factoryResetStepTime >= FACTORY_RESET_STEP_DURATION) {
    factoryResetStep++;
    factoryResetStepTime = millis();
    
    if (factoryResetStep >= 3) {
      // Factory reset complete
      factoryResetActive = false;
      Serial.println("Factory reset completed!");
      
      // Return to main menu
      extern MenuState currentMenu;
      extern int maxMenuItems;
      extern int menuSelection;
      extern int menuOffset;
      currentMenu = MENU_MAIN;
      maxMenuItems = 8;
      menuSelection = 0;
      menuOffset = 0;
    }
  }
}

void performFactoryReset() {
  Serial.println("Performing factory reset - clearing EEPROM and restoring defaults...");
  
  // Clear EEPROM completely (write zeros)
  for (int i = 0; i < 1024; i++) {
    EEPROM.write(i, 0);
  }
  
  // Apply factory defaults to settings
  settings.joystickDeadzone = factoryDefaults.joystickDeadzone;
  settings.displayBrightness = factoryDefaults.displayBrightness;
  settings.ledEnabled = factoryDefaults.ledEnabled;
  
  // LED colors
  for (int i = 0; i < 3; i++) {
    settings.ledArmedColor[i] = factoryDefaults.ledArmedColor[i];
    settings.ledDisarmedColor[i] = factoryDefaults.ledDisarmedColor[i];
    settings.ledTransmitColor[i] = factoryDefaults.ledTransmitColor[i];
    settings.ledErrorColor[i] = factoryDefaults.ledErrorColor[i];
    settings.ledMenuColor[i] = factoryDefaults.ledMenuColor[i];
  }
  
  // Radio settings
  strcpy(settings.radioAddress, factoryDefaults.radioAddress);
  settings.radioChannel = factoryDefaults.radioChannel;
  
  // Failsafe settings
  settings.failsafeThrottle = factoryDefaults.failsafeThrottle;
  settings.failsafeSteering = factoryDefaults.failsafeSteering;
  settings.failsafeEnabled = factoryDefaults.failsafeEnabled;
  
  // Range settings
  settings.throttleMinPWM = factoryDefaults.throttleMinPWM;
  settings.throttleMaxPWM = factoryDefaults.throttleMaxPWM;
  settings.steerMinDegree = factoryDefaults.steerMinDegree;
  settings.steerNeutralDegree = factoryDefaults.steerNeutralDegree;
  settings.steerMaxDegree = factoryDefaults.steerMaxDegree;
  
  settings.signature = EEPROM_SIGNATURE;
  
  // Apply factory defaults to calibration
  calData.rightJoyX_min = factoryDefaults.rightJoyX_min;
  calData.rightJoyX_neutral = factoryDefaults.rightJoyX_neutral;
  calData.rightJoyX_max = factoryDefaults.rightJoyX_max;
  calData.rightJoyY_min = factoryDefaults.rightJoyY_min;
  calData.rightJoyY_neutral = factoryDefaults.rightJoyY_neutral;
  calData.rightJoyY_max = factoryDefaults.rightJoyY_max;
  calData.leftJoyX_min = factoryDefaults.leftJoyX_min;
  calData.leftJoyX_neutral = factoryDefaults.leftJoyX_neutral;
  calData.leftJoyX_max = factoryDefaults.leftJoyX_max;
  calData.leftJoyY_min = factoryDefaults.leftJoyY_min;
  calData.leftJoyY_neutral = factoryDefaults.leftJoyY_neutral;
  calData.leftJoyY_max = factoryDefaults.leftJoyY_max;
  calData.leftPot_min = factoryDefaults.leftPot_min;
  calData.leftPot_neutral = factoryDefaults.leftPot_neutral;
  calData.leftPot_max = factoryDefaults.leftPot_max;
  calData.rightPot_min = factoryDefaults.rightPot_min;
  calData.rightPot_neutral = factoryDefaults.rightPot_neutral;
  calData.rightPot_max = factoryDefaults.rightPot_max;
  
  // Reset calibration flags (all uncalibrated)
  calData.rightJoyX_calibrated = false;
  calData.rightJoyY_calibrated = false;
  calData.leftJoyX_calibrated = false;
  calData.leftJoyY_calibrated = false;
  calData.leftPot_calibrated = false;
  calData.rightPot_calibrated = false;
  
  calData.signature = EEPROM_SIGNATURE;
  
  // Save to EEPROM
  saveSettings();
  saveCalibration();
  
  // Apply settings immediately
  applyLEDSettings();
  applyDisplayBrightness();
  updateDataPacketRanges();
  
  Serial.println("Factory reset data applied and saved to EEPROM");
}

void drawFactoryResetScreen() {
  // White background, black text
  display.fillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, SSD1306_WHITE);
  display.setTextColor(SSD1306_BLACK);
  display.setTextSize(1);
  
  // Progress calculation (0 to 1)
  unsigned long stepElapsed = millis() - factoryResetStepTime;
  float stepProgress = (float)stepElapsed / (float)FACTORY_RESET_STEP_DURATION;
  if (stepProgress > 1.0) stepProgress = 1.0;
  
  // Overall progress (3 steps total)
  float overallProgress = ((float)factoryResetStep + stepProgress) / 3.0;
  if (overallProgress > 1.0) overallProgress = 1.0;
  
  // Step text - positioned in yellow region (0-15 pixels)
  String stepText;
  switch (factoryResetStep) {
    case 0: stepText = "Clearing Settings"; break;
    case 1: stepText = "Creating Settings"; break;
    case 2: stepText = "Done Resetting"; break;
    default: stepText = "Factory Reset"; break;
  }
  
  // Center the step text in yellow region
  int textWidth = stepText.length() * 6;
  int textX = (SCREEN_WIDTH - textWidth) / 2;
  display.setCursor(textX, 4); // Y=4 to center in yellow region (0-15)
  display.println(stepText);
  
  // Progress bar - positioned in blue region
  int progressBarX = 20;
  int progressBarY = 25; // Blue region starts at 16, so 25 is well within it
  int progressBarWidth = SCREEN_WIDTH - 40;
  int progressBarHeight = 10;
  
  // Draw progress bar outline
  display.drawRect(progressBarX, progressBarY, progressBarWidth, progressBarHeight, SSD1306_BLACK);
  
  // Fill progress bar
  int fillWidth = (int)(overallProgress * (progressBarWidth - 2));
  if (fillWidth > 0) {
    display.fillRect(progressBarX + 1, progressBarY + 1, fillWidth, progressBarHeight - 2, SSD1306_BLACK);
  }
  
  // Percentage text - positioned in blue region
  String percentText = String((int)(overallProgress * 100)) + "%";
  int percentWidth = percentText.length() * 6;
  int percentX = (SCREEN_WIDTH - percentWidth) / 2;
  display.setCursor(percentX, 40); // Blue region
  display.println(percentText);
  
  // Additional status info - positioned in blue region
  display.setCursor(10, 55); // Blue region
  if (factoryResetStep < 2) {
    display.print("Please wait...");
  } else {
    display.print("Reset Complete!");
  }
  
  // Reset text color for other functions
  display.setTextColor(SSD1306_WHITE);
}

bool isFactoryResetActive() {
  return factoryResetActive;
}

#endif // MENU_DATA_H