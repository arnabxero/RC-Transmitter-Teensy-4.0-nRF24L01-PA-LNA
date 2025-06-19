/*
  menu.h - Enhanced Core Menu System with Range Settings, Factory Reset, and Audio Settings
  RC Transmitter for Teensy 4.0 (MPU6500 Removed)
*/

#ifndef MENU_H
#define MENU_H

#include "config.h"
#include "display.h"
#include "controls.h"
#include "menu_data.h"
#include "menu_display.h"
#include "menu_settings.h"
#include "menu_calibration.h"
#include "display_test.h"
#include "test_buttons.h"

// Menu navigation variables - declare extern where used in other files
MenuState currentMenu = MENU_HIDDEN;
MenuState previousMenu = MENU_HIDDEN;
int menuSelection = 0;
int menuOffset = 0;
int maxMenuItems = 0;
int maxVisibleItems = 4;
bool menuActive = false;
unsigned long menuTimer = 0;
bool lastOkButtonState = false;

// Navigation timing
unsigned long lastNavigation = 0;
#define NAV_DEBOUNCE 200

// Cancel confirmation variables
bool cancelConfirmActive = false;
int cancelSelection = 0; // 0 = Cancel, 1 = OK

// Function declarations
void initMenu();
void updateMenu();
void handleMenuNavigation();
void enterMenu();
void exitMenu();
void selectMenuItem();
void goBack();
void showCancelConfirm();
void handleCancelConfirmation();
int getNavigationDirection();
bool isMenuActive();
void drawMenu();

// Forward declaration for the lockout check
extern bool isInSettingLockout();
extern void drawSettingSaveScreen();

void initMenu() {
  Serial.println("Initializing enhanced menu system...");
  
  // Initialize all subsystems
  initMenuData();
  initMenuSettings();
  initMenuCalibration();
  
  Serial.println("Enhanced menu system initialized with Range Settings, Audio Settings, and Factory Reset!");
}

void updateMenu() {
  // Handle factory reset updates first
  if (isFactoryResetActive()) {
    updateFactoryReset();
    return; // Don't process other menu updates during factory reset
  }

  // Handle display test updates
  if (isDisplayTestActive()) {
    updateDisplayTest();
    return;
  }

  // Handle input test updates
  if (isButtonTestActive()) {
    updateButtonTest();
    return;
  }
    
  // Handle cancel confirmation first
  if (cancelConfirmActive) {
    handleCancelConfirmation();
    return;
  }
  
  // Check for right joystick button press (cancel function)
  if (buttons.rightJoyBtn && millis() - lastNavigation > NAV_DEBOUNCE) {
    if (currentMenu != MENU_MAIN && currentMenu != MENU_HIDDEN) {
      showCancelConfirm();
      return;
    }
  }
  
  // Simple OK button press handling
  bool currentOkState = buttons.btnOK;
  
  // Rising edge detection for OK button - but only handle it if we're not in special modes
  if (currentOkState && !lastOkButtonState) {
    // Add debounce protection
    if (millis() - lastNavigation > NAV_DEBOUNCE) {
      if (currentMenu == MENU_HIDDEN) {
        // Simple press from homepage - enter menu immediately
        enterMenu();
        Serial.println("OK pressed from homepage - entering menu");
        lastNavigation = millis();
      } else if (currentMenu == MENU_RADIO_TEST) {
        // OK button pressed during radio test - go back to main menu
        goBack();
        lastNavigation = millis();
      } else if (!isSettingActive() && !isCalibrationActive()) {
        // Only handle menu selection if we're not in setting or calibration mode
        if (!isInSettingLockout()) {
          selectMenuItem();
          Serial.println("OK pressed in menu - selecting item");
        } else {
          Serial.println("Menu selection blocked - setting lockout active");
        }
        lastNavigation = millis();
      }
    }
  }
  
  lastOkButtonState = currentOkState;
  
  // Handle different subsystem updates
  if (currentMenu != MENU_HIDDEN) {
    // Update appropriate subsystem
    if (isCalibrationActive()) {
      updateMenuCalibration();
    } else if (isSettingActive()) {
      updateMenuSettings();
    } else {
      // Only handle navigation if not in setting lockout
      if (!isInSettingLockout()) {
        handleMenuNavigation();
      }
    }
    
    // Auto-exit menu after 30 seconds of inactivity
    if (millis() - menuTimer > 30000) {
      exitMenu();
    }
  }
}

void handleMenuNavigation() {
  if (millis() - lastNavigation < NAV_DEBOUNCE) return;
  
  int navDirection = getNavigationDirection();
  if (navDirection != 0) {
    menuTimer = millis();
    
    if (navDirection == 1) { // Down
      extern void playNavigationDownSound();
      playNavigationDownSound();
      menuSelection++;
      if (menuSelection >= maxMenuItems) {
        menuSelection = 0;
        menuOffset = 0;
      } else if (menuSelection >= menuOffset + maxVisibleItems) {
        menuOffset++;
      }
    } else if (navDirection == -1) { // Up
      extern void playNavigationUpSound();
      playNavigationUpSound();
      menuSelection--;
      if (menuSelection < 0) {
        menuSelection = maxMenuItems - 1;
        menuOffset = max(0, maxMenuItems - maxVisibleItems);
      } else if (menuSelection < menuOffset) {
        menuOffset--;
      }
    } else if (navDirection == 2) { // Right/Select
      if (!isInSettingLockout()) {
        extern void playSelectSound();
        playSelectSound();
        selectMenuItem();
      }
    } else if (navDirection == -2) { // Left/Back
      extern void playBackSound();
      playBackSound();
      goBack();
    }
    
    lastNavigation = millis();
  }
}

void handleCancelConfirmation() {
  if (millis() - lastNavigation < NAV_DEBOUNCE) return;
  
  int navDirection = getNavigationDirection();
  if (navDirection != 0) {
    if (navDirection == 2 || navDirection == -2) { // Left or Right
      cancelSelection = 1 - cancelSelection; // Toggle between 0 and 1
    }
    lastNavigation = millis();
  }
  
  // Check for OK to confirm selection
  if (buttons.btnOK && millis() - lastNavigation > NAV_DEBOUNCE) {
    if (cancelSelection == 1) { // OK selected - cancel operation
      exitMenu();
    }
    cancelConfirmActive = false;
    lastNavigation = millis();
  }
}

int getNavigationDirection() {
  // Always allow arrow button navigation
  if (buttons.btnDown) return 1;
  if (buttons.btnUp) return -1;
  if (buttons.btnRight) return 2;
  if (buttons.btnLeft) return -2;
  
  // Allow joystick navigation only when not in special modes
  if (!isSettingActive() && !isCalibrationActive()) {
    int rightJoyY = analogRead(RIGHT_JOY_Y);
    int leftJoyY = analogRead(LEFT_JOY_Y);
    int rightJoyX = analogRead(RIGHT_JOY_X);
    int leftJoyX = analogRead(LEFT_JOY_X);
    
    if (rightJoyY < 200 || leftJoyY > 800) return -1; // Up
    if (rightJoyY > 800 || leftJoyY < 200) return 1;  // Down
    if (rightJoyX < 200 || leftJoyX > 800) return -2; // Left
    if (rightJoyX > 800 || leftJoyX < 200) return 2;  // Right
  }
  
  return 0;
}

void enterMenu() {
  Serial.println("Entering enhanced menu...");
  extern void playMenuEnterSound();
  playMenuEnterSound();
  currentMenu = MENU_MAIN;
  maxMenuItems = 10;  // Updated from 9 to 10 to include Audio Settings
  menuSelection = 0;
  menuOffset = 0;
  menuTimer = millis();
  menuActive = true;
  applyLEDSettings();
}

void exitMenu() {
  Serial.println("Exiting menu...");
  extern void playMenuExitSound();
  playMenuExitSound();
  currentMenu = MENU_HIDDEN;
  menuActive = false;
  exitMenuCalibration();
  exitMenuSettings();
  cancelConfirmActive = false;
  menuSelection = 0;
  menuOffset = 0;
  applyLEDSettings(); // Return to normal LED state
}

void showCancelConfirm() {
  cancelConfirmActive = true;
  cancelSelection = 0; // Default to "Cancel"
}

void goBack() {
  switch (currentMenu) {
    case MENU_MAIN:
      exitMenu();
      break;
    case MENU_CALIBRATION:
      currentMenu = MENU_MAIN;
      maxMenuItems = 10;  // Updated to 10
      break;
    case MENU_SETTINGS:
      currentMenu = MENU_MAIN;
      maxMenuItems = 10;  // Updated to 10
      break;
    case MENU_RANGE_SETTINGS:
      currentMenu = MENU_MAIN;
      maxMenuItems = 10;  // Updated to 10
      break;
    case MENU_AUDIO_SETTINGS:  // NEW: Audio settings back navigation
      currentMenu = MENU_MAIN;
      maxMenuItems = 10;  // Updated to 10
      break;
    case MENU_DISPLAY_TEST:
      // Reset display test state and go back to main menu
      resetDisplayTest();
      currentMenu = MENU_MAIN;
      maxMenuItems = 10;  // Updated to 10
      break;
    case MENU_BUTTON_TEST:
      // Reset input test state and go back to main menu
      resetButtonTest();
      currentMenu = MENU_MAIN;
      maxMenuItems = 10;  // Updated to 10
      break;
    case MENU_INFO:
      currentMenu = MENU_MAIN;
      maxMenuItems = 10;  // Updated to 10
      break;
    case MENU_RADIO_TEST:
      // Reset radio test state and go back to main menu
      extern void resetRadioTest();
      resetRadioTest();
      currentMenu = MENU_MAIN;
      maxMenuItems = 10;  // Updated to 10
      break;
    case MENU_FACTORY_RESET_CONFIRM:
      currentMenu = MENU_MAIN;
      maxMenuItems = 10;  // Updated to 10
      break;
    case MENU_FACTORY_RESET_FINAL:
      currentMenu = MENU_FACTORY_RESET_CONFIRM;
      maxMenuItems = 2;
      break;
    case MENU_JOYSTICK_CAL:
    case MENU_POTENTIOMETER_CAL:
      currentMenu = MENU_CALIBRATION;
      maxMenuItems = 3; // Reduced from 4 after removing MPU6500
      break;
    case MENU_LED_SETTINGS:
    case MENU_FAILSAFE_SETTINGS:
      currentMenu = MENU_SETTINGS;
      maxMenuItems = 8;
      break;
    default:
      // Let subsystems handle their own back navigation
      if (isSettingActive()) {
        goBackSettings();
      } else if (isCalibrationActive()) {
        goBackCalibration();
      } else {
        currentMenu = MENU_MAIN;
        maxMenuItems = 10;  // Updated to 10
      }
      break;
  }
  menuSelection = 0;
  menuOffset = 0;
}

void selectMenuItem() {
  // Final safety check before any menu action
  if (isInSettingLockout()) {
    Serial.println("selectMenuItem() blocked - setting lockout active");
    return;
  }
  
  switch (currentMenu) {
    case MENU_MAIN:
      switch (menuSelection) {
        case 0: // Calibration
          currentMenu = MENU_CALIBRATION;
          maxMenuItems = 3; // Reduced from 4 after removing MPU6500
          break;
        case 1: // Settings
          currentMenu = MENU_SETTINGS;
          maxMenuItems = 8;
          break;
        case 2: // Range Settings
          currentMenu = MENU_RANGE_SETTINGS;
          maxMenuItems = 7;
          break;
        case 3: // NEW: Audio Settings
          currentMenu = MENU_AUDIO_SETTINGS;
          maxMenuItems = 9;
          break;
        case 4: // System Info (moved from 3)
          currentMenu = MENU_INFO;
          maxMenuItems = 5;  // Updated to 5 to include audio system info
          break;
        case 5: // Radio Test (moved from 4)
          // Start radio test and show results
          extern void startRadioTest();
          startRadioTest();
          currentMenu = MENU_RADIO_TEST;
          break;
        case 6: // Display Test (moved from 5)
          startDisplayTest();
          currentMenu = MENU_DISPLAY_TEST;
          break;
        case 7: // Input Test (moved from 6)
          startButtonTest();
          currentMenu = MENU_BUTTON_TEST;
          break;
        case 8: // Factory Reset (moved from 7)
          currentMenu = MENU_FACTORY_RESET_CONFIRM;
          maxMenuItems = 2;
          break;
        case 9: // Exit (moved from 8)
          exitMenu();
          return;
      }
      break;
      
    case MENU_CALIBRATION:
      switch (menuSelection) {
        case 0: 
          currentMenu = MENU_JOYSTICK_CAL; 
          maxMenuItems = 5; 
          break;
        case 1: 
          currentMenu = MENU_POTENTIOMETER_CAL; 
          maxMenuItems = 3; 
          break;
        case 2: // Back (moved from index 3)
          goBack(); 
          return;
      }
      break;
      
    case MENU_JOYSTICK_CAL:
      switch (menuSelection) {
        case 0: startCalibration("JOYSTICK", "RIGHT_X"); return;
        case 1: startCalibration("JOYSTICK", "RIGHT_Y"); return;
        case 2: startCalibration("JOYSTICK", "LEFT_X"); return;
        case 3: startCalibration("JOYSTICK", "LEFT_Y"); return;
        case 4: goBack(); return;
      }
      break;
      
    case MENU_POTENTIOMETER_CAL:
      switch (menuSelection) {
        case 0: startCalibration("POTENTIOMETER", "LEFT"); return;
        case 1: startCalibration("POTENTIOMETER", "RIGHT"); return;
        case 2: goBack(); return;
      }
      break;
      
    case MENU_SETTINGS:
      switch (menuSelection) {
        case 0: startSetting("DEADZONE"); return;
        case 1: startSetting("BRIGHTNESS"); return;
        case 2: 
          currentMenu = MENU_LED_SETTINGS; 
          maxMenuItems = 7; 
          break;
        case 3: startSetting("RADIO_ADDRESS"); return;
        case 4: startSetting("CHANNEL"); return;
        case 5: 
          currentMenu = MENU_FAILSAFE_SETTINGS; 
          maxMenuItems = 4;
          break;
        case 6: resetAllSettings(); break;
        case 7: goBack(); return;
      }
      break;
      
    // Range Settings Menu
    case MENU_RANGE_SETTINGS:
      handleRangeSettingsSelection(menuSelection);
      if (menuSelection == 6) goBack(); // Back option
      return;
      
    // NEW: Audio Settings Menu
    case MENU_AUDIO_SETTINGS:
      handleAudioSettingsSelection(menuSelection);
      if (menuSelection == 8) goBack(); // Back option
      return;
      
    case MENU_LED_SETTINGS:
      handleLEDSettingsSelection(menuSelection);
      if (menuSelection == 6) goBack(); // Back option
      return;
      
    case MENU_FAILSAFE_SETTINGS:
      handleFailsafeSettingsSelection(menuSelection);
      if (menuSelection == 3) goBack(); // Back option
      return;
      
    case MENU_INFO:
      if (menuSelection == maxMenuItems - 1) {
        goBack();
        return;
      }
      break;
      
    // Factory Reset Menu Handling
    case MENU_FACTORY_RESET_CONFIRM:
      if (menuSelection == 0) { // No selected
        goBack(); // Return to main menu
      } else { // Yes selected
        currentMenu = MENU_FACTORY_RESET_FINAL;
        maxMenuItems = 2; // No/Yes options
        menuSelection = 0; // Default to No on final confirmation
      }
      return;
      
    case MENU_FACTORY_RESET_FINAL:
      if (menuSelection == 0) { // No selected
        goBack(); // Return to previous confirmation
      } else { // YES selected - start factory reset
        currentMenu = MENU_FACTORY_RESET_PROGRESS;
        startFactoryReset();
      }
      return;
  }
  
  menuSelection = 0;
  menuOffset = 0;
  menuTimer = millis();
}

bool isMenuActive() {
  return menuActive;
}

// Main display function - delegates to appropriate subsystem
void drawMenu() {
  if (currentMenu == MENU_HIDDEN) return;
  
  display.clearDisplay();
  
  // Check for factory reset first
  if (isFactoryResetActive()) {
    drawFactoryResetScreen();
  } else if (isDisplayTestActive()) {
    drawDisplayTestScreen();
  } else if (isInSettingLockout()) {
    drawSettingSaveScreen();
  } else if (cancelConfirmActive) {
    drawCancelConfirmation();
  } else if (isCalibrationActive()) {
    drawMenuCalibration();
  } else if (isSettingActive()) {
    drawMenuSettings();
  } else if (currentMenu == MENU_RADIO_TEST) {
    // Draw radio test screen
    extern void drawRadioTestScreen();
    drawRadioTestScreen();
  } else if (isButtonTestActive()) {
    drawButtonTestScreen();
  } else {
    drawMainMenus();
  }
  
  display.display();
}

#endif