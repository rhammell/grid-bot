#include "ui_elements.h"

// This helper is still useful for subclasses
void UIButton::draw(Adafruit_ILI9341 &tft) const {
  tft.fillRect(x, y, width, height, bgColor);
}

// Implementation is moved here. It now uses member variables.
void UITextButton::draw(Adafruit_ILI9341 &tft) const {
  UIButton::draw(tft); // Draw the background first!

  tft.setTextColor(textColor);
  tft.setTextSize(textSize);
  int charWidth = 6 * textSize;
  int textWidth = label.length() * charWidth;
  int textX = x + (width - textWidth) / 2;
  int textY = y + (height - (8 * textSize)) / 2;
  tft.setCursor(textX, textY);
  tft.print(label);
}

void UIIconButton::draw(Adafruit_ILI9341 &tft) const {
  UIButton::draw(tft);

  int iconX = x + (width - iconWidth) / 2;
  int iconY = y + (height - iconHeight) / 2;
  tft.drawRGBBitmap(iconX, iconY, icon, iconWidth, iconHeight);
}

void UIArrow::draw(Adafruit_ILI9341 &tft) const {
  // Draw the background first
  UIButton::draw(tft);

  // Calculate the triangle points to be centered within the button bounds
  int centerX = x + width / 2;
  int centerY = y + height / 2;

  if (dir == ARROW_LEFT) {
    tft.fillTriangle(
        centerX - triangleWidth/2, centerY,  // Left point
        centerX + triangleWidth/2, y + (height - triangleHeight) / 2,  // Top point
        centerX + triangleWidth/2, y + (height + triangleHeight) / 2,  // Bottom point
        triangleColor);  // Use the triangle color for the arrow
  } else {
    tft.fillTriangle(
        centerX + triangleWidth/2, centerY,  // Right point
        centerX - triangleWidth/2, y + (height - triangleHeight) / 2,  // Top point
        centerX - triangleWidth/2, y + (height + triangleHeight) / 2,  // Bottom point
        triangleColor);  // Use the triangle color for the arrow
  }
}

void UISettingsValue::draw(Adafruit_ILI9341 &tft) const {
    // Clear the entire value area using calculated height
    int textHeight = charHeight * textSize;
    tft.fillRect(x, y, width, textHeight, backgroundColor);

    // Set text properties
    tft.setTextSize(textSize);
    tft.setTextColor(textColor);
    
    // Calculate centered position for text using charWidth
    int fontPadding = textSize;
    int scaledCharWidth = charWidth * textSize;
    int actualTextWidth = value.length() * scaledCharWidth - fontPadding;
    int textX = x + (width - actualTextWidth) / 2;
    
    tft.setCursor(textX, y);
    tft.print(value);
}
