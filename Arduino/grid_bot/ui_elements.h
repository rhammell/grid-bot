#ifndef UI_ELEMENTS_H
#define UI_ELEMENTS_H

#include <Adafruit_ILI9341.h>
#include <Arduino.h>
#include "grid_model.h"

// UI Configuration Constants
#define CELL_SIZE 30

// Background color
const uint16_t BACKGROUND_COLOR = 0x0000; // Black

// Button dimensions
const int BUTTON_HEIGHT = 36;
const int BUTTON_MARGIN = 2;
const int UNDO_BUTTON_WIDTH = 36;
const int SETTINGS_BUTTON_WIDTH = 36;

// Button colors
const uint16_t BUTTON_IDLE_COLOR =  0x4a7f;     // Blue (75, 75, 225)
const uint16_t BUTTON_COUNTING_COLOR = 0xebe4;  // Orange (240, 124, 36)
const uint16_t BUTTON_RUNNING_COLOR = 0xfa69;   // Red (255, 75, 75)
const uint16_t BUTTON_COMPLETE_COLOR = 0x4fe9;  // Green (75, 255, 75)
const uint16_t BUTTON_TEXT_COLOR = 0xFFFF;      // White (255, 255, 255)

// Grid colors
const uint16_t GRID_BACKGROUND_COLOR = 0x0000;    // Black (0, 0, 0)
const uint16_t GRID_SELECTED_COLOR = 0x0000;      // Black (0, 0, 0)
const uint16_t GRID_EMPTY_COLOR = 0xFFFF;         // White (255, 255, 255)
const uint16_t GRID_LINE_COLOR = 0x8430;          // Dark grey (132, 132, 132)
const uint16_t GRID_SELECTABLE_COLOR = 0x4fe9;    // Green (75, 255, 75)
const uint16_t GRID_ARROW_COLOR = 0xFFFF;         // White (255, 255, 255)

// Settings menu colors
const uint16_t SETTINGS_MENU_BG_COLOR = 0x8430;     // Dark grey (132, 132, 132)
const uint16_t SETTINGS_MENU_TEXT_COLOR = 0xFFFF;   // White (255, 255, 255)
const uint16_t SETTINGS_MENU_BUTTON_COLOR = 0xFFFF; // White (255, 255, 255)

// UI State enum (moved from grid_bot.ino)
enum UIState {
  IDLE,
  COUNTING,
  RUNNING,
  SETTINGS,
  COMPLETE
};

// Settings menu dimensions
const int SETTINGS_TEXT_SIZE = 2;
const int SETTINGS_ARROW_WIDTH = 38;
const int SETTINGS_ARROW_HEIGHT = 30;
const int SETTINGS_ARROW_MARGIN_X = 2;

// Base class now only contains what is common to ALL buttons.
class UIButton {
public:
  int x;
  int y;
  int width;
  int height;
  uint16_t bgColor;

  UIButton() : x(0), y(0), width(0), height(0), bgColor(0) {}

  void setBounds(int bx, int by, int w, int h) {
    x = bx;
    y = by;
    width = w;
    height = h;
  }

  bool contains(int px, int py) const {
    return px >= x && px <= x + width && py >= y && py <= y + height;
  }

  void setBgColor(uint16_t color) { bgColor = color; }

  // The base draw method is now virtual.
  virtual void draw(Adafruit_ILI9341 &tft) const;
};

class UITextButton : public UIButton {
public:
  String label;
  uint16_t textColor;
  uint8_t textSize;

  UITextButton() : label(""), textColor(0), textSize(2) {}

  void setLabel(const String &l) { label = l; }
  void setTextColor(uint16_t c) { textColor = c; }
  void setTextSize(uint8_t size) { textSize = size; }

  // This is the primary drawing method for this class.
  void draw(Adafruit_ILI9341 &tft) const override;
};

class UIIconButton : public UIButton {
public:
  const uint16_t *icon;
  int iconWidth;
  int iconHeight;

  UIIconButton() : icon(nullptr), iconWidth(0), iconHeight(0) {}

  void setIcon(const uint16_t *i, int w, int h) {
    icon = i;
    iconWidth = w;
    iconHeight = h;
  }

  // This is the primary drawing method for this class.
  void draw(Adafruit_ILI9341 &tft) const override;
};

enum ArrowDirection { ARROW_LEFT, ARROW_RIGHT };

class UIArrow : public UIButton {
public:
  ArrowDirection dir;
  int triangleWidth;   // New attribute for explicit triangle width
  int triangleHeight;  // New attribute for explicit triangle height
  uint16_t triangleColor;  // New attribute for triangle color

  UIArrow() : dir(ARROW_LEFT), triangleWidth(12), triangleHeight(18), triangleColor(ILI9341_WHITE) {}  // Default values based on previous 60% of 20x30

  void setDirection(ArrowDirection d) { dir = d; }
  void setTriangleSize(int width, int height) {  // New setter method
    triangleWidth = width;
    triangleHeight = height;
  }
  void setTriangleColor(uint16_t color) { triangleColor = color; }

  // Override the draw method to draw the arrow
  void draw(Adafruit_ILI9341 &tft) const override;
};

class UISettingsValue {
public:
    int x;
    int y;
    int width;
    uint16_t backgroundColor;
    uint16_t textColor;
    uint8_t textSize;
    int charWidth;  // NEW: character width in pixels
    int charHeight; // NEW: character height in pixels
    String value;

    UISettingsValue() : x(0), y(0), width(0),
                       backgroundColor(ILI9341_DARKGREY),
                       textColor(ILI9341_WHITE),
                       textSize(2),
                       charWidth(6),  // Default font char width
                       charHeight(8), // Default font char height
                       value("") {}

    void setPosition(int bx, int by, int w) {
        x = bx;
        y = by;
        width = w;
    }

    void setColors(uint16_t bg, uint16_t txt) {
        backgroundColor = bg;
        textColor = txt;
    }

    void setTextSize(uint8_t size) { textSize = size; }
    void setValue(const String &val) { value = val; }
    
    // NEW: Set font metrics
    void setFontMetrics(int cw, int ch) {
        charWidth = cw;
        charHeight = ch;
    }

    void draw(Adafruit_ILI9341 &tft) const;
};

class UISettingsOption {
public:
    String label;
    UIArrow leftArrow;
    UIArrow rightArrow;
    UISettingsValue value;
    int x;
    int y;
    int width;
    int height;
    uint16_t backgroundColor;
    uint16_t textColor;
    uint8_t textSize;
    int arrowWidth;
    int arrowHeight;
    int arrowMarginX;
    int optionSpacing;
    int labelX;
    int labelY;

    UISettingsOption() : label(""), x(0), y(0), width(0), height(0),
                        backgroundColor(ILI9341_DARKGREY),
                        textColor(ILI9341_WHITE),
                        textSize(2),
                        arrowWidth(38),
                        arrowHeight(30),
                        arrowMarginX(2),
                        optionSpacing(70) {}

    void setPosition(int bx, int by, int w, int h) {
        x = bx;
        y = by;
        width = w;
        height = h;
    }

    void setLabel(const String &l) { label = l; }
    void setColors(uint16_t bg, uint16_t txt) {
        backgroundColor = bg;
        textColor = txt;
    }
    void setTextSize(uint8_t size) { textSize = size; }
    void setArrowSize(int w, int h) {
        arrowWidth = w;
        arrowHeight = h;
    }
    void setArrowMargin(int margin) { arrowMarginX = margin; }
    void setOptionSpacing(int spacing) { optionSpacing = spacing; }
    void setValue(const String &val) { value.setValue(val); }

    void layout();
    void draw(Adafruit_ILI9341 &tft) const;
    bool isLeftArrowTouched(int px, int py) const;
    bool isRightArrowTouched(int px, int py) const;
};

class UISettingsMenu {
public:
    int x;
    int y;
    int width;
    int height;
    uint16_t backgroundColor;
    uint16_t borderColor;
    uint16_t textColor;
    uint8_t textSize;
    int fontCharWidth;
    int fontCharHeight;
    int fontPadding;
    int arrowWidth;
    int arrowHeight;
    int arrowMarginX;
    int optionSpacing;

    UISettingsMenu() : x(0), y(0), width(0), height(0),
                      backgroundColor(ILI9341_DARKGREY),
                      borderColor(ILI9341_WHITE),
                      textColor(ILI9341_WHITE),
                      textSize(2),
                      fontCharWidth(6),
                      fontCharHeight(8),
                      fontPadding(2),
                      arrowWidth(38),
                      arrowHeight(30),
                      arrowMarginX(2),
                      optionSpacing(70),
                      numOptions(0) {}

    void setPosition(int bx, int by, int w, int h) {
        x = bx;
        y = by;
        width = w;
        height = h;
    }

    void setColors(uint16_t bg, uint16_t border, uint16_t txt) {
        backgroundColor = bg;
        borderColor = border;
        textColor = txt;
    }

    void setTextSize(uint8_t size) { textSize = size; }
    void setFontMetrics(int cw, int ch) {
        fontCharWidth = cw;
        fontCharHeight = ch;
    }
    void setArrowSize(int w, int h) {
        arrowWidth = w;
        arrowHeight = h;
    }
    void setArrowMargin(int margin) { arrowMarginX = margin; }
    void setOptionSpacing(int spacing) { optionSpacing = spacing; }

    void layout();
    void draw(Adafruit_ILI9341 &tft) const;
    int getTouchedOption(int px, int py) const; // Returns option index or -1 if none
    bool isLeftArrowTouched(int px, int py, int optionIndex) const;
    bool isRightArrowTouched(int px, int py, int optionIndex) const;
    
    // Helper methods for setup and updates
    void setupOptions(const String* labels, int count);
    void updateOptionValue(int optionIndex, const String& value);

    // Add this method to the UISettingsMenu class
    void redrawOption(int optionIndex, Adafruit_ILI9341 &tft) const;

private:
    static const int MAX_OPTIONS = 3;
    UISettingsOption options[MAX_OPTIONS];
    int numOptions;
};

// --- UIGrid class for grid UI management ---
class UIGrid {
public:
    int x, y, width, height;
    int numRows, numCols;

    UIGrid();
    void setSize(int rows, int cols);
    void setBounds(int bx, int by, int w, int h);
    void drawGridLines(Adafruit_ILI9341 &tft);
    void drawGridCells(Adafruit_ILI9341 &tft, GridModel &model, UIState state);
    void drawGridCells(Adafruit_ILI9341 &tft, GridModel &model, UIState state, int startRow, int endRow, int startCol, int endCol);
    static bool isPointInRect(int x, int y, int rectX, int rectY, int rectWidth, int rectHeight);
    bool contains(int x, int y) const;
};

#endif
