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

void UISettingsOption::layout() {
    // Calculate character width based on text size
    int charWidth = 6 * textSize;
    int fontPadding = textSize;
    
    // Calculate label position (centered)
    int labelWidth = label.length() * charWidth - fontPadding;
    labelX = x + (width - labelWidth) / 2;
    labelY = y;
    
    // Calculate arrow positions
    int arrowY = labelY + 20;
    int leftArrowX = x + arrowMarginX;
    int rightArrowX = x + width - arrowMarginX - arrowWidth;
    
    // Set up arrows
    leftArrow.setBounds(leftArrowX, arrowY, arrowWidth, arrowHeight);
    leftArrow.setDirection(ARROW_LEFT);
    leftArrow.setTriangleSize(12, 18);
    leftArrow.setBgColor(backgroundColor);
    leftArrow.setTriangleColor(textColor);
    
    rightArrow.setBounds(rightArrowX, arrowY, arrowWidth, arrowHeight);
    rightArrow.setDirection(ARROW_RIGHT);
    rightArrow.setTriangleSize(12, 18);
    rightArrow.setBgColor(backgroundColor);
    rightArrow.setTriangleColor(textColor);
    
    // Calculate value position
    int maxValueChars = 8; // "Standard" is the longest label
    int maxValueWidth = maxValueChars * charWidth - fontPadding;
    int valueX = x + (width - maxValueWidth) / 2;
    int valueY = arrowY + (arrowHeight - textSize * 8) / 2;
    
    value.setPosition(valueX, valueY, maxValueWidth);
    value.setColors(backgroundColor, textColor);
    value.setTextSize(textSize);
    value.setFontMetrics(6, 8);
}

void UISettingsOption::draw(Adafruit_ILI9341 &tft) const {
    // Set text properties for label
    tft.setTextSize(textSize);
    tft.setTextColor(textColor);
    
    // Draw label using precomputed labelX and labelY
    tft.setCursor(labelX, labelY);
    tft.print(label);
    
    // Draw arrows
    leftArrow.draw(tft);
    rightArrow.draw(tft);
    
    // Draw value
    value.draw(tft);
}

bool UISettingsOption::isLeftArrowTouched(int px, int py) const {
    return leftArrow.contains(px, py);
}

bool UISettingsOption::isRightArrowTouched(int px, int py) const {
    return rightArrow.contains(px, py);
}

void UISettingsMenu::layout() {
    // Set up each option with proper positioning
    for (int i = 0; i < numOptions; i++) {
        int optionY = y + 20 + (i * optionSpacing);
        options[i].setPosition(x, optionY, width, optionSpacing);
        options[i].setColors(backgroundColor, textColor);
        options[i].setTextSize(textSize);
        options[i].setArrowSize(arrowWidth, arrowHeight);
        options[i].setArrowMargin(arrowMarginX);
        options[i].layout();
    }
}

void UISettingsMenu::draw(Adafruit_ILI9341 &tft) const {
    // Draw menu background
    tft.fillRect(x, y, width, height, backgroundColor);
    tft.drawRect(x, y, width, height, borderColor);
    
    // Draw all options
    for (int i = 0; i < numOptions; i++) {
        options[i].draw(tft);
    }
}

int UISettingsMenu::getTouchedOption(int px, int py) const {
    for (int i = 0; i < numOptions; i++) {
        if (px >= options[i].x && px <= options[i].x + options[i].width &&
            py >= options[i].y && py <= options[i].y + options[i].height) {
            return i;
        }
    }
    return -1;
}

bool UISettingsMenu::isLeftArrowTouched(int px, int py, int optionIndex) const {
    if (optionIndex >= 0 && optionIndex < numOptions) {
        return options[optionIndex].isLeftArrowTouched(px, py);
    }
    return false;
}

bool UISettingsMenu::isRightArrowTouched(int px, int py, int optionIndex) const {
    if (optionIndex >= 0 && optionIndex < numOptions) {
        return options[optionIndex].isRightArrowTouched(px, py);
    }
    return false;
}

void UISettingsMenu::setupOptions(const String* labels, int count) {
    numOptions = min(count, MAX_OPTIONS);
    for (int i = 0; i < numOptions; i++) {
        options[i].setLabel(labels[i]);
    }
}

void UISettingsMenu::updateOptionValue(int optionIndex, const String& value) {
    if (optionIndex >= 0 && optionIndex < numOptions) {
        options[optionIndex].setValue(value);
    }
}

void UISettingsMenu::redrawOption(int optionIndex, Adafruit_ILI9341 &tft) const {
    if (optionIndex >= 0 && optionIndex < numOptions) {
        options[optionIndex].draw(tft);
    }
}
