/*
  test_buttons.h - Complete Input Testing Functions
  RC Transmitter for Teensy 4.0
  
  Tests ALL inputs including:
  - Joysticks (Left X/Y, Right X/Y with movement detection)
  - Potentiometers (Left, Right with movement detection)
  - Arrow buttons (Up, Down, Left, Right)
  - OK button
  - Joystick buttons (Left, Right)
  - Trigger switches (Left Up/Down, Right Up/Down)
  
  Exit: Press Up Arrow + Down Arrow together
*/

#ifndef TEST_BUTTONS_H
#define TEST_BUTTONS_H

#include "config.h"
#include "display.h"
#include "controls.h"

// Trigger state enum
enum TriggerState {
  TRIGGER_UP = 0,
  TRIGGER_MIDDLE = 1,
  TRIGGER_DOWN = 2
};

// Button test variables
bool buttonTestActive = false;
unsigned long buttonTestStartTime = 0;
bool lastUpArrow = false;
bool lastDownArrow = false;
bool exitCombinationDetected = false;

// Test result tracking
struct InputTestResults {
  // Joystick movement detection
  bool leftJoyXMoved = false;
  bool leftJoyYMoved = false;
  bool rightJoyXMoved = false;
  bool rightJoyYMoved = false;
  
  // Potentiometer movement detection
  bool leftPotMoved = false;
  bool rightPotMoved = false;
  
  // Button states
  bool arrowUp = false;
  bool arrowDown = false;
  bool arrowLeft = false;
  bool arrowRight = false;
  bool okButton = false;
  bool leftJoyBtn = false;
  bool rightJoyBtn = false;
  
  // Trigger states
  bool leftTriggerUp = false;
  bool leftTriggerDown = false;
  bool rightTriggerUp = false;
  bool rightTriggerDown = false;
};

InputTestResults inputTestResults;

// Previous values for movement detection
struct PreviousValues {
  int leftJoyX = 512;
  int leftJoyY = 512;
  int rightJoyX = 512;
  int rightJoyY = 512;
  int leftPot = 512;
  int rightPot = 512;
};

PreviousValues prevValues;

// Display page management
int buttonTestPage = 0;
unsigned long pageChangeTime = 0;
#define BUTTON_TEST_PAGES 4
#define PAGE_DURATION 3000

// Function declarations
void startButtonTest();
void updateButtonTest();
void drawButtonTestScreen();
void resetButtonTest();
bool isButtonTestActive();
void checkAllButtons();
void checkJoysticks();
void checkPotentiometers();
void drawButtonTestPage0();
void drawButtonTestPage1();
void drawButtonTestPage2();
void drawButtonTestPage3();
TriggerState readTriggerState(int upPin, int downPin);
String triggerStateToString(TriggerState state);
String getCurrentActiveInput();

void startButtonTest() {
  Serial.println("Starting complete input test...");
  buttonTestActive = true;
  buttonTestStartTime = millis();
  buttonTestPage = 0;
  pageChangeTime = millis();
  exitCombinationDetected = false;
  
  // Reset all test results
  inputTestResults.leftJoyXMoved = false;
  inputTestResults.leftJoyYMoved = false;
  inputTestResults.rightJoyXMoved = false;
  inputTestResults.rightJoyYMoved = false;
  inputTestResults.leftPotMoved = false;
  inputTestResults.rightPotMoved = false;
  inputTestResults.arrowUp = false;
  inputTestResults.arrowDown = false;
  inputTestResults.arrowLeft = false;
  inputTestResults.arrowRight = false;
  inputTestResults.okButton = false;
  inputTestResults.leftJoyBtn = false;
  inputTestResults.rightJoyBtn = false;
  inputTestResults.leftTriggerUp = false;
  inputTestResults.leftTriggerDown = false;
  inputTestResults.rightTriggerUp = false;
  inputTestResults.rightTriggerDown = false;
  
  // Initialize previous values
  prevValues.leftJoyX = analogRead(A1);
  prevValues.leftJoyY = analogRead(A0);
  prevValues.rightJoyX = analogRead(A2);
  prevValues.rightJoyY = analogRead(A3);
  prevValues.leftPot = analogRead(A6);
  prevValues.rightPot = analogRead(A7);
  
  Serial.println("Press UP + DOWN arrows together to exit test");
}

void updateButtonTest() {
  if (!buttonTestActive) return;
  
  // Check all inputs
  checkJoysticks();
  checkPotentiometers();
  checkAllButtons();
  
  // Auto-cycle through display pages
  if (millis() - pageChangeTime >= PAGE_DURATION) {
    buttonTestPage = (buttonTestPage + 1) % BUTTON_TEST_PAGES;
    pageChangeTime = millis();
  }
  
  // Check for exit combination (Up + Down arrows)
  extern ButtonStates buttons;
  bool currentUpArrow = buttons.btnUp;
  bool currentDownArrow = buttons.btnDown;
  
  if (currentUpArrow && currentDownArrow && (!lastUpArrow || !lastDownArrow)) {
    exitCombinationDetected = true;
    Serial.println("Exit combination detected - Up + Down arrows pressed");
  }
  
  lastUpArrow = currentUpArrow;
  lastDownArrow = currentDownArrow;
  
  // Exit test if combination was detected
  if (exitCombinationDetected) {
    buttonTestActive = false;
    
    // Return to main menu
    extern MenuState currentMenu;
    extern int maxMenuItems;
    extern int menuSelection;
    extern int menuOffset;
    currentMenu = MENU_MAIN;
    maxMenuItems = 9;
    menuSelection = 0;
    menuOffset = 0;
    
    Serial.println("Input test exited - returning to main menu");
  }
}

void checkJoysticks() {
  int leftJoyX = analogRead(A1);
  int leftJoyY = analogRead(A0);
  int rightJoyX = analogRead(A2);
  int rightJoyY = analogRead(A3);
  
  if (abs(leftJoyX - prevValues.leftJoyX) > 50) {
    inputTestResults.leftJoyXMoved = true;
    prevValues.leftJoyX = leftJoyX;
  }
  
  if (abs(leftJoyY - prevValues.leftJoyY) > 50) {
    inputTestResults.leftJoyYMoved = true;
    prevValues.leftJoyY = leftJoyY;
  }
  
  if (abs(rightJoyX - prevValues.rightJoyX) > 50) {
    inputTestResults.rightJoyXMoved = true;
    prevValues.rightJoyX = rightJoyX;
  }
  
  if (abs(rightJoyY - prevValues.rightJoyY) > 50) {
    inputTestResults.rightJoyYMoved = true;
    prevValues.rightJoyY = rightJoyY;
  }
}

void checkPotentiometers() {
  int leftPot = analogRead(A6);
  int rightPot = analogRead(A7);
  
  if (abs(leftPot - prevValues.leftPot) > 50) {
    inputTestResults.leftPotMoved = true;
    prevValues.leftPot = leftPot;
  }
  
  if (abs(rightPot - prevValues.rightPot) > 50) {
    inputTestResults.rightPotMoved = true;
    prevValues.rightPot = rightPot;
  }
}

void checkAllButtons() {
  extern ButtonStates buttons;
  
  // Check arrow buttons
  if (buttons.btnUp) inputTestResults.arrowUp = true;
  if (buttons.btnDown) inputTestResults.arrowDown = true;
  if (buttons.btnLeft) inputTestResults.arrowLeft = true;
  if (buttons.btnRight) inputTestResults.arrowRight = true;
  if (buttons.btnOK) inputTestResults.okButton = true;
  
  // Check joystick buttons
  if (buttons.leftJoyBtn) inputTestResults.leftJoyBtn = true;
  if (buttons.rightJoyBtn) inputTestResults.rightJoyBtn = true;
  
  // Check triggers
  TriggerState leftTrigger = readTriggerState(25, 28);
  TriggerState rightTrigger = readTriggerState(0, 1);
  
  if (leftTrigger == TRIGGER_UP) inputTestResults.leftTriggerUp = true;
  if (leftTrigger == TRIGGER_DOWN) inputTestResults.leftTriggerDown = true;
  if (rightTrigger == TRIGGER_UP) inputTestResults.rightTriggerUp = true;
  if (rightTrigger == TRIGGER_DOWN) inputTestResults.rightTriggerDown = true;
}

TriggerState readTriggerState(int upPin, int downPin) {
  bool up = digitalRead(upPin);
  bool down = digitalRead(downPin);
  
  if (!up && down) return TRIGGER_UP;
  if (up && !down) return TRIGGER_DOWN;
  return TRIGGER_MIDDLE;
}

String triggerStateToString(TriggerState state) {
  switch (state) {
    case TRIGGER_UP: return "UP";
    case TRIGGER_DOWN: return "DOWN";
    case TRIGGER_MIDDLE: return "MID";
    default: return "???";
  }
}

String getCurrentActiveInput() {
  static int lastValues[6] = {512, 512, 512, 512, 512, 512};
  
  int currentValues[6] = {
    analogRead(A1), analogRead(A0), analogRead(A2), 
    analogRead(A3), analogRead(A6), analogRead(A7)
  };
  
  String names[6] = {"L-Joy X", "L-Joy Y", "R-Joy X", "R-Joy Y", "L-Pot", "R-Pot"};
  
  int maxChange = 0;
  int activeIndex = -1;
  
  for (int i = 0; i < 6; i++) {
    int change = abs(currentValues[i] - lastValues[i]);
    if (change > maxChange && change > 10) {
      maxChange = change;
      activeIndex = i;
    }
    lastValues[i] = currentValues[i];
  }
  
  if (activeIndex >= 0) {
    return names[activeIndex] + ":" + String(currentValues[activeIndex]);
  }
  
  extern ButtonStates buttons;
  if (buttons.btnUp) return "UP Button";
  if (buttons.btnDown) return "DOWN Button"; 
  if (buttons.btnLeft) return "LEFT Button";
  if (buttons.btnRight) return "RIGHT Button";
  if (buttons.btnOK) return "OK Button";
  if (buttons.leftJoyBtn) return "L-Joy Btn";
  if (buttons.rightJoyBtn) return "R-Joy Btn";
  
  TriggerState leftTrig = readTriggerState(25, 28);
  TriggerState rightTrig = readTriggerState(0, 1);
  if (leftTrig != TRIGGER_MIDDLE) return "L-Trig:" + triggerStateToString(leftTrig);
  if (rightTrig != TRIGGER_MIDDLE) return "R-Trig:" + triggerStateToString(rightTrig);
  
  return "No Activity";
}

void drawButtonTestScreen() {
  display.clearDisplay();
  
  // YELLOW REGION (0-15): Show current activity
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("Active: ");
  display.println(getCurrentActiveInput());
  
  display.setCursor(0, 8);
  display.print("Page ");
  display.print(buttonTestPage + 1);
  display.print("/");
  display.print(BUTTON_TEST_PAGES);
  display.print(" Exit:UP+DOWN");
  
  // BLUE REGION (16-63): Show test content
  switch (buttonTestPage) {
    case 0:
      drawButtonTestPage0();
      break;
    case 1:
      drawButtonTestPage1();
      break;
    case 2:
      drawButtonTestPage2();
      break;
    case 3:
      drawButtonTestPage3();
      break;
  }
  
  display.display();
}

void drawButtonTestPage0() {
  display.setTextSize(1);
  display.setCursor(0, 16);
  display.println("JOYSTICKS");
  
  display.setCursor(0, 26);
  display.print("LX:");
  display.print(analogRead(A1));
  display.print(inputTestResults.leftJoyXMoved ? " OK" : " --");
  
  display.setCursor(65, 26);
  display.print("LY:");
  display.print(analogRead(A0));
  display.print(inputTestResults.leftJoyYMoved ? " OK" : " --");
  
  display.setCursor(0, 36);
  display.print("RX:");
  display.print(analogRead(A2));
  display.print(inputTestResults.rightJoyXMoved ? " OK" : " --");
  
  display.setCursor(65, 36);
  display.print("RY:");
  display.print(analogRead(A3));
  display.print(inputTestResults.rightJoyYMoved ? " OK" : " --");
  
  display.setCursor(0, 48);
  display.println("Move sticks to test");
}

void drawButtonTestPage1() {
  display.setTextSize(1);
  display.setCursor(0, 16);
  display.println("POTENTIOMETERS");
  
  display.setCursor(0, 28);
  display.print("L-Pot:");
  display.print(analogRead(A6));
  display.println(inputTestResults.leftPotMoved ? " OK" : " --");
  
  display.setCursor(0, 38);
  display.print("R-Pot:");
  display.print(analogRead(A7));
  display.println(inputTestResults.rightPotMoved ? " OK" : " --");
  
  display.setCursor(0, 50);
  display.println("Turn pots to test");
}

void drawButtonTestPage2() {
  display.setTextSize(1);
  display.setCursor(0, 16);
  display.println("ARROWS & OK");
  
  display.setCursor(0, 26);
  display.print("UP:");
  display.print(inputTestResults.arrowUp ? "OK" : "--");
  
  display.setCursor(40, 26);
  display.print("DN:");
  display.print(inputTestResults.arrowDown ? "OK" : "--");
  
  display.setCursor(80, 26);
  display.print("OK:");
  display.print(inputTestResults.okButton ? "OK" : "--");
  
  display.setCursor(0, 36);
  display.print("LF:");
  display.print(inputTestResults.arrowLeft ? "OK" : "--");
  
  display.setCursor(40, 36);
  display.print("RT:");
  display.print(inputTestResults.arrowRight ? "OK" : "--");
  
  display.setCursor(0, 48);
  display.println("Press buttons to test");
}

void drawButtonTestPage3() {
  display.setTextSize(1);
  display.setCursor(0, 16);
  display.println("JOY BTNS & TRIGGERS");
  
  display.setCursor(0, 26);
  display.print("L-Joy:");
  display.print(inputTestResults.leftJoyBtn ? "OK" : "--");
  
  display.setCursor(60, 26);
  display.print("R-Joy:");
  display.print(inputTestResults.rightJoyBtn ? "OK" : "--");
  
  display.setCursor(0, 36);
  display.print("L-TrUp:");
  display.print(inputTestResults.leftTriggerUp ? "OK" : "--");
  
  display.setCursor(0, 46);
  display.print("L-TrDn:");
  display.print(inputTestResults.leftTriggerDown ? "OK" : "--");
  
  display.setCursor(60, 36);
  display.print("R-TrUp:");
  display.print(inputTestResults.rightTriggerUp ? "OK" : "--");
  
  display.setCursor(60, 46);
  display.print("R-TrDn:");
  display.print(inputTestResults.rightTriggerDown ? "OK" : "--");
  
  display.setCursor(0, 56);
  display.println("Test triggers & buttons");
}

void resetButtonTest() {
  buttonTestActive = false;
  buttonTestPage = 0;
  exitCombinationDetected = false;
  lastUpArrow = false;
  lastDownArrow = false;
}

bool isButtonTestActive() {
  return buttonTestActive;
}

#endif