/*
  display_test.h - OLED Display Testing Functions
  RC Transmitter for Teensy 4.0
  
  Tests various display functions including:
  - Pixel test patterns
  - Text rendering
  - Graphics primitives
  - Brightness/contrast testing
  - Border detection
*/

#ifndef DISPLAY_TEST_H
#define DISPLAY_TEST_H

#include "config.h"
#include "display.h"
#include "controls.h"

// Display test variables
bool displayTestActive = false;
int displayTestStep = 0;
unsigned long displayTestStepTime = 0;
unsigned long displayTestStartTime = 0;
bool displayTestCompleted = false;
bool buttonCheckEnabled = false;

// Test sequence definitions
#define DISPLAY_TEST_STEPS 8
#define DISPLAY_TEST_STEP_DURATION 2000  // 2 seconds per step
#define BUTTON_CHECK_DELAY 500  // Wait 500ms before enabling button checking

// Function declarations
void startDisplayTest();
void updateDisplayTest();
void drawDisplayTestScreen();
void resetDisplayTest();
bool isDisplayTestActive();
void drawTestPattern(int pattern);
void drawPixelTest();
void drawLineTest();
void drawShapeTest();
void drawTextTest();
void drawBrightnessTest();
void drawBorderTest();
void drawFinalTest();

void startDisplayTest() {
  Serial.println("Starting OLED display test sequence...");
  displayTestActive = true;
  displayTestCompleted = false;
  displayTestStep = 0;
  displayTestStartTime = millis();
  displayTestStepTime = millis();
  buttonCheckEnabled = false;  // Disable button checking initially
  
  Serial.println("Display test will cycle through 8 test patterns");
  Serial.println("Wait 0.5 seconds, then any button will exit test");
}

void updateDisplayTest() {
  if (!displayTestActive) return;
  
  // Enable button checking after initial delay
  if (!buttonCheckEnabled && millis() - displayTestStartTime >= BUTTON_CHECK_DELAY) {
    buttonCheckEnabled = true;
    Serial.println("Button checking enabled - any button will now exit test");
  }
  
  // Check for any button press to exit early (only after delay)
  if (buttonCheckEnabled) {
    extern ButtonStates buttons;
    if (buttons.btnOK || buttons.btnLeft || buttons.btnRight || 
        buttons.btnUp || buttons.btnDown || buttons.leftJoyBtn || 
        buttons.rightJoyBtn) {
      // Exit display test
      displayTestActive = false;
      displayTestCompleted = false;
      buttonCheckEnabled = false;
      
      // Return to main menu
      extern MenuState currentMenu;
      extern int maxMenuItems;
      extern int menuSelection;
      extern int menuOffset;
      currentMenu = MENU_MAIN;
      maxMenuItems = 8;
      menuSelection = 0;
      menuOffset = 0;
      
      Serial.println("Display test cancelled by user");
      return;
    }
  }
  
  // Auto-advance test steps
  if (millis() - displayTestStepTime >= DISPLAY_TEST_STEP_DURATION) {
    displayTestStep++;
    displayTestStepTime = millis();
    
    Serial.print("Display test step: ");
    Serial.println(displayTestStep + 1);
    
    if (displayTestStep >= DISPLAY_TEST_STEPS) {
      // Test sequence complete
      displayTestCompleted = true;
      displayTestStep = DISPLAY_TEST_STEPS - 1; // Stay on final step
      
      // Auto-exit after showing final step for 3 seconds
      if (millis() - displayTestStepTime >= 3000) {
        displayTestActive = false;
        displayTestCompleted = false;
        buttonCheckEnabled = false;
        
        // Return to main menu
        extern MenuState currentMenu;
        extern int maxMenuItems;
        extern int menuSelection;
        extern int menuOffset;
        currentMenu = MENU_MAIN;
        maxMenuItems = 8;
        menuSelection = 0;
        menuOffset = 0;
        
        Serial.println("Display test sequence completed");
      }
    }
  }
}

void drawDisplayTestScreen() {
  display.clearDisplay();
  
  // Draw the appropriate test pattern
  drawTestPattern(displayTestStep);
  
  display.display();
}

void drawTestPattern(int pattern) {
  switch (pattern) {
    case 0:
      drawPixelTest();
      break;
    case 1:
      drawLineTest();
      break;
    case 2:
      drawShapeTest();
      break;
    case 3:
      drawTextTest();
      break;
    case 4:
      drawBrightnessTest();
      break;
    case 5:
      drawBorderTest();
      break;
    case 6:
      drawFinalTest();
      break;
    case 7:
      // Test complete screen
      display.setTextSize(2);
      display.setCursor(10, 20);
      display.println("TEST");
      display.setCursor(10, 40);
      display.println("COMPLETE");
      break;
    default:
      // Fallback
      display.setTextSize(1);
      display.setCursor(0, 0);
      display.println("Display Test");
      display.setCursor(0, 20);
      display.print("Step: ");
      display.println(pattern + 1);
      break;
  }
  
  // Show step indicator in top-right corner
  display.setTextSize(1);
  display.setCursor(100, 0);
  display.print(pattern + 1);
  display.print("/");
  display.print(DISPLAY_TEST_STEPS);
}

void drawPixelTest() {
  // Test individual pixel control
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("Pixel Test");
  
  // Draw a checkerboard pattern
  for (int x = 0; x < SCREEN_WIDTH; x += 4) {
    for (int y = 16; y < SCREEN_HEIGHT; y += 4) {
      if ((x/4 + y/4) % 2 == 0) {
        display.fillRect(x, y, 2, 2, SSD1306_WHITE);
      }
    }
  }
  
  // Draw some scattered pixels
  for (int i = 0; i < 50; i++) {
    int x = random(0, SCREEN_WIDTH);
    int y = random(16, SCREEN_HEIGHT);
    display.drawPixel(x, y, SSD1306_WHITE);
  }
}

void drawLineTest() {
  // Test line drawing
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("Line Test");
  
  // Horizontal lines
  for (int y = 20; y < 35; y += 3) {
    display.drawLine(0, y, SCREEN_WIDTH-1, y, SSD1306_WHITE);
  }
  
  // Vertical lines  
  for (int x = 10; x < SCREEN_WIDTH; x += 8) {
    display.drawLine(x, 35, x, 50, SSD1306_WHITE);
  }
  
  // Diagonal lines
  display.drawLine(0, 50, SCREEN_WIDTH-1, SCREEN_HEIGHT-1, SSD1306_WHITE);
  display.drawLine(SCREEN_WIDTH-1, 50, 0, SCREEN_HEIGHT-1, SSD1306_WHITE);
  
  // Draw a star pattern from center
  int centerX = SCREEN_WIDTH / 2;
  int centerY = 45;
  for (int angle = 0; angle < 360; angle += 45) {
    int x2 = centerX + 20 * cos(angle * PI / 180);
    int y2 = centerY + 10 * sin(angle * PI / 180);
    display.drawLine(centerX, centerY, x2, y2, SSD1306_WHITE);
  }
}

void drawShapeTest() {
  // Test shape drawing
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("Shape Test");
  
  // Rectangles
  display.drawRect(10, 15, 30, 20, SSD1306_WHITE);
  display.fillRect(45, 15, 20, 20, SSD1306_WHITE);
  
  // Circles
  display.drawCircle(80, 25, 12, SSD1306_WHITE);
  display.fillCircle(110, 25, 8, SSD1306_WHITE);
  
  // Triangles (using lines)
  display.drawLine(10, 40, 25, 60, SSD1306_WHITE);
  display.drawLine(25, 60, 40, 40, SSD1306_WHITE);
  display.drawLine(40, 40, 10, 40, SSD1306_WHITE);
  
  // Filled triangle area
  for (int y = 45; y < 55; y++) {
    int width = (y - 45) * 2;
    display.drawLine(60 - width/2, y, 60 + width/2, y, SSD1306_WHITE);
  }
  
  // Round rectangle simulation
  display.drawRect(85, 42, 25, 16, SSD1306_WHITE);
  display.drawPixel(85, 42, SSD1306_BLACK);
  display.drawPixel(109, 42, SSD1306_BLACK);
  display.drawPixel(85, 57, SSD1306_BLACK);
  display.drawPixel(109, 57, SSD1306_BLACK);
}

void drawTextTest() {
  // Test text rendering at different sizes
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("Text Test");
  
  display.setTextSize(1);
  display.setCursor(0, 12);
  display.println("Size 1: ABCD 1234");
  
  display.setTextSize(2);
  display.setCursor(0, 25);
  display.println("Size 2");
  
  display.setTextSize(1);
  display.setCursor(0, 45);
  display.println("Special: !@#$%^&*()");
  
  display.setCursor(0, 55);
  display.println("Lower: abcdefghijk");
}

void drawBrightnessTest() {
  // Test brightness/contrast patterns
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("Brightness Test");
  
  // Create gradient effect using dithering
  for (int x = 0; x < SCREEN_WIDTH; x++) {
    for (int y = 15; y < 35; y++) {
      // Create a dithered pattern for "gray" levels
      int intensity = x * 4 / SCREEN_WIDTH; // 0-3 intensity levels
      bool pixel = false;
      
      switch (intensity) {
        case 0: pixel = false; break; // Black
        case 1: pixel = ((x + y) % 4 == 0); break; // 25%
        case 2: pixel = ((x + y) % 2 == 0); break; // 50%
        case 3: pixel = ((x + y) % 4 != 3); break; // 75%
        default: pixel = true; break; // White
      }
      
      if (pixel) {
        display.drawPixel(x, y, SSD1306_WHITE);
      }
    }
  }
  
  // Test alternating bands
  for (int y = 40; y < SCREEN_HEIGHT; y += 4) {
    if ((y / 4) % 2 == 0) {
      display.drawLine(0, y, SCREEN_WIDTH-1, y, SSD1306_WHITE);
      display.drawLine(0, y+1, SCREEN_WIDTH-1, y+1, SSD1306_WHITE);
    }
  }
}

void drawBorderTest() {
  // Test border and edge detection
  display.setTextSize(1);
  display.setCursor(20, 28);
  display.println("Border Test");
  
  // Draw border around entire screen
  display.drawRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, SSD1306_WHITE);
  
  // Draw corner markers
  display.fillRect(0, 0, 5, 5, SSD1306_WHITE);
  display.fillRect(SCREEN_WIDTH-5, 0, 5, 5, SSD1306_WHITE);
  display.fillRect(0, SCREEN_HEIGHT-5, 5, 5, SSD1306_WHITE);
  display.fillRect(SCREEN_WIDTH-5, SCREEN_HEIGHT-5, 5, 5, SSD1306_WHITE);
  
  // Draw crosshairs at center
  int centerX = SCREEN_WIDTH / 2;
  int centerY = SCREEN_HEIGHT / 2;
  display.drawLine(centerX-10, centerY, centerX+10, centerY, SSD1306_WHITE);
  display.drawLine(centerX, centerY-10, centerX, centerY+10, SSD1306_WHITE);
  
  // Draw inner border
  display.drawRect(10, 10, SCREEN_WIDTH-20, SCREEN_HEIGHT-20, SSD1306_WHITE);
}

void drawFinalTest() {
  // Comprehensive test combining multiple elements
  display.setTextSize(1);
  display.setCursor(25, 0);
  display.println("Final Test");
  
  // Simulate the normal display layout
  display.setCursor(0, 10);
  display.print("RC TX - ONLINE");
  display.setCursor(0, 20);
  display.print("ARMED PKT:999");
  
  // Draw throttle bar (vertical)
  display.drawRect(0, 30, 8, 32, SSD1306_WHITE);
  display.fillRect(1, 40, 6, 12, SSD1306_WHITE); // Mid-throttle
  display.drawLine(0, 46, 7, 46, SSD1306_WHITE); // Center line
  
  // Draw steering bar (horizontal)  
  display.drawRect(15, 56, 80, 8, SSD1306_WHITE);
  display.fillRect(45, 57, 20, 6, SSD1306_WHITE); // Right steer
  display.drawLine(55, 56, 55, 63, SSD1306_WHITE); // Center line
  
  // Values table simulation
  display.drawRect(15, 30, 70, 24, SSD1306_WHITE);
  display.setCursor(17, 32);
  display.print("THR  500  512");
  display.setCursor(17, 42);
  display.print("STR -200  400");
  
  // Status indicators
  display.fillCircle(100, 35, 3, SSD1306_WHITE); // Armed indicator
  display.drawCircle(110, 35, 3, SSD1306_WHITE);  // Radio indicator
  
  // Menu hint
  display.setCursor(100, 56);
  display.print("Hold OK");
}

void resetDisplayTest() {
  displayTestActive = false;
  displayTestCompleted = false;
  displayTestStep = 0;
  buttonCheckEnabled = false;
}

bool isDisplayTestActive() {
  return displayTestActive;
}

#endif