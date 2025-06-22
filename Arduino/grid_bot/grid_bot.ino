#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include "TouchScreen.h"
#include "icons.h"
#include "ui_elements.h"
#include "grid_model.h"
#include "settings_manager.h"
#include "state.h"

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

// Touch debounce delay
unsigned long lastTouchTime = 0;
const unsigned long touchDebounceDelay = 200;

void setup() {
  // Initialize serial communication
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

  // Initialize grid model
  initGridModel();

  // Layout and draw UI
  initUI();
  drawUI();
}

void initGridModel(){
  // Calculate grid size to fit in screen
  int availableHeight = screenHeight - BUTTON_HEIGHT - BUTTON_MARGIN - 1;
  int availableWidth = screenWidth - 1;
  int numRows = availableHeight / CELL_SIZE;
  int numCols = (availableWidth / CELL_SIZE) - ((availableWidth / CELL_SIZE) % 2 == 0 ? 1 : 0);
  
  // Initialize grid model
  gridModel.initGrid(numRows, numCols);
}

void initUI() {
  // Set UI grid size
  uiGrid.setSize(gridModel.getNumRows(), gridModel.getNumCols());

  // Set grid bounds
  int gridWidth = gridModel.getNumCols() * CELL_SIZE;
  int gridHeight = gridModel.getNumRows() * CELL_SIZE;
  int gridX = (screenWidth - (gridWidth + 1)) / 2;
  int totalGroupHeight = gridHeight + BUTTON_MARGIN + BUTTON_HEIGHT + 1;
  int gridY = (screenHeight - totalGroupHeight) / 2;
  uiGrid.setBounds(gridX, gridY, gridWidth, gridHeight);

  // Button row y position
  int y = gridY + gridHeight + BUTTON_MARGIN + 1;

  // Set undo button bounds and icon
  undoButton.setBounds(gridX, y, UNDO_BUTTON_WIDTH, BUTTON_HEIGHT);
  undoButton.setIcon(UNDO_ICON, 24, 24);

  // Set start button bounds and width
  int startButtonWidth = gridWidth - UNDO_BUTTON_WIDTH - BUTTON_MARGIN -
                      SETTINGS_BUTTON_WIDTH - BUTTON_MARGIN + 1;
  int startX = undoButton.x + undoButton.width + BUTTON_MARGIN;
  startButton.setBounds(startX, y, startButtonWidth, BUTTON_HEIGHT);
  
  // Update start button text and color
  updateStartButton();

  // Set settings button bounds and icon
  int settingsX = startButton.x + startButton.width + BUTTON_MARGIN;
  settingsButton.setBounds(settingsX, y, SETTINGS_BUTTON_WIDTH, BUTTON_HEIGHT);
  settingsButton.setIcon(SETTINGS_ICON, 24, 24);

  // Set settings menu options
  settingsMenu.setupOptions(SettingsManager::getSettingsLabels(), SettingsManager::getSettingsLabelsCount());
  settingsMenu.updateOptionValue(BRIGHTNESS, String(settingsManager.getDisplayBrightness()) + "%");
  settingsMenu.updateOptionValue(DRIVE_SPEED, settingsManager.getDriveSpeedLabel());
  settingsMenu.updateOptionValue(DRIVE_DISTANCE, settingsManager.getDriveDistanceLabel());

  // Set settings menu bounds
  int menuWidth = gridWidth * 0.85;
  int menuHeight = 225;
  int menuX = gridX + (gridWidth - menuWidth) / 2;
  int menuY = gridY + (gridHeight - menuHeight) / 2;
  settingsMenu.setPosition(menuX, menuY, menuWidth, menuHeight);

  // Set settings options positions
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
  // Get brightness percentage from settings manager
  int brightnessPercent = settingsManager.getDisplayBrightness();

  // Convert percentage (0-100) to PWM value (0-255)
  int pwmOutput = map(brightnessPercent, 0, 100, 0, 255);

  // Set PWM output
  analogWrite(TFT_LED, pwmOutput);
}

void updateStartButton() {
  // Button properties
  uint16_t currentColor;
  String buttonText;
  
  // Define properties based on current state
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
  
  // Set button text and color
  startButton.setLabel(buttonText);
  startButton.setBgColor(currentColor);
}

int calculateTurn(Direction currentDir, Direction targetDir) {
  // Calculate difference between current and target direction
  int diff = static_cast<int>(targetDir) - static_cast<int>(currentDir);

  // Normalize to -1 (left) or 1 (right)
  if (diff == 3) diff = -1;
  if (diff == -3) diff = 1;

  return diff;
}

// Function to print current cell information
void printCurrentCell(PathCell current, PathCell next) {
}

void executeMovement() {
  // Main movement state machine that handles bot navigation through the grid
  switch (driveState) {

    // Handle stopped state - determine next action (turn or drive)
    case STOPPED:
      {
        // Get the target direction from the current path point
        Direction targetDirection = gridModel.getNextDirection();

        // Check if bot is already aligned with target direction
        if (gridModel.getCurrentDirection() == targetDirection) {
          // Bot is aligned, transition to driving state
          driveState = DRIVING;
          // Here you would add actual motor control:
          // driveForward();
          Serial.println("Driving");
        }
        // Bot needs to turn to align with target direction
        else {
          // Transition to turning state
          driveState = TURNING;
          // Calculate which direction to turn (left or right)
          int turn = calculateTurn(gridModel.getCurrentDirection(), targetDirection);
          // Here you would add actual motor control:
          // if (turn < 0) turnLeft();
          // if (turn > 0) turnRight();
          Serial.print("Turning ");
          Serial.println(turn < 0 ? "left" : "right");
        }
        // Record the start time for movement timing
        moveStartTime = millis();
        break;
      }

    // Handle driving state - move forward for specified duration
    case DRIVING:
      {
        // Check if forward movement duration has elapsed
        if (millis() - moveStartTime >= FORWARD_MOVE_TIME) {
          // Get current path cell for visual update
          PathCell current = gridModel.getCurrentPathCell();
          uiGrid.drawGridCells(tft, gridModel, uiState, current.row, current.row, current.col, current.col);

          // Check if we've reached the end of the path
          if (gridModel.isPathComplete()) {
            // Path is complete, stop all movement
            // Here you would stop motors:
            // stopMotors();
            Serial.println("Path complete");
            // Update UI to show completion state
            uiState = COMPLETE;
            driveState = STOPPED;
            updateStartButton();
            startButton.draw(tft);
          }
          // Path is not complete, prepare for next movement
          else {
            // Reset timer for next movement segment
            moveStartTime = millis();
            // Get direction for next path point
            Direction nextDirection = gridModel.getNextDirection();
            // Advance to next path point
            gridModel.setCurrentPathIndex(gridModel.getCurrentPathIndex() + 1);

            // Check if direction changes at next point
            if (nextDirection != gridModel.getCurrentDirection()) {
              // Direction changes, stop for turn
              driveState = STOPPED;
              // Here you would stop motors:
              // stopMotors();
              Serial.println("Stopping for turn");
            }
            // Direction remains same, continue driving
            else {
              Serial.println("Continuing forward");
              // No need to modify motors, just keep driving
            }
          }
        }
        break;
      }

    // Handle turning state - execute 90-degree turn
    case TURNING:
      {
        // Check if turn duration has elapsed
        if (millis() - moveStartTime >= TURN_MOVE_TIME) {
          // Get target direction for after turn
          Direction targetDirection = gridModel.getNextDirection();
          // Update bot's current direction and transition to driving
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

void handleState() {
  // Handle countdown logic
  if (uiState == COUNTING) {
    handleCountdown();
  } 
  // Handle movement execution
  else if (uiState == RUNNING) {
    executeMovement();
  }
}

void handleCountdown() {
  // Check if countdown duration has elapsed
  if (millis() - countdownStart >= countdownDuration) {

    // Change state to running
    uiState = RUNNING;

    // Initialize path execution
    gridModel.setCurrentPathIndex(0);
    gridModel.setCurrentDirection(UP);

    // Update and draw start button
    updateStartButton();
    startButton.draw(tft);
  } else {

    // Calculate current countdown number and update display if changed
    int currentNumber = (countdownDuration / 1000) - ((millis() - countdownStart) / 1000);
    
    // Update and draw start button if countdown number has changed
    if (currentNumber != countdownNumber) {
      countdownNumber = currentNumber;
      updateStartButton();
      startButton.draw(tft);
    }
  }
}

void handleTouch(TSPoint p) {
  // End processing if touch occurred within debounce period
  unsigned long now = millis();
  if (now - lastTouchTime < touchDebounceDelay) {
    return;
  }

  // Update last touch time
  lastTouchTime = now;

  // Convert touch coordinates from raw values to screen pixel coordinates
  int pixelX = map(p.x, TOUCH_MIN_X, TOUCH_MAX_X, 0, screenWidth);
  int pixelY = map(p.y, TOUCH_MIN_Y, TOUCH_MAX_Y, 0, screenHeight);

  // --- Touch Event Dispatching ---
  // 
  // Handle start button interactions outside of settings state
  if (startButton.contains(pixelX, pixelY) && uiState != SETTINGS) {
    onTouchStartButton();
  }
  // Handle undo button interactions in idle state
  else if (undoButton.contains(pixelX, pixelY) && uiState == IDLE) {
    onTouchUndoButton();
  }
  // Handle settings button interactions
  else if (settingsButton.contains(pixelX, pixelY)) {
    onTouchSettingsButton();  
  }
  // Handle grid interactions in idle state
  else if (uiGrid.contains(pixelX, pixelY) && uiState == IDLE) {
    onTouchGrid(pixelX, pixelY);
  }
  // Handle settings menu interactions in settings state
  else if (settingsMenu.contains(pixelX, pixelY) && uiState == SETTINGS) {
    onTouchSettingsMenu(pixelX, pixelY);
  }
}

void onTouchStartButton() {
  Serial.println("Start button toucheddd");

  // Handle start button touch in different states
  switch (uiState) {
    case IDLE:
      // Change state to counting set countdown values
      uiState = COUNTING;
      countdownStart = millis();
      countdownNumber = countdownDuration / 1000;
      break;

    case COUNTING:
      // Fallthrough to next case

    case RUNNING:
      // Fallthrough to next case

    case COMPLETE:
      // Change state to idle and reset drive state
      uiState = IDLE;
      driveState = STOPPED;
      break;
  }

  // Redraw grid cells
  uiGrid.drawGridCells(tft, gridModel, uiState);

  // Update and draw start button
  updateStartButton();
  startButton.draw(tft);
}

void onTouchUndoButton() {
  Serial.println("Undo button touched");

  // Reset grid values and default path
  gridModel.resetGridValues();
  gridModel.resetDefaultPath();

  // Redraw grid cells
  uiGrid.drawGridCells(tft, gridModel, uiState);
}

void onTouchSettingsButton() {
  Serial.println("Settings button touched");

  // In idle state, change to settings state and draw settings menu
  if (uiState == IDLE) {
    uiState = SETTINGS;
    settingsMenu.draw(tft);
  } 
  // In settings state, change to idle state and draw UI
  else if (uiState == SETTINGS) {
    uiState = IDLE;
    drawUI();
  }
}

void onTouchGrid(int pixelX, int pixelY) {
  Serial.println("Grid touched");

  // Calculate grid column and row
  int gridCol = (pixelX - uiGrid.x) / CELL_SIZE;
  int gridRow = (pixelY - uiGrid.y) / CELL_SIZE;

  // Check if cell is selectable
  if (gridModel.isSelectable(gridRow, gridCol)) {
    // Add path cell to model
    gridModel.pathAdd(gridRow, gridCol);

    // Redraw grid cells
    uiGrid.drawGridCells(tft, gridModel, uiState, gridRow - 2, gridRow + 2, gridCol - 2, gridCol + 2);
  }
}

void onTouchSettingsMenu(int pixelX, int pixelY) {
  // Determine which option was touched
  int optionIndex = settingsMenu.optionIndexContaining(pixelX, pixelY);
  
  // Handle arrow interactions
  if (optionIndex >= 0) {
    if (settingsMenu.leftArrowContains(pixelX, pixelY, optionIndex)) {
      handleSettingsArrow(static_cast<SettingOption>(optionIndex), -1);
    } else if (settingsMenu.rightArrowContains(pixelX, pixelY, optionIndex)) {
      handleSettingsArrow(static_cast<SettingOption>(optionIndex), 1);
    }
  }
}

void handleSettingsArrow(SettingOption option, int direction) {
  // Update the setting option value in the manager
  settingsManager.adjustSetting(option, direction);

  // Update value in settings menu and redraw
  switch (option) {
    case BRIGHTNESS:
      setBrightness();
      settingsMenu.updateOptionValue(BRIGHTNESS, String(settingsManager.getDisplayBrightness()) + "%");
      settingsMenu.redrawOption(BRIGHTNESS, tft);
      break;
    case DRIVE_SPEED:
      settingsMenu.updateOptionValue(DRIVE_SPEED, settingsManager.getDriveSpeedLabel());
      settingsMenu.redrawOption(DRIVE_SPEED, tft);
      break;
    case DRIVE_DISTANCE:
      settingsMenu.updateOptionValue(DRIVE_DISTANCE, settingsManager.getDriveDistanceLabel());
      settingsMenu.redrawOption(DRIVE_DISTANCE, tft);
      break;
  }
}

void loop() {
  // Handle countdown and movement states
  handleState();

  // Check for and process discrete user touch events
  TSPoint p = ts.getPoint();
  if (p.z > ts.pressureThreshhold) {
    handleTouch(p);
  }

  // Loop delay
  delay(50);
}