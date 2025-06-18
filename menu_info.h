/*
  menu_info.h - System Information Page for Teensy 4.0
*/

#ifndef MENU_INFO_H
#define MENU_INFO_H

#include "config.h"
#include "display.h"

// Function to draw the system information page
void drawSystemInfo() {
  display.clearDisplay();
  
  // Header
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("System Information");
  
  // Version info
  display.setCursor(0, 16);
  display.print("Firmware: ");
  display.println("v3.1 Teensy");
  
  // Memory info
  display.setCursor(0, 26);
  display.print("Free RAM: ");
  extern int freeMemory();
  display.print(freeMemory() / 1024);
  display.println("KB");
  
  // Board info
  display.setCursor(0, 36);
  display.print("MCU: Teensy 4.0");
  
  // Radio info
  display.setCursor(0, 46);
  display.print("Radio: ");
  extern bool isRadioOK();
  display.println(isRadioOK() ? "Connected" : "Disconnected");
  
  // Range settings status
  display.setCursor(0, 56);
  display.print("Range Config: Active");
  
  display.display();
}

#endif