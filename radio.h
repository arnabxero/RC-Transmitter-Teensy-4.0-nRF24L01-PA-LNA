/*
  Enhanced radio.h - NRF24 communication with ACK system
  RC Transmitter for Teensy 4.0
*/

#ifndef RADIO_H
#define RADIO_H

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include "config.h"
#include "controls.h"

// Radio object
extern RF24 radio;
extern bool radioOK;

// ACK tracking variables
extern uint32_t totalPacketsSent;
extern uint32_t acksReceived;
extern uint32_t failedAcks;

// Function declarations
void initRadio();
void transmitData();
bool isRadioOK();
uint32_t getTotalPacketsSent();
uint32_t getAcksReceived();
uint32_t getFailedAcks();
float getAckSuccessRate();

// Radio implementation
RF24 radio(RADIO_CE, RADIO_CSN);
bool radioOK = false;

// ACK tracking variables
uint32_t totalPacketsSent = 0;
uint32_t acksReceived = 0;
uint32_t failedAcks = 0;

void initRadio() {
  Serial.print("Initializing radio with ACK system... ");
  
  radioOK = radio.begin();
  if (radioOK) {
    radio.setDataRate(RF24_250KBPS);
    radio.setPALevel(RF24_PA_MAX);
    radio.setChannel(RADIO_CHANNEL);
    
    // ENABLE AUTO-ACK for reliability tracking
    radio.setAutoAck(true);  // CHANGED: Enable acknowledgements
    radio.setRetries(3, 5);  // CHANGED: Reduce retries for faster response
    radio.setCRCLength(RF24_CRC_16);
    
    radio.openWritingPipe((byte*)RADIO_ADDRESS);
    radio.stopListening(); // Transmitter mode
    
    Serial.println("SUCCESS!");
    Serial.println("ACK system enabled for reliability tracking");
    
    // Reset ACK counters
    totalPacketsSent = 0;
    acksReceived = 0;
    failedAcks = 0;
    
    extern void applyLEDSettings();
    applyLEDSettings();
  } else {
    Serial.println("FAILED!");
    setLED(true, false, false); // Red for error
    Serial.println("LED set to red due to radio failure");
  }
}

void transmitData() {
  data.counter++;
  totalPacketsSent++;
  
  bool result = radio.write(&data, sizeof(data));
  
  // Track ACK results
  if (result) {
    acksReceived++;
  } else {
    failedAcks++;
  }
  
  // Debug output every DEBUG_INTERVAL packets
  if (data.counter % DEBUG_INTERVAL == 0) {
    Serial.print("TX - T:");
    Serial.print(data.throttle);
    Serial.print(" S:");
    Serial.print(data.steering);
    Serial.print(" #");
    Serial.print(data.counter);
    Serial.print(" ACK:");
    Serial.print(result ? "OK" : "FAIL");
    Serial.print(" Success:");
    Serial.print(getAckSuccessRate(), 1);
    Serial.println("%");
    
    if (!result) {
      Serial.println("Warning: ACK not received - check receiver");
    }
  }
}

bool isRadioOK() {
  return radioOK;
}

uint32_t getTotalPacketsSent() {
  return totalPacketsSent;
}

uint32_t getAcksReceived() {
  return acksReceived;
}

uint32_t getFailedAcks() {
  return failedAcks;
}

float getAckSuccessRate() {
  if (totalPacketsSent == 0) return 0.0;
  return ((float)acksReceived / (float)totalPacketsSent) * 100.0;
}

#endif