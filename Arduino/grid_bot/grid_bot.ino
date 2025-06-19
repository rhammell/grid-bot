#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include "TouchScreen.h"
#include "icons.h"
#include "ui_elements.h"
#include "grid_model.h"

// TFT Pins
#define TFT_CS 17
#define TFT_DC 18
#define TFT_RST 19
#define TFT_LED 20

// TFT display object
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);

// Touchscreen Pins
#define YP A4  // must be an analog pin, use "An" notation!
#define XP 22  // can be any digital pin
#define YM 23  // can be any digital pin
#define XM A7  // must be an analog pin, use "An" notation!

// Touchscreen object
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

// Touch screen calibration values
#define TOUCH_MIN_X 788
#define TOUCH_MAX_X 995
#define TOUCH_MIN_Y 782
#define TOUCH_MAX_Y 993

// Font metrics
const int FONT_CHAR_WIDTH = 6;
const int FONT_CHAR_HEIGHT = 8;

// Display brightness percent
int displayBrightness = 60;

// Screen dimensions
int screenWidth;
int screenHeight;

// Grid model instance
GridModel gridModel;

// Button dimension values
int buttonHeight = 36;
int buttonMargin = 2;

// Undo button position params
int undoButtonWidth = 36;
int settingsButtonWidth = 36;

// Button display colors
int buttonIdleColor = tft.color565(75, 75, 225);
int buttonCountingColor = tft.color565(240, 124, 36);
int buttonRunningColor = tft.color565(255, 75, 75);
int buttonCompleteColor = tft.color565(75, 255, 75);
int buttonTextColor = ILI9341_WHITE;

// Button UI elements
UIIconButton undoButton;
UITextButton startButton;
UIIconButton settingsButton;

// Settings menu UI elements
UISettingsMenu settingsMenu;

// Settings option labels
const String SETTINGS_LABELS[] = { "Brightness", "Drive Speed", "Drive Distance" };

// Settings menu text params
int settingsTextSize = 2;

// Settings menu arrow params
int settingsArrowWidth = 38;
int settingsArrowHeight = 30;
int settingsArrowMarginX = 2;

// Color scheme for settings menu
uint16_t settingsMenuBackgroundColor = ILI9341_DARKGREY;
uint16_t settingsMenuTextColor = ILI9341_WHITE;
uint16_t settingsMenuButtonColor = tft.color565(100, 100, 100);

// States
enum UIState {
  IDLE,
  COUNTING,
  RUNNING,
  SETTINGS,
  COMPLETE
};

// Set current state
UIState uiState = IDLE;

// Driving state enum
enum DriveState {
  STOPPED,
  TURNING,
  DRIVING
};

// Set current state
DriveState driveState = STOPPED;

// Drive speed enum
enum DriveSpeed {
  SPEED_SLOW,
  SPEED_STANDARD,
  SPEED_FAST
};

// Set current drive speed
DriveSpeed driveSpeed = SPEED_STANDARD;

// Drive speed labels
const char* DRIVE_SPEED_LABELS[] = { "Slow", "Standard", "Fast" };

// Grid size enum
enum DriveDistance {
  DISTANCE_COMPACT,
  DISTANCE_STANDARD,
  DISTANCE_EXTENDED
};

// Set current grid size
DriveDistance driveDistance = DISTANCE_STANDARD;

// Grid size labels
const char* DRIVE_DISTANCE_LABELS[] = { "Compact", "Standard", "Extended" };

// Default countdown state
unsigned long countdownStart = 0;
const int countdownDuration = 5000;
int countdownNumber = -1;

// Movement timing constants
const unsigned long FORWARD_MOVE_TIME = 2000;  // Time to move forward one cell
const unsigned long TURN_MOVE_TIME = 2000;     // Time to execute a 90-degree turn

// Current movement tracking
unsigned long moveStartTime;  // Start time of current movement
bool isTurning = false;        // Whether bot is currently executing a turn

void setup() {
  Serial.begin(9600);

  // Set ADC resolution for touchsceen
  analogReadResolution(10);

  // Configure display LED pin
  pinMode(TFT_LED, OUTPUT);

  // Set display brightness
  setBrightness();

  // start by disabling both SD and TFT
  pinMode(SS, OUTPUT);
  digitalWrite(SS, HIGH);

  // Initialize display object
  tft.begin();
  tft.setRotation(0);

  // Fill screen with black background
  tft.fillScreen(ILI9341_BLACK);

  // Dynamically get screen dimensions
  screenWidth = tft.width();
  screenHeight = tft.height();

  // Initialize grid model
  int availableHeight = screenHeight - buttonHeight - buttonMargin - 1;
  gridModel.initGrid(screenWidth, availableHeight);

  // Initialize settings menu
  settingsMenu.setupOptions(SETTINGS_LABELS, 3);

  // Layout and draw UI
  layoutUI();
  drawUI();
}

void layoutUI() {
  // Calculate button row relative to grid
  int y = gridModel.getOffsetY() + gridModel.getGridHeight() + buttonMargin + 1;

  // Undo button position
  undoButton.setBounds(gridModel.getOffsetX(), y, undoButtonWidth, buttonHeight);
  undoButton.setIcon(UNDO_ICON, 24, 24);
  undoButton.setBgColor(ILI9341_DARKGREY);

  // Start button position and width
  int startButtonWidth = gridModel.getGridWidth() - undoButtonWidth - buttonMargin -
                     settingsButtonWidth - buttonMargin + 1;
  int startX = undoButton.x + undoButton.width + buttonMargin;
  startButton.setBounds(startX, y, startButtonWidth, buttonHeight);
  startButton.setBgColor(buttonIdleColor);
  startButton.setTextColor(buttonTextColor);
  startButton.setTextSize(2);
  startButton.setLabel("Start");

  // Settings button position
  int settingsX = startButton.x + startButton.width + buttonMargin;
  settingsButton.setBounds(settingsX, y, settingsButtonWidth, buttonHeight);
  settingsButton.setIcon(SETTINGS_ICON, 24, 24);
  settingsButton.setBgColor(ILI9341_DARKGREY);

  // Layout settings menu
  int menuWidth = gridModel.getGridWidth() * 0.85;
  int menuHeight = 225;
  int menuX = gridModel.getOffsetX() + (gridModel.getGridWidth() - menuWidth) / 2;
  int menuY = gridModel.getOffsetY() + (gridModel.getGridHeight() - menuHeight) / 2;
  settingsMenu.setPosition(menuX, menuY, menuWidth, menuHeight);
  settingsMenu.setColors(settingsMenuBackgroundColor, ILI9341_WHITE, settingsMenuTextColor);
  settingsMenu.setTextSize(settingsTextSize);
  settingsMenu.setArrowSize(settingsArrowWidth, settingsArrowHeight);
  settingsMenu.setArrowMargin(settingsArrowMarginX);
  settingsMenu.setOptionSpacing(70);
  settingsMenu.layout();
}

void drawUI() {
  // Draw all UI elements
  drawGridLines();
  drawGridCells();
  drawUndoButton();
  drawStartButton();
  drawSettingsButton();
}

void setBrightness() {
  // Ensure global brightness is in valid range
  displayBrightness = constrain(displayBrightness, 0, 100);

  // Convert percentage (0-100) to PWM value (0-255)
  int pwmOutput = map(displayBrightness, 0, 100, 0, 255);

  // Set PWM output
  analogWrite(TFT_LED, pwmOutput);
}

void drawGridLines() {
  // Draw horizontal grid lines
  for (int i = 0; i < gridModel.getNumRows() + 1; i++) {
    tft.drawLine(
      gridModel.getOffsetX(),
      i * gridModel.getCellSize() + gridModel.getOffsetY(),
      gridModel.getOffsetX() + gridModel.getGridWidth(),
      i * gridModel.getCellSize() + gridModel.getOffsetY(),
      gridModel.getGridColor());
  }

  // Draw verticalgrid lines
  for (int i = 0; i < gridModel.getNumCols() + 1; i++) {
    tft.drawLine(
      i * gridModel.getCellSize() + gridModel.getOffsetX(),
      gridModel.getOffsetY(),
      i * gridModel.getCellSize() + gridModel.getOffsetX(),
      gridModel.getOffsetY() + gridModel.getGridHeight(),
      gridModel.getGridColor());
  }
}


void drawGridCells() {
  // Default to drawing entire grid
  drawGridCells(0, gridModel.getNumRows() - 1, 0, gridModel.getNumCols() - 1);
}


void drawGridCells(int startRow, int endRow, int startCol, int endCol) {
  // Clamp values to grid bounds
  startRow = max(0, min(startRow, gridModel.getNumRows() - 1));
  endRow = max(0, min(endRow, gridModel.getNumRows() - 1));
  startCol = max(0, min(startCol, gridModel.getNumCols() - 1));
  endCol = max(0, min(endCol, gridModel.getNumCols() - 1));

  // Loop through row and col values
  for (int i = startRow; i <= endRow; i++) {
    for (int j = startCol; j <= endCol; j++) {

      // Determine cell color
      int color;

      // Check if cell is activated
      if (gridModel.isCellActivated(i, j)) {

        if (uiState == RUNNING || uiState == COMPLETE) {
          // Check if this cell is within the processed path range
          bool isProcessed = false;
          for (int p = 0; p <= gridModel.getCurrentPathIndex(); p++) {
            PathCell pathCell = gridModel.getPathCell(p);
            if (i == pathCell.row && j == pathCell.col) {
              isProcessed = true;
              break;
            }
          }
          
          if (isProcessed) {
            color = gridModel.getSelectableColor(); // Green for processed cells
          } else {
            color = gridModel.getSelectedColor(); // Black for unprocessed cells
          }
        } else {
          color = gridModel.getSelectedColor(); // Black for idle state
        }
      }
      // UI is in idle state and cell is selectable
      else if (uiState == IDLE && gridModel.isSelectable(i, j)) {
        color = gridModel.getSelectableColor();  // Green for selectable cells
      }
      // Default empty cell color
      else {
        color = gridModel.getEmptyColor();
      }

      // Draw cell rect
      tft.fillRect(
        j * gridModel.getCellSize() + gridModel.getOffsetX() + 1,
        i * gridModel.getCellSize() + gridModel.getOffsetY() + 1,
        gridModel.getCellSize() - 1,
        gridModel.getCellSize() - 1,
        color);

      // Loop through path point indexes
      for (int p = 0; p < gridModel.getPathLength(); p++) {

        // Get path cell using the existing method
        PathCell pathCell = gridModel.getPathCell(p);
        
        // Check if current cell is equal to path point
        if (i == pathCell.row && j == pathCell.col) {

          // If only one path point, define next point to draw up arrow
          if (gridModel.getPathLength() == 1) {
            drawCellDirection(pathCell.row, pathCell.col, pathCell.row - 1, pathCell.col);
          }
          // If last path point, use current point to draw circle
          else if (p == gridModel.getPathLength() - 1) {
            drawCellDirection(pathCell.row, pathCell.col, pathCell.row, pathCell.col);
          }
          // If any other path point, use next point in path
          else {
            PathCell nextPathCell = gridModel.getPathCell(p + 1);
            drawCellDirection(pathCell.row, pathCell.col, nextPathCell.row, nextPathCell.col);
          }
        }
      }
    }
  }
}

void drawCellDirection(int row, int col, int nextRow, int nextCol) {
  // Calculate cell center
  int centerX = col * gridModel.getCellSize() + gridModel.getOffsetX() + (gridModel.getCellSize() / 2);
  int centerY = row * gridModel.getCellSize() + gridModel.getOffsetY() + (gridModel.getCellSize() / 2);

  // If last cell in path draw circle (using existing method)
  PathCell lastPathCell = gridModel.getPathCell(gridModel.getPathLength() - 1);
  if (row == lastPathCell.row && col == lastPathCell.col) {
    int circleRadius = 3;
    tft.fillCircle(centerX, centerY, circleRadius, gridModel.getArrowColor());
    return;
  }

  // Calculate direction - reversed from before
  int dx = nextCol - col;  // Direction is next minus current
  int dy = nextRow - row;

  // Draw small filled triangle pointing in direction of movement
  if (dx > 0) {  // Moving right
    tft.fillTriangle(
      centerX - 3, centerY - 3,
      centerX - 3, centerY + 3,
      centerX + 3, centerY,
      gridModel.getArrowColor());
  } else if (dx < 0) {  // Moving left
    tft.fillTriangle(
      centerX + 3, centerY - 3,
      centerX + 3, centerY + 3,
      centerX - 3, centerY,
      gridModel.getArrowColor());
  } else if (dy > 0) {  // Moving down
    tft.fillTriangle(
      centerX - 3, centerY - 3,
      centerX + 3, centerY - 3,
      centerX, centerY + 3,
      gridModel.getArrowColor());
  } else if (dy < 0) {  // Moving up
    tft.fillTriangle(
      centerX - 3, centerY + 3,
      centerX + 3, centerY + 3,
      centerX, centerY - 3,
      gridModel.getArrowColor());
  }
}

void drawUndoButton() {
  // Button bounds are precomputed in layoutUI()
  undoButton.draw(tft);
}

void drawStartButton() {
  // Button bounds are precomputed in layoutUI()

  // Init color and text
  uint16_t currentColor;
  String buttonText;

  switch (uiState) {
    case COUNTING:
      {
        currentColor = buttonCountingColor;
        buttonText = String(countdownNumber);
        break;
      }

    case RUNNING:
      currentColor = buttonRunningColor;
      buttonText = "Stop";
      break;

    case COMPLETE:
      currentColor = buttonCompleteColor;
      buttonText = "Done!";
      break;

    case IDLE:
    default:
      currentColor = buttonIdleColor;
      buttonText = "Start";
      break;
  }

  startButton.setLabel(buttonText);
  startButton.setBgColor(currentColor);
  startButton.draw(tft);
}

void drawSettingsButton() {
// Draw button background
  settingsButton.draw(tft);
}

void drawSettingsMenu() {
  // Update option values with current settings
  settingsMenu.updateOptionValue(0, String(displayBrightness) + "%");
  settingsMenu.updateOptionValue(1, DRIVE_SPEED_LABELS[driveSpeed]);
  settingsMenu.updateOptionValue(2, DRIVE_DISTANCE_LABELS[driveDistance]);

  // Draw the menu
  settingsMenu.draw(tft);
}

void updateBrightnessDisplay() {
  settingsMenu.updateOptionValue(0, String(displayBrightness) + "%");
  settingsMenu.redrawOption(0, tft);
}

void updateDriveSpeedDisplay() {
  settingsMenu.updateOptionValue(1, DRIVE_SPEED_LABELS[driveSpeed]);
  settingsMenu.redrawOption(1, tft);
}

void updateDriveDistanceDisplay() {
  settingsMenu.updateOptionValue(2, DRIVE_DISTANCE_LABELS[driveDistance]);
  settingsMenu.redrawOption(2, tft);
}

bool isPointInRect(int x, int y, int rectX, int rectY, int rectWidth, int rectHeight) {
  return x >= rectX && x <= rectX + rectWidth && y >= rectY && y <= rectY + rectHeight;
}

bool isTouchInGrid(int x, int y) {
  // Subtract one to maintain previous < comparisons
  return isPointInRect(x, y, gridModel.getOffsetX(), gridModel.getOffsetY(), gridModel.getGridWidth() - 1, gridModel.getGridHeight() - 1);
}

// Function to calculate direction between two points
int getDirection(int startRow, int startCol, int endRow, int endCol) {
  if (endRow < startRow) return 0;  // Up
  if (endCol > startCol) return 1;  // Right
  if (endRow > startRow) return 2;  // Down
  if (endCol < startCol) return 3;  // Left
  return gridModel.getCurrentDirection();           // No change
}

// Function to calculate required turn
int calculateTurn(int currentDir, int targetDir) {
  int diff = targetDir - currentDir;

  // Normalize to -1 (left) or 1 (right)
  if (diff == 3) diff = -1;
  if (diff == -3) diff = 1;

  return diff;
}

// Function to print current cell information
void printCurrentCell(PathCell current, PathCell next) {
}

// Function to execute next movement
void executeMovement() {
  switch (driveState) {

    // Stopped driving state
    case STOPPED:
      {
        // Get target direction using the new method
        int targetDirection = gridModel.getNextDirection();

        // Check if aligned to target direction
        if (gridModel.getCurrentDirection() == targetDirection) {

          // Set driving flags
          driveState = DRIVING;

          // Here you would add actual motor control:
          // driveForward();

          Serial.println("Driving");
        }
        // Not aligned to target direction
        else {

          // Set turning flags
          driveState = TURNING;

          // Get turn direction
          int turn = calculateTurn(gridModel.getCurrentDirection(), targetDirection);

          // Here you would add actual motor control:
          // if (turn < 0) turnLeft();
          // if (turn > 0) turnRight();

          Serial.print("Turning ");
          Serial.println(turn < 0 ? "left" : "right");
        }

        // Set move start time
        moveStartTime = millis();

        break;
      }

    // Driving state
    case DRIVING:
      {
        // Check if driving movement is complete
        if (millis() - moveStartTime >= FORWARD_MOVE_TIME) {

          // Get current path cell for redrawing
          PathCell current = gridModel.getCurrentPathCell();
          drawGridCells(current.row, current.row, current.col, current.col);

          // Check if current path point is the last
          if (gridModel.isPathComplete()) {

            // Here you would stop motors:
            // stopMotors();
            Serial.println("Path complete");

            // Update UI and driving state
            uiState = COMPLETE;
            driveState = STOPPED;
            drawStartButton();
          }

          // Current path point is not the last
          else {

            // Reset timer for next movement
            moveStartTime = millis();

            // Get next direction using the new method
            int nextDirection = gridModel.getNextDirection();

            // Update current path point
            gridModel.setCurrentPathIndex(gridModel.getCurrentPathIndex() + 1);

            // If direction changes, stop for turn
            if (nextDirection != gridModel.getCurrentDirection()) {
              driveState = STOPPED;
              // Here you would stop motors:
              // stopMotors();
              Serial.println("Stopping for turn");
            }
            // If same direction, continue driving
            else {
              Serial.println("Continuing forward");
              // No need to modify motors, just keep driving
            }
          }
        }
        break;
      }

    // Turning state
    case TURNING:
      {
        // Check if turn is complete
        if (millis() - moveStartTime >= TURN_MOVE_TIME) {

          // Get target direction using the new method
          int targetDirection = gridModel.getNextDirection();

          // Update direction and start driving
          gridModel.setCurrentDirection(targetDirection);
          driveState = DRIVING;
          moveStartTime = millis();

          // Here you would update motor control:
          // stopTurning();
          // driveForward();

          Serial.println("Turn complete, starting forward movement");
        }
        break;
      }
  }
}

void loop() {

  // Get touch data
  TSPoint p = ts.getPoint();

  // Check if touch occured
  if (p.z > ts.pressureThreshhold) {

    // Calculate touch pixel coordinates
    int pixelX = map(p.x, TOUCH_MIN_X, TOUCH_MAX_X, 0, screenWidth);
    int pixelY = map(p.y, TOUCH_MIN_Y, TOUCH_MAX_Y, 0, screenHeight);
    Serial.println(pixelX);
    Serial.println(pixelY);
    Serial.println(" ");

    // Check if start button touched while not in settings state
    if (startButton.contains(pixelX, pixelY) && uiState != SETTINGS) {
      Serial.println("Start button touched");

      // Process touch based on current state
      switch (uiState) {

        // Idle state
        case IDLE:
          {
            // Set to counting state and initialze couting variables
            uiState = COUNTING;
            countdownStart = millis();
            countdownNumber = countdownDuration / 1000;
            break;
          }

        // Counting state
        case COUNTING:
          // Set to idle state
          uiState = IDLE;
          break;

        // Running state
        case RUNNING:
          // Set to idle state and stopped driving state
          uiState = IDLE;
          driveState = STOPPED;
          break;

        // Running state
        case COMPLETE:
          // Set to idle state
          uiState = IDLE;
          break;
      }

      // Redraw entire grid
      drawGridCells(0, gridModel.getNumRows() - 1, 0, gridModel.getNumCols() - 1);

      // Draw button
      drawStartButton();

      // Debounce delay
      delay(250);
    }

    // Check if undo button touched while in idle state
    else if (undoButton.contains(pixelX, pixelY) && uiState == IDLE) {
      Serial.println("Undo button touched");

      // Reset grid values and path to default cells
      gridModel.resetGridValues();
      gridModel.resetDefaultPath();

      // Redraw entire grid
      drawGridCells(0, gridModel.getNumRows() - 1, 0, gridModel.getNumCols() - 1);

      // Debounce delay
      delay(50);
    }

    // Check if settings button touched
    else if (settingsButton.contains(pixelX, pixelY)) {
      Serial.println("Settings button touched");

      // Check if in idle state
      if (uiState == IDLE) {

        // Change to settings state and show menu
        uiState = SETTINGS;
        drawSettingsMenu();
      }

      // Check if in settings state
      else if (uiState == SETTINGS) {

        // Change back to idle state and redraw main interface
        uiState = IDLE;
        drawUI();
      }

      // Debounce delay
      delay(250);
    }

    // Check if grid touched while in idle state
    else if (isTouchInGrid(pixelX, pixelY) && uiState == IDLE) {
      Serial.println("Grid button touched");

      // Calculated col/row of touched cell
      int gridCol = (pixelX - gridModel.getOffsetX()) / gridModel.getCellSize();
      int gridRow = (pixelY - gridModel.getOffsetY()) / gridModel.getCellSize();
      Serial.println(gridCol);
      Serial.println(gridRow);
      Serial.println(" ");

      // If cell is selectable, add to path and redraw surrounding cells
      if (gridModel.isSelectable(gridRow, gridCol)) {
        gridModel.pathAdd(gridRow, gridCol);
        drawGridCells(gridRow - 2, gridRow + 2, gridCol - 2, gridCol + 2);
      }
    }

    // Check if in settings state
    else if (uiState == SETTINGS) {
      // Get which option was touched
      int touchedOption = settingsMenu.getTouchedOption(pixelX, pixelY);
      
      if (touchedOption >= 0) {
        // Check for arrow touches on the specific option
        if (settingsMenu.isLeftArrowTouched(pixelX, pixelY, touchedOption)) {
          handleSettingsLeftArrow(touchedOption);
          delay(250);
        }
        else if (settingsMenu.isRightArrowTouched(pixelX, pixelY, touchedOption)) {
          handleSettingsRightArrow(touchedOption);
          delay(250);
        }
      }
    }

    // Debounce delay
    delay(50);
  }

  // Check if in coutning state
  if (uiState == COUNTING) {
    //
    if (millis() - countdownStart >= countdownDuration) {
      uiState = RUNNING;
      gridModel.setCurrentPathIndex(0);
      gridModel.setCurrentDirection(0);
      drawStartButton();
      //
    } else {
      // Calculate current countdown number
      int currentNumber = (countdownDuration / 1000) - ((millis() - countdownStart) / 1000);

      // Only draw if number has changed
      if (currentNumber != countdownNumber) {
        countdownNumber = currentNumber;
        drawStartButton();
      }
    }
  }

  // Check if in running state
  if (uiState == RUNNING) {
    // Execute bot movements
    executeMovement();
  }

  delay(50);
}

// New helper functions for settings handling
void handleSettingsLeftArrow(int optionIndex) {
  switch (optionIndex) {
    case 0: // Brightness
      displayBrightness = max(10, displayBrightness - 10);
      setBrightness();
      updateBrightnessDisplay();
      break;
    case 1: // Drive Speed
      if (driveSpeed > SPEED_SLOW) {
        driveSpeed = (DriveSpeed)(driveSpeed - 1);
        updateDriveSpeedDisplay();
      }
      break;
    case 2: // Drive Distance
      if (driveDistance > DISTANCE_COMPACT) {
        driveDistance = (DriveDistance)(driveDistance - 1);
        updateDriveDistanceDisplay();
      }
      break;
  }
}

void handleSettingsRightArrow(int optionIndex) {
  switch (optionIndex) {
    case 0: // Brightness
      displayBrightness = min(100, displayBrightness + 10);
      setBrightness();
      updateBrightnessDisplay();
      break;
    case 1: // Drive Speed
      if (driveSpeed < SPEED_FAST) {
        driveSpeed = (DriveSpeed)(driveSpeed + 1);
        updateDriveSpeedDisplay();
      }
      break;
    case 2: // Drive Distance
      if (driveDistance < DISTANCE_EXTENDED) {
        driveDistance = (DriveDistance)(driveDistance + 1);
        updateDriveDistanceDisplay();
      }
      break;
  }
}
