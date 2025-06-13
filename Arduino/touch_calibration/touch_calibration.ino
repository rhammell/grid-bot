#include <SPI.h>                // SPI communication library
#include <Adafruit_GFX.h>       // Core graphics library
#include <Adafruit_ILI9341.h>   // Specific driver for the ILI9341 display
#include "TouchScreen.h"       // Resistive touchscreen driver

// --- TFT display pin assignments ---
#define TFT_CS 17   // Chip select
#define TFT_DC 18   // Data/command
#define TFT_RST 19  // Reset line
#define TFT_LED 20  // Backlight control

// --- Resistive touchscreen pins ---
#define YP A4  // Must be an analog pin
#define XP 22  // Can be any digital pin
#define YM 23  // Can be any digital pin
#define XM A7  // Must be an analog pin

// --- Display and touch objects ---
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST); // TFT driver
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);               // Touch driver

// Structure to store a single raw touch reading
struct TouchPoint { int x; int y; };

// Buffer of four corner samples
TouchPoint samples[4];

// Current sample being requested
int sampleIndex = 0;

// Draw a crosshair in one of the four corners to prompt the user
void drawCrosshair(int index) {
  int padding = 20;        // Distance from edge for the crosshair
  int x = padding;         // Default to upper-left corner
  int y = padding;

  // Choose corner based on sample index
  switch (index) {
    case 0: x = padding; y = padding; break;                          // Top-left
    case 1: x = tft.width() - padding; y = padding; break;            // Top-right
    case 2: x = tft.width() - padding; y = tft.height() - padding; break; // Bottom-right
    case 3: x = padding; y = tft.height() - padding; break;           // Bottom-left
  }

  // Clear the screen and draw the lines forming the crosshair
  tft.fillScreen(ILI9341_BLACK);
  tft.drawFastHLine(x - 5, y, 10, ILI9341_WHITE);
  tft.drawFastVLine(x, y - 5, 10, ILI9341_WHITE);

  // Inform the user which crosshair to press
  Serial.print("Touch crosshair ");
  Serial.println(index + 1);
}

// Arduino setup function -- runs once at boot
void setup() {
  Serial.begin(9600);       // Initialize serial output

  // Enable the TFT backlight at full brightness
  pinMode(TFT_LED, OUTPUT);
  analogWrite(TFT_LED, 255);

  // Initialize the display
  tft.begin();
  tft.setRotation(0);       // Portrait orientation
  tft.fillScreen(ILI9341_BLACK);

  // Print instructions and show the first crosshair
  Serial.println("Touchscreen calibration");
  drawCrosshair(sampleIndex);
}

// Main loop -- collect touches from all four corners
void loop() {
  // Read raw coordinates from the touchscreen
  TSPoint p = ts.getPoint();

  // Check if the screen is being touched
  if (p.z > ts.pressureThreshhold) {
    // Store the raw reading
    samples[sampleIndex].x = p.x;
    samples[sampleIndex].y = p.y;
    sampleIndex++;
    delay(500);  // Debounce delay

    // Prompt for the next corner or finish calibration
    if (sampleIndex < 4) {
      drawCrosshair(sampleIndex);
    } else {
      // Determine min/max for X and Y from the four samples
      int minX = min(min(samples[0].x, samples[1].x), min(samples[2].x, samples[3].x));
      int maxX = max(max(samples[0].x, samples[1].x), max(samples[2].x, samples[3].x));
      int minY = min(min(samples[0].y, samples[2].y), min(samples[1].y, samples[3].y));
      int maxY = max(max(samples[0].y, samples[2].y), max(samples[1].y, samples[3].y));

      // Print out calibration macros for the user to copy
      Serial.println();
      Serial.println("Calibration complete. Use these values:");
      Serial.print("#define TOUCH_MIN_X "); Serial.println(minX);
      Serial.print("#define TOUCH_MAX_X "); Serial.println(maxX);
      Serial.print("#define TOUCH_MIN_Y "); Serial.println(minY);
      Serial.print("#define TOUCH_MAX_Y "); Serial.println(maxY);

      while (1); // Halt program once calibration is done
    }
  }
}

