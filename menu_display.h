/*
  menu_display.h - Enhanced Display Functions with Range Settings and Factory Reset
  RC Transmitter for Teensy 4.0 (MPU6500 Removed)
*/

#ifndef MENU_DISPLAY_H
#define MENU_DISPLAY_H

#include "config.h"
#include "display.h"
#include "menu_data.h"

// Display constants
#define MENU_ITEM_HEIGHT 12
#define MENU_START_Y 17
#define SCROLLBAR_WIDTH 4
#define SCROLLBAR_X 124

// External variables from menu.h
extern int menuSelection;
extern int menuOffset;
extern int maxMenuItems;
extern int maxVisibleItems;
extern MenuState currentMenu;
extern bool cancelConfirmActive;
extern int cancelSelection;

// Function declarations
void drawMainMenus();
void drawScrollableMenu(MenuItem* items, int itemCount, String header);
void drawScrollbar(int totalItems, int visibleItems, int offset);
void drawCancelConfirmation();

void drawMainMenus() {
  switch (currentMenu) {
    case MENU_MAIN: {
      MenuItem items[] = {
        {"Calibration", true, true},
        {"Settings", true, true},
        {"Range Settings", true, true},
        {"System Info", true, true},
        {"Radio Test", true, false},
        {"Display Test", true, false},
        {"Factory Reset", true, true},  // Changed to hasSubmenu = true
        {"Exit", true, false}
      };
      drawScrollableMenu(items, 8, "RC TX MENU");
      break;
    }
    
    case MENU_CALIBRATION: {
      MenuItem items[] = {
        {"Joystick Cal", true, true},
        {"Potentiometer Cal", true, true},
        {"Back", true, false}
      };
      drawScrollableMenu(items, 3, "Calibration"); // Reduced from 4 after removing MPU6500
      break;
    }
    
    case MENU_JOYSTICK_CAL: {
      MenuItem items[] = {
        {"Right X " + getCalibrationStatus("RIGHT_X"), true, false},
        {"Right Y " + getCalibrationStatus("RIGHT_Y"), true, false},
        {"Left X " + getCalibrationStatus("LEFT_X"), true, false},
        {"Left Y " + getCalibrationStatus("LEFT_Y"), true, false},
        {"Back", true, false}
      };
      drawScrollableMenu(items, 5, "Joystick Cal");
      break;
    }
    
    case MENU_POTENTIOMETER_CAL: {
      MenuItem items[] = {
        {"Left Pot " + getCalibrationStatus("LEFT_POT"), true, false},
        {"Right Pot " + getCalibrationStatus("RIGHT_POT"), true, false},
        {"Back", true, false}
      };
      drawScrollableMenu(items, 3, "Potentiometer Cal");
      break;
    }
    
    case MENU_SETTINGS: {
      MenuItem items[] = {
        {"Joystick Deadzone", true, false},
        {"Display Brightness", true, false},
        {"LED Settings", true, true},
        {"Radio Address", true, false},
        {"Radio Channel", true, false},
        {"Failsafe Settings", true, true},
        {"Reset to Defaults", true, false},
        {"Back", true, false}
      };
      drawScrollableMenu(items, 8, "Settings");
      break;
    }
    
    // Range Settings Menu
    case MENU_RANGE_SETTINGS: {
      MenuItem items[] = {
        {"Throttle Min: " + String(settings.throttleMinPWM) + "us", true, false},
        {"Throttle Max: " + String(settings.throttleMaxPWM) + "us", true, false},
        {"Steer Min: " + String(settings.steerMinDegree) + "°", true, false},
        {"Steer Neutral: " + String(settings.steerNeutralDegree) + "°", true, false},
        {"Steer Max: " + String(settings.steerMaxDegree) + "°", true, false},
        {"Reset to Defaults", true, false},
        {"Back", true, false}
      };
      drawScrollableMenu(items, 7, "Range Settings");
      break;
    }
    
    case MENU_LED_SETTINGS: {
      MenuItem items[] = {
        {"LED Enable: " + String(settings.ledEnabled ? "ON" : "OFF"), true, false},
        {"Armed Color", true, false},
        {"Disarmed Color", true, false},
        {"Transmit Color", true, false},
        {"Error Color", true, false},
        {"Menu Color", true, false},
        {"Back", true, false}
      };
      drawScrollableMenu(items, 7, "LED Settings");
      break;
    }
    
    case MENU_FAILSAFE_SETTINGS: {
      MenuItem items[] = {
        {"Enable: " + String(settings.failsafeEnabled ? "ON" : "OFF"), true, false},
        {"Set Throttle: " + String(settings.failsafeThrottle), true, false},
        {"Set Steering: " + String(settings.failsafeSteering), true, false},
        {"Back", true, false}
      };
      drawScrollableMenu(items, 4, "Failsafe");
      break;
    }
    
    case MENU_INFO: {
      MenuItem items[] = {
        {"Firmware v3.1", false, false},
        {"Free Memory: " + String(freeMemory()), false, false},
        {"Range Config: Active", false, false},
        {"Back", true, false}
      };
      drawScrollableMenu(items, 4, "System Info");
      break;
    }
    
    // Factory Reset Confirmation Screens
    case MENU_FACTORY_RESET_CONFIRM: {
      // Draw white background confirmation dialog
      display.fillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, SSD1306_WHITE);
      display.setTextColor(SSD1306_BLACK);
      display.setTextSize(1);
      
      // Title
      display.setCursor(15, 5);
      display.println("Factory Reset");
      
      // Warning message
      display.setCursor(5, 20);
      display.println("Are you sure you want");
      display.setCursor(5, 30);
      display.println("to reset all settings");
      display.setCursor(5, 40);
      display.println("to factory defaults?");
      
      // Selection options
      display.setCursor(20, 55);
      if (menuSelection == 0) {
        display.print("[No]");
      } else {
        display.print("No");
      }
      
      display.print("    ");
      
      if (menuSelection == 1) {
        display.print("[Yes]");
      } else {
        display.print("Yes");
      }
      
      display.setTextColor(SSD1306_WHITE);
      break;
    }
    
    case MENU_FACTORY_RESET_FINAL: {
      // Draw white background final confirmation
      display.fillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, SSD1306_WHITE);
      display.setTextColor(SSD1306_BLACK);
      display.setTextSize(1);
      
      // Title with emphasis
      display.setCursor(20, 5);
      display.println("FINAL WARNING");
      
      // Strong warning message
      display.setCursor(8, 20);
      display.println("ARE YOU ABSOLUTELY");
      display.setCursor(35, 30);
      display.println("SURE?");
      
      // Selection options at bottom
      display.setCursor(20, 57);
      if (menuSelection == 0) {
        display.print("[No]");
      } else {
        display.print("No");
      }
      
      display.print("    ");
      
      if (menuSelection == 1) {
        display.print("[YES]");
      } else {
        display.print("YES");
      }
      
      display.setTextColor(SSD1306_WHITE);
      break;
    }
    
    case MENU_FACTORY_RESET_PROGRESS: {
      drawFactoryResetScreen();
      break;
    }
  }
}

void drawScrollableMenu(MenuItem* items, int itemCount, String header) {
  // Draw header
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println(header);
  
  // Draw menu items
  int visibleItems = min(maxVisibleItems, itemCount);
  
  for (int i = 0; i < visibleItems; i++) {
    int itemIndex = menuOffset + i;
    if (itemIndex >= itemCount) break;
    
    int yPos = MENU_START_Y + (i * MENU_ITEM_HEIGHT);
    
    // Draw selection box
    if (itemIndex == menuSelection) {
      display.fillRect(0, yPos, 120, MENU_ITEM_HEIGHT, SSD1306_WHITE);
      display.setTextColor(SSD1306_BLACK);
    } else {
      display.setTextColor(SSD1306_WHITE);
    }
    
    // Draw menu item text
    display.setCursor(2, yPos + 2);
    display.print(items[itemIndex].title);
    
    // Draw submenu indicator
    if (items[itemIndex].hasSubmenu) {
      display.setCursor(110, yPos + 2);
      display.print(">");
    }
    
    // Reset text color
    display.setTextColor(SSD1306_WHITE);
  }
  
  // Draw scrollbar if needed
  if (itemCount > maxVisibleItems) {
    drawScrollbar(itemCount, visibleItems, menuOffset);
  }
}

void drawScrollbar(int totalItems, int visibleItems, int offset) {
  // Calculate scrollbar dimensions
  int scrollbarHeight = (visibleItems * MENU_ITEM_HEIGHT * maxVisibleItems) / totalItems;
  int scrollbarY = MENU_START_Y + (offset * MENU_ITEM_HEIGHT * maxVisibleItems) / totalItems;
  
  // Draw scrollbar track
  display.drawRect(SCROLLBAR_X, MENU_START_Y, SCROLLBAR_WIDTH, maxVisibleItems * MENU_ITEM_HEIGHT, SSD1306_WHITE);
  
  // Draw scrollbar thumb
  display.fillRect(SCROLLBAR_X + 1, scrollbarY, SCROLLBAR_WIDTH - 2, scrollbarHeight, SSD1306_WHITE);
}

void drawCancelConfirmation() {
  // Draw background box
  display.fillRect(20, 20, 88, 24, SSD1306_WHITE);
  display.drawRect(20, 20, 88, 24, SSD1306_BLACK);
  
  // Draw text
  display.setTextColor(SSD1306_BLACK);
  display.setCursor(25, 25);
  display.println("Cancel Operation?");
  
  // Draw buttons
  display.setCursor(25, 35);
  if (cancelSelection == 0) {
    display.print("[Cancel]");
  } else {
    display.print("Cancel");
  }
  
  display.print("  ");
  
  if (cancelSelection == 1) {
    display.print("[OK]");
  } else {
    display.print("OK");
  }
  
  display.setTextColor(SSD1306_WHITE);
}

#endif