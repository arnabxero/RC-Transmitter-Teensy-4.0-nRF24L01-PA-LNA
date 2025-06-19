/*
  audio.h - Professional Audio System for RC Transmitter
  Teensy 4.0 with Piezo Speaker
  
  Features:
  - Professional UI interaction sounds
  - Non-blocking audio with sequence support
  - Musical boot sequence
  - Context-aware audio feedback
  - EEPROM settings integration
  - Volume control and mute functionality
*/

#ifndef AUDIO_H
#define AUDIO_H

#include "config.h"

// Audio pin definition
#define SPEAKER_PIN 23

// Audio settings structure (will be integrated with existing settings)
struct AudioSettings {
  bool enabled = true;
  int volume = 75;  // 0-100%
  bool systemSounds = true;
  bool navigationSounds = true;
  bool alertSounds = true;
  bool musicEnabled = true;
};

extern AudioSettings audioSettings;

// Audio sequence structure
struct AudioNote {
  int frequency;  // Hz (0 = silence)
  int duration;   // milliseconds
};

// Audio state management
struct AudioState {
  bool isPlaying = false;
  bool sequencePlaying = false;
  const AudioNote* currentSequence = nullptr;
  int sequenceLength = 0;
  int sequenceIndex = 0;
  unsigned long noteStartTime = 0;
  unsigned long lastUpdateTime = 0;
  bool muted = false;
};

AudioState audioState;
AudioSettings audioSettings;

// Musical note frequencies (4th octave)
#define NOTE_C4   262
#define NOTE_CS4  277
#define NOTE_D4   294
#define NOTE_DS4  311
#define NOTE_E4   330
#define NOTE_F4   349
#define NOTE_FS4  370
#define NOTE_G4   392
#define NOTE_GS4  415
#define NOTE_A4   440
#define NOTE_AS4  466
#define NOTE_B4   494
#define NOTE_C5   523
#define NOTE_D5   587
#define NOTE_E5   659
#define NOTE_F5   698
#define NOTE_G5   784
#define NOTE_A5   880
#define NOTE_B5   988
#define NOTE_C6   1047
#define REST      0

// ================== AUDIO SEQUENCES ==================

// Boot sequence - Professional startup melody
const AudioNote bootSequence[] = {
  {NOTE_C4, 150}, {NOTE_E4, 150}, {NOTE_G4, 150}, {NOTE_C5, 200},
  {REST, 100}, {NOTE_G4, 100}, {NOTE_C5, 300}, {REST, 200},
  {NOTE_A4, 150}, {NOTE_C5, 150}, {NOTE_E5, 400}
    //  {NOTE_C5, 200}, {REST, 50}, {NOTE_E5, 200}, {REST, 50}, {NOTE_G5, 400}

};

// System ready confirmation
const AudioNote systemReadySequence[] = {
  {NOTE_C5, 100}, {NOTE_E5, 100}, {NOTE_G5, 100}, {NOTE_C6, 200},
  {REST, 100}, {NOTE_C6, 300}
};

// Arm sequence - Confident ascending melody
const AudioNote armSequence[] = {
  {NOTE_F4, 100}, {NOTE_A4, 100}, {NOTE_C5, 120}, {NOTE_F5, 200},
  {REST, 50}, {NOTE_F5, 150}
};

// Disarm sequence - Gentle descending melody
const AudioNote disarmSequence[] = {
  {NOTE_C5, 100}, {NOTE_A4, 100}, {NOTE_F4, 120}, {NOTE_C4, 200},
  {REST, 50}, {NOTE_C4, 150}
};

// Menu enter - Professional entrance tone
const AudioNote menuEnterSequence[] = {
  {NOTE_G4, 80}, {NOTE_B4, 80}, {NOTE_D5, 80}, {NOTE_G5, 120},
  {REST, 50}, {NOTE_D5, 100}
};

// Menu exit - Professional exit tone
const AudioNote menuExitSequence[] = {
  {NOTE_D5, 80}, {NOTE_B4, 80}, {NOTE_G4, 100}
};

// Navigation up - Quick ascending chirp
const AudioNote navUpSequence[] = {
  {NOTE_A4, 60}, {NOTE_C5, 80}
};

// Navigation down - Quick descending chirp
const AudioNote navDownSequence[] = {
  {NOTE_C5, 60}, {NOTE_A4, 80}
};

// Selection/OK - Confident confirmation
const AudioNote selectSequence[] = {
  {NOTE_E5, 80}, {NOTE_G5, 120}
};

// Back/Cancel - Gentle negative feedback
const AudioNote backSequence[] = {
  {NOTE_B4, 60}, {NOTE_F4, 100}
};

// Calibration start - Encouraging sequence
const AudioNote calibrationStartSequence[] = {
  {NOTE_C4, 100}, {NOTE_E4, 100}, {NOTE_G4, 100}, {NOTE_C5, 100},
  {REST, 50}, {NOTE_E5, 150}, {NOTE_C5, 150}
};

// Calibration step - Progress indication
const AudioNote calibrationStepSequence[] = {
  {NOTE_A4, 100}, {NOTE_C5, 120}
};

// Calibration complete - Achievement melody
const AudioNote calibrationCompleteSequence[] = {
  {NOTE_C5, 100}, {NOTE_E5, 100}, {NOTE_G5, 100}, {NOTE_C6, 120},
  {REST, 80}, {NOTE_E5, 80}, {NOTE_G5, 80}, {NOTE_C6, 200}
};

// Save operation - Professional confirmation
const AudioNote saveSequence[] = {
  {NOTE_F4, 80}, {NOTE_A4, 80}, {NOTE_C5, 80}, {NOTE_F5, 150},
  {REST, 50}, {NOTE_A5, 100}
};

// Error/Warning - Clear but not harsh
const AudioNote errorSequence[] = {
  {NOTE_D4, 120}, {REST, 80}, {NOTE_D4, 120}, {REST, 80}, {NOTE_D4, 180}
};

// Success/Achievement - Uplifting melody
const AudioNote successSequence[] = {
  {NOTE_C4, 80}, {NOTE_E4, 80}, {NOTE_G4, 80}, {NOTE_C5, 100},
  {NOTE_E5, 100}, {NOTE_G5, 120}, {NOTE_C6, 200}
};

// Factory reset warning - Serious warning tone
const AudioNote factoryResetWarningSequence[] = {
  {NOTE_C4, 200}, {REST, 100}, {NOTE_C4, 200}, {REST, 100},
  {NOTE_F4, 150}, {NOTE_C4, 250}
};

// Battery low alert - Attention-getting but not annoying
const AudioNote batteryLowSequence[] = {
  {NOTE_E4, 300}, {REST, 200}, {NOTE_E4, 300}, {REST, 200}, {NOTE_E4, 400}
};

// Radio lost alert - Urgent but professional
const AudioNote radioLostSequence[] = {
  {NOTE_A4, 150}, {NOTE_F4, 150}, {NOTE_A4, 150}, {NOTE_F4, 150}, {NOTE_A4, 200}
};

// Test sequence - Playful ascending scale
const AudioNote testSequence[] = {
  {NOTE_C4, 120}, {NOTE_D4, 120}, {NOTE_E4, 120}, {NOTE_F4, 120},
  {NOTE_G4, 120}, {NOTE_A4, 120}, {NOTE_B4, 120}, {NOTE_C5, 200}
};

// ================== FUNCTION DECLARATIONS ==================

void initAudio();
void updateAudio();
void playAudioSequence(const AudioNote* sequence, int length);
void stopAudio();
bool isAudioPlaying();
void setAudioEnabled(bool enabled);
void setAudioVolume(int volume);
void muteAudio();
void unmuteAudio();
bool isAudioMuted();

// System event functions
void playBootMusic();
void playSystemReady();
void playArmSound();
void playDisarmSound();
void playMenuEnterSound();
void playMenuExitSound();
void playNavigationUpSound();
void playNavigationDownSound();
void playSelectSound();
void playBackSound();
void playCalibrationStartSound();
void playCalibrationStepSound();
void playCalibrationCompleteSound();
void playSaveSound();
void playErrorSound();
void playSuccessSound();
void playFactoryResetWarning();
void playBatteryLowAlert();
void playRadioLostAlert();
void playTestSound();

// ================== IMPLEMENTATION ==================

void initAudio() {
  Serial.println("Initializing professional audio system...");
  
  // Setup speaker pin
  pinMode(SPEAKER_PIN, OUTPUT);
  digitalWrite(SPEAKER_PIN, LOW);
  
  // Initialize audio state
  audioState.isPlaying = false;
  audioState.sequencePlaying = false;
  audioState.currentSequence = nullptr;
  audioState.sequenceLength = 0;
  audioState.sequenceIndex = 0;
  audioState.noteStartTime = 0;
  audioState.lastUpdateTime = 0;
  audioState.muted = false;
  
  // Load settings (integrate with existing EEPROM system)
  audioSettings.enabled = true;
  audioSettings.volume = 75;
  audioSettings.systemSounds = true;
  audioSettings.navigationSounds = true;
  audioSettings.alertSounds = true;
  audioSettings.musicEnabled = true;
  
  Serial.println("Audio system initialized successfully!");
}

void updateAudio() {
  if (!audioSettings.enabled || audioState.muted) return;
  
  unsigned long currentTime = millis();
  
  if (audioState.sequencePlaying && audioState.currentSequence) {
    // Check if current note should end
    if (currentTime - audioState.noteStartTime >= audioState.currentSequence[audioState.sequenceIndex].duration) {
      // Stop current note
      noTone(SPEAKER_PIN);
      audioState.isPlaying = false;
      
      // Move to next note
      audioState.sequenceIndex++;
      
      if (audioState.sequenceIndex >= audioState.sequenceLength) {
        // Sequence complete
        audioState.sequencePlaying = false;
        audioState.currentSequence = nullptr;
        audioState.sequenceIndex = 0;
      } else {
        // Start next note
        int frequency = audioState.currentSequence[audioState.sequenceIndex].frequency;
        
        if (frequency > 0) {
          // Apply volume scaling (simple frequency-based approach for piezo)
          int scaledFreq = frequency;
          if (audioSettings.volume < 100) {
            // For piezo, we can't really change volume, but we can vary the frequency slightly
            // or use shorter durations for "quieter" effect
            scaledFreq = frequency; // Keep frequency pure for now
          }
          
          tone(SPEAKER_PIN, scaledFreq);
          audioState.isPlaying = true;
        } else {
          // Rest note (silence)
          audioState.isPlaying = false;
        }
        
        audioState.noteStartTime = currentTime;
      }
    }
  }
  
  audioState.lastUpdateTime = currentTime;
}

void playAudioSequence(const AudioNote* sequence, int length) {
  if (!audioSettings.enabled || audioState.muted || !sequence || length <= 0) return;
  
  // Stop any current audio
  stopAudio();
  
  // Start new sequence
  audioState.currentSequence = sequence;
  audioState.sequenceLength = length;
  audioState.sequenceIndex = 0;
  audioState.sequencePlaying = true;
  audioState.noteStartTime = millis();
  
  // Start first note
  int frequency = sequence[0].frequency;
  if (frequency > 0) {
    tone(SPEAKER_PIN, frequency);
    audioState.isPlaying = true;
  } else {
    audioState.isPlaying = false;
  }
}

void stopAudio() {
  noTone(SPEAKER_PIN);
  audioState.isPlaying = false;
  audioState.sequencePlaying = false;
  audioState.currentSequence = nullptr;
  audioState.sequenceIndex = 0;
}

bool isAudioPlaying() {
  return audioState.isPlaying || audioState.sequencePlaying;
}

void setAudioEnabled(bool enabled) {
  audioSettings.enabled = enabled;
  if (!enabled) {
    stopAudio();
  }
  // TODO: Save to EEPROM with existing settings system
}

void setAudioVolume(int volume) {
  audioSettings.volume = constrain(volume, 0, 100);
  // TODO: Save to EEPROM with existing settings system
}

void muteAudio() {
  audioState.muted = true;
  stopAudio();
}

void unmuteAudio() {
  audioState.muted = false;
}

bool isAudioMuted() {
  return audioState.muted;
}

// ================== SYSTEM EVENT FUNCTIONS ==================

void playBootMusic() {
  if (!audioSettings.musicEnabled) return;
  playAudioSequence(bootSequence, sizeof(bootSequence) / sizeof(AudioNote));
}

void playSystemReady() {
  if (!audioSettings.systemSounds) return;
  playAudioSequence(systemReadySequence, sizeof(systemReadySequence) / sizeof(AudioNote));
}

void playArmSound() {
  if (!audioSettings.systemSounds) return;
  playAudioSequence(armSequence, sizeof(armSequence) / sizeof(AudioNote));
}

void playDisarmSound() {
  if (!audioSettings.systemSounds) return;
  playAudioSequence(disarmSequence, sizeof(disarmSequence) / sizeof(AudioNote));
}

void playMenuEnterSound() {
  if (!audioSettings.navigationSounds) return;
  playAudioSequence(menuEnterSequence, sizeof(menuEnterSequence) / sizeof(AudioNote));
}

void playMenuExitSound() {
  if (!audioSettings.navigationSounds) return;
  playAudioSequence(menuExitSequence, sizeof(menuExitSequence) / sizeof(AudioNote));
}

void playNavigationUpSound() {
  if (!audioSettings.navigationSounds) return;
  playAudioSequence(navUpSequence, sizeof(navUpSequence) / sizeof(AudioNote));
}

void playNavigationDownSound() {
  if (!audioSettings.navigationSounds) return;
  playAudioSequence(navDownSequence, sizeof(navDownSequence) / sizeof(AudioNote));
}

void playSelectSound() {
  if (!audioSettings.navigationSounds) return;
  playAudioSequence(selectSequence, sizeof(selectSequence) / sizeof(AudioNote));
}

void playBackSound() {
  if (!audioSettings.navigationSounds) return;
  playAudioSequence(backSequence, sizeof(backSequence) / sizeof(AudioNote));
}

void playCalibrationStartSound() {
  if (!audioSettings.systemSounds) return;
  playAudioSequence(calibrationStartSequence, sizeof(calibrationStartSequence) / sizeof(AudioNote));
}

void playCalibrationStepSound() {
  if (!audioSettings.systemSounds) return;
  playAudioSequence(calibrationStepSequence, sizeof(calibrationStepSequence) / sizeof(AudioNote));
}

void playCalibrationCompleteSound() {
  if (!audioSettings.systemSounds) return;
  playAudioSequence(calibrationCompleteSequence, sizeof(calibrationCompleteSequence) / sizeof(AudioNote));
}

void playSaveSound() {
  if (!audioSettings.systemSounds) return;
  playAudioSequence(saveSequence, sizeof(saveSequence) / sizeof(AudioNote));
}

void playErrorSound() {
  if (!audioSettings.alertSounds) return;
  playAudioSequence(errorSequence, sizeof(errorSequence) / sizeof(AudioNote));
}

void playSuccessSound() {
  if (!audioSettings.systemSounds) return;
  playAudioSequence(successSequence, sizeof(successSequence) / sizeof(AudioNote));
}

void playFactoryResetWarning() {
  if (!audioSettings.alertSounds) return;
  playAudioSequence(factoryResetWarningSequence, sizeof(factoryResetWarningSequence) / sizeof(AudioNote));
}

void playBatteryLowAlert() {
  if (!audioSettings.alertSounds) return;
  playAudioSequence(batteryLowSequence, sizeof(batteryLowSequence) / sizeof(AudioNote));
}

void playRadioLostAlert() {
  if (!audioSettings.alertSounds) return;
  playAudioSequence(radioLostSequence, sizeof(radioLostSequence) / sizeof(AudioNote));
}

void playTestSound() {
  playAudioSequence(testSequence, sizeof(testSequence) / sizeof(AudioNote));
}

#endif