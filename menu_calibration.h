/*
  menu_calibration.h - Calibration Management (UPDATED FOR TEENSY 4.0, MPU6500 REMOVED)
  RC Transmitter for Teensy 4.0
*/

#ifndef MENU_CALIBRATION_H
#define MENU_CALIBRATION_H

#include <Wire.h>
#include "config.h"
#include "display.h"
#include "menu_data.h"

// Calibration variables
bool calibrationActive = false;
String currentCalType = "";
String currentCalAxis = "";
int calStep = 0;
int maxCalSteps = 0;
bool waitingForOK = false;
CalibrationState calState = CAL_IDLE;

// External variables from menu.h
extern MenuState currentMenu;
extern int menuSelection;
extern int menuOffset;
extern int maxMenuItems;
extern unsigned long lastNavigation;

// Function declarations
void initMenuCalibration();
void updateMenuCalibration();
void startCalibration(String calType, String axis);
void completeCalibration();
void exitMenuCalibration();
void goBackCalibration();
void drawMenuCalibration();
void drawCalibrationScreen();
String getCalibrationStepText();
bool isCalibrationActive();

void initMenuCalibration() {
  calibrationActive = false;
}

void updateMenuCalibration() {
  if (!waitingForOK) return;
  
  // During calibration, check for both OK button and left joystick button
  static bool lastOKState = false;
  static bool lastLeftJoyState = false;
  bool currentOKState = buttons.btnOK;
  bool currentLeftJoyState = buttons.leftJoyBtn;
  
  // ADDED: Check for left joystick button press (back/cancel functionality)
  if (currentLeftJoyState && !lastLeftJoyState) {
    Serial.println("Left joystick pressed during calibration - going back");
    
    // Cancel current calibration and go back to appropriate menu
    calibrationActive = false;
    waitingForOK = false;
    calState = CAL_IDLE;
    
    // Return to the appropriate parent menu
    if (currentCalType == "JOYSTICK") {
      currentMenu = MENU_JOYSTICK_CAL;
      maxMenuItems = 5;
    } else if (currentCalType == "POTENTIOMETER") {
      currentMenu = MENU_POTENTIOMETER_CAL;
      maxMenuItems = 3;
    } else {
      // Default fallback
      currentMenu = MENU_CALIBRATION;
      maxMenuItems = 3; // Reduced from 4 after removing MPU6500
    }
    
    menuSelection = 0;
    menuOffset = 0;
    
    Serial.println("Calibration cancelled - returned to menu");
    lastLeftJoyState = currentLeftJoyState;
    return;
  }
  
  // Check for OK button press (rising edge detection) - ORIGINAL FUNCTIONALITY
  if (currentOKState && !lastOKState) {
    Serial.println("OK pressed during calibration");
    
    if (currentCalType == "JOYSTICK") {
      int rawValue = 0;
      
      // Read the specific axis
      if (currentCalAxis == "RIGHT_X") rawValue = analogRead(RIGHT_JOY_X);
      else if (currentCalAxis == "RIGHT_Y") rawValue = analogRead(RIGHT_JOY_Y);
      else if (currentCalAxis == "LEFT_X") rawValue = analogRead(LEFT_JOY_X);
      else if (currentCalAxis == "LEFT_Y") rawValue = analogRead(LEFT_JOY_Y);
      
      // Store calibration values for specific axis
      switch (calState) {
        case CAL_NEUTRAL:
          if (currentCalAxis == "RIGHT_X") calData.rightJoyX_neutral = rawValue;
          else if (currentCalAxis == "RIGHT_Y") calData.rightJoyY_neutral = rawValue;
          else if (currentCalAxis == "LEFT_X") calData.leftJoyX_neutral = rawValue;
          else if (currentCalAxis == "LEFT_Y") calData.leftJoyY_neutral = rawValue;
          calState = CAL_MAX;
          break;
        case CAL_MAX:
          if (currentCalAxis == "RIGHT_X") calData.rightJoyX_max = rawValue;
          else if (currentCalAxis == "RIGHT_Y") calData.rightJoyY_max = rawValue;
          else if (currentCalAxis == "LEFT_X") calData.leftJoyX_max = rawValue;
          else if (currentCalAxis == "LEFT_Y") calData.leftJoyY_max = rawValue;
          calState = CAL_MIN;
          break;
        case CAL_MIN:
          if (currentCalAxis == "RIGHT_X") {
            calData.rightJoyX_min = rawValue;
            calData.rightJoyX_calibrated = true;
          } else if (currentCalAxis == "RIGHT_Y") {
            calData.rightJoyY_min = rawValue;
            calData.rightJoyY_calibrated = true;
          } else if (currentCalAxis == "LEFT_X") {
            calData.leftJoyX_min = rawValue;
            calData.leftJoyX_calibrated = true;
          } else if (currentCalAxis == "LEFT_Y") {
            calData.leftJoyY_min = rawValue;
            calData.leftJoyY_calibrated = true;
          }
          completeCalibration();
          break;
        default:
          break;
      }
    } else if (currentCalType == "POTENTIOMETER") {
      int rawValue = 0;
      
      if (currentCalAxis == "LEFT") rawValue = analogRead(LEFT_POT);
      else if (currentCalAxis == "RIGHT") rawValue = analogRead(RIGHT_POT);
      
      switch (calState) {
        case CAL_NEUTRAL:
          if (currentCalAxis == "LEFT") calData.leftPot_neutral = rawValue;
          else if (currentCalAxis == "RIGHT") calData.rightPot_neutral = rawValue;
          calState = CAL_MAX;
          break;
        case CAL_MAX:
          if (currentCalAxis == "LEFT") calData.leftPot_max = rawValue;
          else if (currentCalAxis == "RIGHT") calData.rightPot_max = rawValue;
          calState = CAL_MIN;
          break;
        case CAL_MIN:
          if (currentCalAxis == "LEFT") {
            calData.leftPot_min = rawValue;
            calData.leftPot_calibrated = true;
          } else if (currentCalAxis == "RIGHT") {
            calData.rightPot_min = rawValue;
            calData.rightPot_calibrated = true;
          }
          completeCalibration();
          break;
        default:
          break;
      }
    }
    
    calStep++;
  }
  
  lastOKState = currentOKState;
  lastLeftJoyState = currentLeftJoyState;
}

void startCalibration(String calType, String axis) {
  Serial.print("Starting calibration: ");
  Serial.print(calType);
  if (axis != "") {
    Serial.print(" - ");
    Serial.print(axis);
  }
  Serial.println();
  
  calibrationActive = true;
  currentCalType = calType;
  currentCalAxis = axis;
  calStep = 0;
  waitingForOK = true;
  currentMenu = MENU_CAL_IN_PROGRESS;
  
  if (calType == "JOYSTICK" || calType == "POTENTIOMETER") {
    maxCalSteps = 3; // Neutral, Max, Min
    calState = CAL_NEUTRAL;
  }
}

void completeCalibration() {
  Serial.print("Calibration complete: ");
  Serial.print(currentCalType);
  if (currentCalAxis != "") {
    Serial.print(" - ");
    Serial.print(currentCalAxis);
  }
  Serial.println();
  
  saveCalibration();
  calibrationActive = false;
  
  // Return to appropriate menu
  if (currentCalType == "JOYSTICK") {
    currentMenu = MENU_JOYSTICK_CAL;
    maxMenuItems = 5;
  } else if (currentCalType == "POTENTIOMETER") {
    currentMenu = MENU_POTENTIOMETER_CAL;
    maxMenuItems = 3;
  } else {
    currentMenu = MENU_CALIBRATION;
    maxMenuItems = 3; // Reduced from 4 after removing MPU6500
  }
  
  menuSelection = 0;
  menuOffset = 0;
  calState = CAL_IDLE;
  
  delay(1000);
}

void exitMenuCalibration() {
  calibrationActive = false;
  calState = CAL_IDLE;
}

void goBackCalibration() {
  // Return to calibration menu if in progress
  if (calibrationActive) {
    calibrationActive = false;
    currentMenu = MENU_CALIBRATION;
    maxMenuItems = 3; // Reduced from 4 after removing MPU6500
    menuSelection = 0;
    menuOffset = 0;
  }
}

bool isCalibrationActive() {
  return calibrationActive;
}

void drawMenuCalibration() {
  drawCalibrationScreen();
}

void drawCalibrationScreen() {
  // Header
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("Cal: ");
  display.print(currentCalType);
  if (currentCalAxis != "") {
    display.print(" ");
    display.print(currentCalAxis);
  }
  
  // Progress
  display.setCursor(0, 16);
  display.print("Step ");
  display.print(calStep + 1);
  display.print("/");
  display.print(maxCalSteps);
  
  // Instructions
  display.setCursor(0, 28);
  display.println(getCalibrationStepText());
  
  // Current value display
  if (currentCalType == "JOYSTICK") {
    display.setCursor(0, 42);
    if (currentCalAxis == "RIGHT_X") {
      display.print("Value: ");
      display.print(analogRead(RIGHT_JOY_X));
    } else if (currentCalAxis == "RIGHT_Y") {
      display.print("Value: ");
      display.print(analogRead(RIGHT_JOY_Y));
    } else if (currentCalAxis == "LEFT_X") {
      display.print("Value: ");
      display.print(analogRead(LEFT_JOY_X));
    } else if (currentCalAxis == "LEFT_Y") {
      display.print("Value: ");
      display.print(analogRead(LEFT_JOY_Y));
    }
  } else if (currentCalType == "POTENTIOMETER") {
    display.setCursor(0, 42);
    if (currentCalAxis == "LEFT") {
      display.print("Value: ");
      display.print(analogRead(LEFT_POT));
    } else if (currentCalAxis == "RIGHT") {
      display.print("Value: ");
      display.print(analogRead(RIGHT_POT));
    }
  }
  
  // Show both OK and Back instructions
  display.setCursor(0, 52);
  display.print("OK: Continue");
}

String getCalibrationStepText() {
  if (currentCalType == "JOYSTICK" || currentCalType == "POTENTIOMETER") {
    switch (calState) {
      case CAL_NEUTRAL: return "Move to CENTER";
      case CAL_MAX: return "Move to MAXIMUM";
      case CAL_MIN: return "Move to MINIMUM";
      default: return "Unknown";
    }
  }
  return "Unknown";
}

#endif

