#ifndef SETTINGS_MANAGER_H
#define SETTINGS_MANAGER_H

#include <Arduino.h>

// Drive speed enum
enum DriveSpeed {
  SPEED_SLOW,
  SPEED_STANDARD,
  SPEED_FAST
};

// Drive distance enum
enum DriveDistance {
  DISTANCE_COMPACT,
  DISTANCE_STANDARD,
  DISTANCE_EXTENDED
};

// Settings option labels
extern const String SETTINGS_LABELS[];
extern const char* DRIVE_SPEED_LABELS[];
extern const char* DRIVE_DISTANCE_LABELS[];

class SettingsManager {
private:
  int displayBrightness;
  DriveSpeed driveSpeed;
  DriveDistance driveDistance;
  
public:
  // Constructor
  SettingsManager();
  
  // Brightness methods
  int getDisplayBrightness() const;
  void setDisplayBrightness(int brightness);
  void adjustBrightness(int delta);
  int getBrightnessPWM() const;
  
  // Drive speed methods
  DriveSpeed getDriveSpeed() const;
  void setDriveSpeed(DriveSpeed speed);
  void increaseDriveSpeed();
  void decreaseDriveSpeed();
  const char* getDriveSpeedLabel() const;
  
  // Drive distance methods
  DriveDistance getDriveDistance() const;
  void setDriveDistance(DriveDistance distance);
  void increaseDriveDistance();
  void decreaseDriveDistance();
  const char* getDriveDistanceLabel() const;
  
  // Settings labels
  static const String* getSettingsLabels();
  static int getSettingsLabelsCount();
  
  // Utility methods
  void resetToDefaults();
};

#endif // SETTINGS_MANAGER_H 