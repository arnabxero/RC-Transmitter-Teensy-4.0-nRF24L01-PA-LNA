/*
  radio_test.h - NRF24 Radio Testing and Diagnostics
  RC Transmitter for Teensy 4.0
  
  Tests if the nRF24L01 module is working by comparing register values
  against standard reference values
*/

#ifndef RADIO_TEST_H
#define RADIO_TEST_H

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include "config.h"
#include "display.h"

// Radio test variables
bool radioTestActive = false;
unsigned long radioTestStartTime = 0;
bool radioTestCompleted = false;

// Register values structure - using different names to avoid NRF24 macro conflicts
struct RadioRegisters {
  uint8_t reg_EN_AA;      // Expected: 0x3f
  uint8_t reg_EN_RXADDR;  // Expected: 0x02  
  uint8_t reg_RF_CH;      // Expected: 0x4c
  uint8_t reg_RF_SETUP;   // Expected: 0x03
  uint8_t reg_CONFIG;     // Expected: 0x0f
};

RadioRegisters testResults;
RadioRegisters expectedValues = {0x3f, 0x02, 0x4c, 0x03, 0x0f};

// Function declarations
void startRadioTest();
void runRadioTest();
void drawRadioTestScreen();
bool isRadioTestCompleted();
void resetRadioTest();
uint8_t readRegister(uint8_t reg);

// Read a register directly from the nRF24L01 using SPI
uint8_t readRegister(uint8_t reg) {
  uint8_t result;
  
  digitalWrite(RADIO_CSN, LOW);
  SPI.transfer(0x00 | (reg & 0x1F)); // Read command
  result = SPI.transfer(0xFF);       // Read the register value
  digitalWrite(RADIO_CSN, HIGH);
  
  return result;
}

void startRadioTest() {
  Serial.println("Starting nRF24L01 radio test...");
  radioTestActive = true;
  radioTestCompleted = false;
  radioTestStartTime = millis();
  
  // Reset test results
  testResults.reg_EN_AA = 0x00;
  testResults.reg_EN_RXADDR = 0x00;
  testResults.reg_RF_CH = 0x00;
  testResults.reg_RF_SETUP = 0x00;
  testResults.reg_CONFIG = 0x00;
  
  // Run the test
  runRadioTest();
}

void runRadioTest() {
  // Create new radio instance for testing
  RF24 testRadio(RADIO_CE, RADIO_CSN);
  
  // Initialize the radio
  if (!testRadio.begin()) {
    Serial.println("Radio hardware not responding!");
    radioTestCompleted = true;
    return;
  }
  
  // Configure radio with standard test settings
  testRadio.setPALevel(RF24_PA_LOW);
  
  // Simple configuration to match the reference test
  byte addresses[][6] = {"1Node", "2Node"};
  testRadio.openWritingPipe(addresses[0]);
  testRadio.openReadingPipe(1, addresses[1]); 
  testRadio.startListening();
  
  // Read the specific registers
  testResults.reg_EN_AA = readRegister(0x01);      // EN_AA register
  testResults.reg_EN_RXADDR = readRegister(0x02);  // EN_RXADDR register  
  testResults.reg_RF_CH = readRegister(0x05);      // RF_CH register
  testResults.reg_RF_SETUP = readRegister(0x06);   // RF_SETUP register
  testResults.reg_CONFIG = readRegister(0x00);     // CONFIG register
  
  // Print results to serial for debugging
  Serial.println("NRF24L01 Register Test Results:");
  
  Serial.print("EN_AA (Expected 0x3f): 0x"); 
  if (testResults.reg_EN_AA < 0x10) Serial.print("0");
  Serial.println(testResults.reg_EN_AA, HEX);
  
  Serial.print("EN_RXADDR (Expected 0x02): 0x"); 
  if (testResults.reg_EN_RXADDR < 0x10) Serial.print("0");
  Serial.println(testResults.reg_EN_RXADDR, HEX);
  
  Serial.print("RF_CH (Expected 0x4c): 0x"); 
  if (testResults.reg_RF_CH < 0x10) Serial.print("0");
  Serial.println(testResults.reg_RF_CH, HEX);
  
  Serial.print("RF_SETUP (Expected 0x03): 0x"); 
  if (testResults.reg_RF_SETUP < 0x10) Serial.print("0");
  Serial.println(testResults.reg_RF_SETUP, HEX);
  
  Serial.print("CONFIG (Expected 0x0f): 0x"); 
  if (testResults.reg_CONFIG < 0x10) Serial.print("0");
  Serial.println(testResults.reg_CONFIG, HEX);
  
  radioTestCompleted = true;
}

void drawRadioTestScreen() {
  display.clearDisplay();
  display.setTextSize(1);
  
  // Header
  display.setCursor(0, 0);
  display.println("NRF24 Radio Test");
  
  // Table header
  display.setCursor(0, 8);
  display.print("#  Ideal Yours");
  
  // Define register names and their data
  String regNames[] = {"AA", "AD", "CH", "ST", "CF"};
  uint8_t expectedVals[] = {expectedValues.reg_EN_AA, expectedValues.reg_EN_RXADDR, 
                           expectedValues.reg_RF_CH, expectedValues.reg_RF_SETUP, expectedValues.reg_CONFIG};
  uint8_t actualVals[] = {testResults.reg_EN_AA, testResults.reg_EN_RXADDR,
                         testResults.reg_RF_CH, testResults.reg_RF_SETUP, testResults.reg_CONFIG};
  
  int startY = 16;
  int rowHeight = 9;
  
  for (int i = 0; i < 5; i++) {
    int yPos = startY + (i * rowHeight);
    
    // Register name (2 chars)
    display.setCursor(0, yPos);
    display.print(regNames[i]);
    
    // Expected value (0xXX format)
    display.setCursor(18, yPos);
    display.print("0x");
    if (expectedVals[i] < 0x10) display.print("0");
    display.print(expectedVals[i], HEX);
    
    // Actual value (0xXX format) 
    display.setCursor(54, yPos);
    if (radioTestCompleted) {
      display.print("0x");
      if (actualVals[i] < 0x10) display.print("0");
      display.print(actualVals[i], HEX);
      
      // Status indicator
      display.setCursor(90, yPos);
      if (actualVals[i] == expectedVals[i]) {
        display.print("OK");
      } else {
        display.print("FAIL");
      }
    } else {
      display.print("----");
    }
  }
  
  // Instructions at bottom
  display.setCursor(0, 56);
  if (!radioTestCompleted) {
    display.print("Testing...");
  } else {
    // Flash "Press any key to exit" message
    if ((millis() / 500) % 2 == 0) {
      display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Inverted text
    } else {
      display.setTextColor(SSD1306_WHITE); // Normal text
    }
    display.print("Press any key to exit");
    display.setTextColor(SSD1306_WHITE); // Reset text color
  }
  
  display.display();
}

bool isRadioTestCompleted() {
  return radioTestCompleted;
}

void resetRadioTest() {
  radioTestActive = false;
  radioTestCompleted = false;
  
  // Reset test results
  testResults.reg_EN_AA = 0x00;
  testResults.reg_EN_RXADDR = 0x00;
  testResults.reg_RF_CH = 0x00;
  testResults.reg_RF_SETUP = 0x00;
  testResults.reg_CONFIG = 0x00;
}

#endif