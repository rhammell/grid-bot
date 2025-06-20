#include "settings_manager.h"

// Settings option labels
const String SETTINGS_LABELS[] = { "Brightness", "Drive Speed", "Drive Distance" };
const char* DRIVE_SPEED_LABELS[] = { "Slow", "Standard", "Fast" };
const char* DRIVE_DISTANCE_LABELS[] = { "Compact", "Standard", "Extended" };

// Constructor
SettingsManager::SettingsManager() {
  // Initialize with default values
  displayBrightness = 60;
  driveSpeed = SPEED_STANDARD;
  driveDistance = DISTANCE_STANDARD;
}

// Brightness methods
int SettingsManager::getDisplayBrightness() const {
  return displayBrightness;
}

void SettingsManager::setDisplayBrightness(int brightness) {
  displayBrightness = constrain(brightness, 0, 100);
}

void SettingsManager::adjustBrightness(int delta) {
  setDisplayBrightness(displayBrightness + delta);
}

int SettingsManager::getBrightnessPWM() const {
  // Convert percentage (0-100) to PWM value (0-255)
  return map(displayBrightness, 0, 100, 0, 255);
}

// Drive speed methods
DriveSpeed SettingsManager::getDriveSpeed() const {
  return driveSpeed;
}

void SettingsManager::setDriveSpeed(DriveSpeed speed) {
  driveSpeed = speed;
}

void SettingsManager::increaseDriveSpeed() {
  if (driveSpeed < SPEED_FAST) {
    driveSpeed = (DriveSpeed)(driveSpeed + 1);
  }
}

void SettingsManager::decreaseDriveSpeed() {
  if (driveSpeed > SPEED_SLOW) {
    driveSpeed = (DriveSpeed)(driveSpeed - 1);
  }
}

const char* SettingsManager::getDriveSpeedLabel() const {
  return DRIVE_SPEED_LABELS[driveSpeed];
}

// Drive distance methods
DriveDistance SettingsManager::getDriveDistance() const {
  return driveDistance;
}

void SettingsManager::setDriveDistance(DriveDistance distance) {
  driveDistance = distance;
}

void SettingsManager::increaseDriveDistance() {
  if (driveDistance < DISTANCE_EXTENDED) {
    driveDistance = (DriveDistance)(driveDistance + 1);
  }
}

void SettingsManager::decreaseDriveDistance() {
  if (driveDistance > DISTANCE_COMPACT) {
    driveDistance = (DriveDistance)(driveDistance - 1);
  }
}

const char* SettingsManager::getDriveDistanceLabel() const {
  return DRIVE_DISTANCE_LABELS[driveDistance];
}

// Settings labels
const String* SettingsManager::getSettingsLabels() {
  return SETTINGS_LABELS;
}

int SettingsManager::getSettingsLabelsCount() {
  return sizeof(SETTINGS_LABELS) / sizeof(SETTINGS_LABELS[0]);
}

// Utility methods
void SettingsManager::resetToDefaults() {
  displayBrightness = 60;
  driveSpeed = SPEED_STANDARD;
  driveDistance = DISTANCE_STANDARD;
} 