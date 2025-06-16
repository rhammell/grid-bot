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

void UIArrow::draw(Adafruit_ILI9341 &tft, uint16_t color) const {
  if (dir == ARROW_LEFT) {
    tft.fillTriangle(
        x, y + height / 2,
        x + width, y + height / 4,
        x + width, y + (3 * height) / 4,
        color);
  } else {
    tft.fillTriangle(
        x + width, y + height / 2,
        x, y + height / 4,
        x, y + (3 * height) / 4,
        color);
  }
}
