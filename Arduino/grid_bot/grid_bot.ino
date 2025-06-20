#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include "TouchScreen.h"
#include "icons.h"
#include "ui_elements.h"
#include "grid_model.h"
#include "settings_manager.h"

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

// Screen dimensions
int screenWidth;
int screenHeight;

// Grid model instance
GridModel gridModel;

// Settings manager instance
SettingsManager settingsManager;

// UI elements
UIIconButton undoButton;
UITextButton startButton;
UIIconButton settingsButton;
UISettingsMenu settingsMenu;
UIGrid uiGrid;

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
  tft.fillScreen(BACKGROUND_COLOR);

  // Dynamically get screen dimensions
  screenWidth = tft.width();
  screenHeight = tft.height();

  // Calculate grid size
  int availableHeight = screenHeight - BUTTON_HEIGHT - BUTTON_MARGIN - 1;
  int availableWidth = screenWidth - 1;
  int numRows = availableHeight / CELL_SIZE;
  int numCols = (availableWidth / CELL_SIZE) - ((availableWidth / CELL_SIZE) % 2 == 0 ? 1 : 0);
  
  // Initialize grid model
  gridModel.initGrid(numRows, numCols);
  uiGrid.setSize(numRows, numCols);

  // Initialize settings menu
  settingsMenu.setupOptions(SettingsManager::getSettingsLabels(), SettingsManager::getSettingsLabelsCount());
  settingsMenu.updateOptionValue(0, String(settingsManager.getDisplayBrightness()) + "%");
  settingsMenu.updateOptionValue(1, settingsManager.getDriveSpeedLabel());
  settingsMenu.updateOptionValue(2, settingsManager.getDriveDistanceLabel());

  // Layout and draw UI
  layoutUI();
  drawUI();
}

void layoutUI() {
    // Calculate grid bounds
    int gridWidth = gridModel.getNumCols() * CELL_SIZE;
    int gridHeight = gridModel.getNumRows() * CELL_SIZE;
    int gridX = (screenWidth - (gridWidth + 1)) / 2;

    // Center the grid + button row as a group vertically
    int totalGroupHeight = gridHeight + BUTTON_MARGIN + BUTTON_HEIGHT + 1;
    int gridY = (screenHeight - totalGroupHeight) / 2;
    uiGrid.setBounds(gridX, gridY, gridWidth, gridHeight);

    // Button row y position
    int y = gridY + gridHeight + BUTTON_MARGIN + 1;

    // Undo button position
    undoButton.setBounds(gridX, y, UNDO_BUTTON_WIDTH, BUTTON_HEIGHT);
    undoButton.setIcon(UNDO_ICON, 24, 24);

    // Start button position and width
    int startButtonWidth = gridWidth - UNDO_BUTTON_WIDTH - BUTTON_MARGIN -
                       SETTINGS_BUTTON_WIDTH - BUTTON_MARGIN + 1;
    int startX = undoButton.x + undoButton.width + BUTTON_MARGIN;
    startButton.setBounds(startX, y, startButtonWidth, BUTTON_HEIGHT);
    startButton.setBgColor(BUTTON_IDLE_COLOR);
    startButton.setLabel("Start");

    // Settings button position
    int settingsX = startButton.x + startButton.width + BUTTON_MARGIN;
    settingsButton.setBounds(settingsX, y, SETTINGS_BUTTON_WIDTH, BUTTON_HEIGHT);
    settingsButton.setIcon(SETTINGS_ICON, 24, 24);

    // Layout settings menu
    int menuWidth = gridWidth * 0.85;
    int menuHeight = 225;
    int menuX = gridX + (gridWidth - menuWidth) / 2;
    int menuY = gridY + (gridHeight - menuHeight) / 2;
    settingsMenu.setPosition(menuX, menuY, menuWidth, menuHeight);
    settingsMenu.layout();
}

void drawUI() {
  // Draw all UI elements
  uiGrid.drawGridLines(tft);
  uiGrid.drawGridCells(tft, gridModel, uiState);
  undoButton.draw(tft);
  startButton.draw(tft);
  settingsButton.draw(tft);
}

void setBrightness() {
  // Get PWM value from settings manager
  int pwmOutput = settingsManager.getBrightnessPWM();

  // Set PWM output
  analogWrite(TFT_LED, pwmOutput);
}

void updateStartButton() {
  // Update start button properties based on current state
  uint16_t currentColor;
  String buttonText;
  
  switch (uiState) {
    case COUNTING:
      currentColor = BUTTON_COUNTING_COLOR;
      buttonText = String(countdownNumber);
      break;
    case RUNNING:
      currentColor = BUTTON_RUNNING_COLOR;
      buttonText = "Stop";
      break;
    case COMPLETE:
      currentColor = BUTTON_COMPLETE_COLOR;
      buttonText = "Done!";
      break;
    case IDLE:
    default:
      currentColor = BUTTON_IDLE_COLOR;
      buttonText = "Start";
      break;
  }
  
  startButton.setLabel(buttonText);
  startButton.setBgColor(currentColor);
}

void updateBrightnessDisplay() {
  settingsMenu.updateOptionValue(0, String(settingsManager.getDisplayBrightness()) + "%");
  settingsMenu.redrawOption(0, tft);
}

void updateDriveSpeedDisplay() {
  settingsMenu.updateOptionValue(1, settingsManager.getDriveSpeedLabel());
  settingsMenu.redrawOption(1, tft);
}

void updateDriveDistanceDisplay() {
  settingsMenu.updateOptionValue(2, settingsManager.getDriveDistanceLabel());
  settingsMenu.redrawOption(2, tft);
}

// Function to calculate direction between two points
Direction getDirection(int startRow, int startCol, int endRow, int endCol) {
  if (endRow < startRow) return UP;    // Up
  if (endCol > startCol) return RIGHT; // Right
  if (endRow > startRow) return DOWN;  // Down
  if (endCol < startCol) return LEFT;  // Left
  return gridModel.getCurrentDirection();           // No change
}

// Function to calculate required turn

int calculateTurn(Direction currentDir, Direction targetDir) {
  int diff = static_cast<int>(targetDir) - static_cast<int>(currentDir);

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
        Direction targetDirection = gridModel.getNextDirection();

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
          uiGrid.drawGridCells(tft, gridModel, uiState, current.row, current.row, current.col, current.col);

          // Check if current path point is the last
          if (gridModel.isPathComplete()) {

            // Here you would stop motors:
            // stopMotors();
            Serial.println("Path complete");

            // Update UI and driving state
            uiState = COMPLETE;
            driveState = STOPPED;
            updateStartButton();
            startButton.draw(tft);
          }

          // Current path point is not the last
          else {

            // Reset timer for next movement
            moveStartTime = millis();

            // Get next direction using the new method
            Direction nextDirection = gridModel.getNextDirection();

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
          Direction targetDirection = gridModel.getNextDirection();

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
      uiGrid.drawGridCells(tft, gridModel, uiState, 0, gridModel.getNumRows() - 1, 0, gridModel.getNumCols() - 1);

      // Draw button
      updateStartButton();
      startButton.draw(tft);

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
      uiGrid.drawGridCells(tft, gridModel, uiState, 0, gridModel.getNumRows() - 1, 0, gridModel.getNumCols() - 1);

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
        settingsMenu.draw(tft);
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
    else if (uiGrid.contains(pixelX, pixelY) && uiState == IDLE) {
      Serial.println("Grid button touched");

      // Calculated col/row of touched cell
      int gridCol = (pixelX - uiGrid.x) / CELL_SIZE;
      int gridRow = (pixelY - uiGrid.y) / CELL_SIZE;
      Serial.println(gridCol);
      Serial.println(gridRow);
      Serial.println(" ");

      // If cell is selectable, add to path and redraw surrounding cells
      if (gridModel.isSelectable(gridRow, gridCol)) {
        gridModel.pathAdd(gridRow, gridCol);
        uiGrid.drawGridCells(tft, gridModel, uiState, gridRow - 2, gridRow + 2, gridCol - 2, gridCol + 2);
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
      gridModel.setCurrentDirection(UP);
      updateStartButton();
      startButton.draw(tft);
      //
    } else {
      // Calculate current countdown number
      int currentNumber = (countdownDuration / 1000) - ((millis() - countdownStart) / 1000);

      // Only draw if number has changed
      if (currentNumber != countdownNumber) {
        countdownNumber = currentNumber;
        updateStartButton();
        startButton.draw(tft);
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
      settingsManager.adjustBrightness(-10);
      setBrightness();
      updateBrightnessDisplay();
      break;
    case 1: // Drive Speed
      settingsManager.decreaseDriveSpeed();
      updateDriveSpeedDisplay();
      break;
    case 2: // Drive Distance
      settingsManager.decreaseDriveDistance();
      updateDriveDistanceDisplay();
      break;
  }
}

void handleSettingsRightArrow(int optionIndex) {
  switch (optionIndex) {
    case 0: // Brightness
      settingsManager.adjustBrightness(10);
      setBrightness();
      updateBrightnessDisplay();
      break;
    case 1: // Drive Speed
      settingsManager.increaseDriveSpeed();
      updateDriveSpeedDisplay();
      break;
    case 2: // Drive Distance
      settingsManager.increaseDriveDistance();
      updateDriveDistanceDisplay();
      break;
  }
}
