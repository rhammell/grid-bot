#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include "TouchScreen.h"
#include "icons.h"
#include "ui_elements.h"

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

// Display brightness percent
int displayBrightness = 60;

// Screen dimensions
int screenWidth;
int screenHeight;

// Grid cell size
int cellSize = 30;

// Grid dimensions and offsets
int numRows;
int numCols;
int gridWidth;
int gridHeight;
int offsetX;
int offsetY;

// Grid display colors
uint16_t backgroundColor = ILI9341_BLACK;
uint16_t selectedColor = ILI9341_BLACK;
uint16_t emptyColor = ILI9341_WHITE;
uint16_t gridColor = ILI9341_DARKGREY;
uint16_t selectableColor = tft.color565(75, 225, 75);
uint16_t arrowColor = ILI9341_WHITE;

// Grid values array
bool** gridVals;

// Path cell structure
struct PathCell {
  int row;
  int col;
};

// Initi path cell array and length
PathCell* path;
int pathLength = 0;

// Button dimension values
int buttonHeight = 36;
int buttonMargin = 2;

// Undo button position params
int undoButtonWidth = 36;

// Settings button position params
int settingsButtonWidth = 36;

// Button display colors
int buttonIdleColor = tft.color565(75, 75, 225);
int buttonCountingColor = tft.color565(240, 124, 36);
int buttonRunningColor = tft.color565(255, 75, 75);
int buttonCompleteColor = tft.color565(75, 255, 75);
int buttonTextColor = ILI9341_WHITE;

// UI elements
UIIconButton undoButton;
UITextButton startButton;
UIIconButton settingsButton;
UIArrow brightnessLeftArrow;
UIArrow brightnessRightArrow;
UIArrow distanceLeftArrow;
UIArrow distanceRightArrow;
UIArrow speedLeftArrow;
UIArrow speedRightArrow;

// Settings menu position params
int settingsMenuWidth;
int settingsMenuHeight;
int settingsMenuX;
int settingsMenuY;

// Font metrics
const int FONT_CHAR_WIDTH = 6;
const int FONT_CHAR_HEIGHT = 8;

// Settings menu text params
int settingsTextSize = 2;

// Settings menu arrow params
int settingsArrowWidth = 38;
int settingsArrowHeight = 30;
int settingsArrowMarginX = 2;

// Replace individual parameters with class instances
UISettingsValue brightnessValue;
UISettingsValue distanceValue;
UISettingsValue speedValue;

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

// Color scheme for settings menu
uint16_t settingsMenuBackgroundColor = ILI9341_DARKGREY;
uint16_t settingsMenuTextColor = ILI9341_WHITE;
uint16_t settingsMenuButtonColor = tft.color565(100, 100, 100);

// Movement timing constants
const unsigned long FORWARD_MOVE_TIME = 750;  // Time to move forward one cell
const unsigned long TURN_MOVE_TIME = 500;     // Time to execute a 90-degree turn

// Current movement tracking
int currentPathIndex = 0;     // Current position in path array
int currentDirection = 0;      // 0=up, 1=right, 2=down, 3=left
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
  tft.fillScreen(backgroundColor);

  // Dynamically get screen dimensions
  screenWidth = tft.width();
  screenHeight = tft.height();

  // Initialize grid values
  initGrid();

  // Initialize path array with default points
  initPath();

  // Layout and draw UI
  layoutUI();
  drawUI();
}

void setBrightness() {
  // Ensure global brightness is in valid range
  displayBrightness = constrain(displayBrightness, 0, 100);

  // Convert percentage (0-100) to PWM value (0-255)
  int pwmOutput = map(displayBrightness, 0, 100, 0, 255);

  // Set PWM output
  analogWrite(TFT_LED, pwmOutput);
}

void initGrid() {

  // Calculate grid size based on available screen space
  int availableHeight = screenHeight - buttonHeight - 1 - buttonMargin;
  numRows = (availableHeight - 1) / cellSize;
  numCols = ((screenWidth - 1) / cellSize) -
            (((screenWidth - 1) / cellSize) % 2 == 0 ? 1 : 0);

  // Calculate grid dimensions and offsets
  gridWidth = cellSize * numCols;
  gridHeight = cellSize * numRows;
  offsetX = (screenWidth - gridWidth - 1) / 2;
  offsetY =
    (screenHeight - gridHeight - 1 - buttonHeight - buttonMargin) / 2;

  // Initialize 2D grid of cell values
  gridVals = new bool*[numRows];
  for (int i = 0; i < numRows; i++) {
    gridVals[i] = new bool[numCols];
  }

  // Reset all grid values to 0
  resetGridValues();
}

void resetGridValues() {
  for (int i = 0; i < numRows; i++) {
    for (int j = 0; j < numCols; j++) {
      gridVals[i][j] = false;
    }
  }
}

void initPath() {
  // Initialize array of path points
  path = new PathCell[numRows * numCols];

  // Set two initial default path points
  resetDefaultPath();
}

void resetDefaultPath() {
  // Set path length to zero
  pathLength = 0;

  // Add to points at center bottom of grid
  int startRow = numRows - 1;
  int startCol = numCols / 2;
  pathAdd(startRow, startCol);
  pathAdd(startRow - 1, startCol);
}

void pathAdd(int row, int col) {
  // Add coordinate to the list of path points
  path[pathLength] = { row, col };
  pathLength += 1;

  // Update the grid value of point
  gridVals[row][col] = true;
}

bool isSelectable(int row, int col) {
  // If cell is already selected, it's not selectable
  if (gridVals[row][col]) {
    return false;
  }

  // Get the last point in the path
PathCell lastPoint = path[pathLength - 1];

  // Check if cell is adjacent (not diagonal) to last point
  bool isAdjacent = (
    // Check horizontal adjacency
    (row == lastPoint.row && abs(col - lastPoint.col) == 1) ||
    // Check vertical adjacency
    (col == lastPoint.col && abs(row - lastPoint.row) == 1));

  return isAdjacent;
}

void layoutUI() {
  // Calculate button row relative to grid
  int y = offsetY + gridHeight + buttonMargin + 1;

  // Undo button position
  undoButton.setBounds(offsetX, y, undoButtonWidth, buttonHeight);
  undoButton.setIcon(UNDO_ICON, 24, 24);
  undoButton.setBgColor(ILI9341_DARKGREY);

  // Start button position and width
  int startButtonWidth = gridWidth - undoButtonWidth - buttonMargin -
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
}

void drawUI() {
  // Draw all UI elements
  drawGridLines();
  drawGridCells(0, numRows - 1, 0, numCols - 1);
  drawUndoButton();
  drawStartButton();
  drawSettingsButton();
}

void drawGridLines() {
  // Draw horizontal grid lines
  for (int i = 0; i < numRows + 1; i++) {
    tft.drawLine(
      offsetX,
      i * cellSize + offsetY,
      offsetX + gridWidth,
      i * cellSize + offsetY,
      gridColor);
  }

  // Draw verticalgrid lines
  for (int i = 0; i < numCols + 1; i++) {
    tft.drawLine(
      i * cellSize + offsetX,
      offsetY,
      i * cellSize + offsetX,
      offsetY + gridHeight,
      gridColor);
  }
}


void drawGridCells(int startRow, int endRow, int startCol, int endCol) {
  // Clamp values to grid bounds
  startRow = max(0, min(startRow, numRows - 1));
  endRow = max(0, min(endRow, numRows - 1));
  startCol = max(0, min(startCol, numCols - 1));
  endCol = max(0, min(endCol, numCols - 1));

  // Loop through row and col values
  for (int i = startRow; i <= endRow; i++) {
    for (int j = startCol; j <= endCol; j++) {
      Serial.println("looping");

      // Determine cell color
      int color;

      // Check if cell is activated
      if (gridVals[i][j]) {

        if (uiState == RUNNING || uiState == COMPLETE) {
          for (int p = 0; p <= currentPathIndex; p++) {
            if (i == path[p].row && j == path[p].col) {
              color = selectableColor;
              break;
            }
          }
        } else {
          color = selectedColor;
        }
      }
      // UI is
      else if (uiState == IDLE && isSelectable(i, j)) {
        color = selectableColor;  // Green for selectable cells
      }
      //
      else {
        color = emptyColor;
      }

      // Draw cell rect
      tft.fillRect(
        j * cellSize + offsetX + 1,
        i * cellSize + offsetY + 1,
        cellSize - 1,
        cellSize - 1,
        color);

      // Loop through path point indexes
      for (int p = 0; p < pathLength; p++) {

        // Check if current cell is equal to path point
        if (i == path[p].row && j == path[p].col) {

          // Current and next path point
          PathCell currentPoint = path[p];
          PathCell nextPoint;

          // If only one path point, define next point to draw up arrow
          if (pathLength == 1) {
            nextPoint = { currentPoint.row - 1, currentPoint.col };
          }
          // If last path point, use current point to draw circle
          else if (p == pathLength - 1) {
            nextPoint = currentPoint;
          }
          // If any other path point, use next point in path
          else {
            nextPoint = path[p + 1];
          }

          // Draw cell arrow using current and next
          drawCellDirection(currentPoint.row, currentPoint.col, nextPoint.row, nextPoint.col);
        }
      }
    }
  }
}

void drawCellDirection(int row, int col, int nextRow, int nextCol) {
  // Calculate cell center
  int centerX = col * cellSize + offsetX + (cellSize / 2);
  int centerY = row * cellSize + offsetY + (cellSize / 2);

  // If last cell in path draw circle
  if (row == path[pathLength - 1].row && col == path[pathLength - 1].col) {
    int circleRadius = 3;
    tft.fillCircle(centerX, centerY, circleRadius, arrowColor);
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
      arrowColor);
  } else if (dx < 0) {  // Moving left
    tft.fillTriangle(
      centerX + 3, centerY - 3,
      centerX + 3, centerY + 3,
      centerX - 3, centerY,
      arrowColor);
  } else if (dy > 0) {  // Moving down
    tft.fillTriangle(
      centerX - 3, centerY - 3,
      centerX + 3, centerY - 3,
      centerX, centerY + 3,
      arrowColor);
  } else if (dy < 0) {  // Moving up
    tft.fillTriangle(
      centerX - 3, centerY + 3,
      centerX + 3, centerY + 3,
      centerX, centerY - 3,
      arrowColor);
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
  // Button bounds are precomputed in layoutUI()

  // Draw button background
  settingsButton.draw(tft);
}

void drawSettingsMenu() {
  // Calculate menu dimensions and position
  settingsMenuWidth = gridWidth * 0.85;
  settingsMenuHeight = 225;
  settingsMenuX = offsetX + (gridWidth - settingsMenuWidth) / 2;
  settingsMenuY = offsetY + (gridHeight - settingsMenuHeight) / 2;

  // Calculate character width based on text size
  int settingsCharWidth = FONT_CHAR_WIDTH * settingsTextSize;
  int centeringOffset = settingsTextSize;
  int fontPadding = settingsTextSize;
  
  // Draw menu background
  tft.fillRect(settingsMenuX, settingsMenuY, settingsMenuWidth, settingsMenuHeight, settingsMenuBackgroundColor);
  tft.drawRect(settingsMenuX, settingsMenuY, settingsMenuWidth, settingsMenuHeight, ILI9341_WHITE);

  // Set text size and color
  tft.setTextSize(settingsTextSize);
  tft.setTextColor(settingsMenuTextColor);

  // Draw brightness setting label
  String brightnessLabel = "Brightness";
  int brightnessLabelX = settingsMenuX + (settingsMenuWidth - (brightnessLabel.length() * settingsCharWidth - fontPadding)) / 2;
  int brightnessLabelY = settingsMenuY + 20;
  tft.setCursor(brightnessLabelX, brightnessLabelY);
  tft.print(brightnessLabel);

  // Calculate brightness arrow positions
  int brightnessArrowX1 = settingsMenuX + settingsArrowMarginX;
  int brightnessArrowX2 = settingsMenuX + settingsMenuWidth - settingsArrowMarginX - settingsArrowWidth;
  int brightnessArrowY = brightnessLabelY + 20;
  brightnessLeftArrow.setBounds(brightnessArrowX1, brightnessArrowY, settingsArrowWidth, settingsArrowHeight);
  brightnessLeftArrow.setDirection(ARROW_LEFT);
  brightnessLeftArrow.setTriangleSize(12, 18);  // Explicit triangle size
  brightnessLeftArrow.setBgColor(settingsMenuBackgroundColor);
  brightnessLeftArrow.setTriangleColor(ILI9341_WHITE);
  brightnessLeftArrow.draw(tft);
  brightnessRightArrow.setBounds(brightnessArrowX2, brightnessArrowY, settingsArrowWidth, settingsArrowHeight);
  brightnessRightArrow.setDirection(ARROW_RIGHT);
  brightnessRightArrow.setTriangleSize(12, 18);  // Explicit triangle size
  brightnessRightArrow.setBgColor(settingsMenuBackgroundColor);
  brightnessRightArrow.setTriangleColor(ILI9341_WHITE);
  brightnessRightArrow.draw(tft);

  // Set up brightness value display
  int maxBrightnessChars = 4;
  int maxBrightnessWidth = maxBrightnessChars * settingsCharWidth - fontPadding;
  brightnessValue.setPosition(
      settingsMenuX + (settingsMenuWidth - maxBrightnessWidth) / 2,
      brightnessArrowY + (settingsArrowHeight - settingsTextSize * FONT_CHAR_HEIGHT) / 2,
      maxBrightnessWidth
  );
  brightnessValue.setColors(settingsMenuBackgroundColor, settingsMenuTextColor);
  brightnessValue.setTextSize(settingsTextSize);
  brightnessValue.setFontMetrics(FONT_CHAR_WIDTH, FONT_CHAR_HEIGHT);
  brightnessValue.setValue(String(displayBrightness) + "%");
  brightnessValue.draw(tft);

  // Draw speed setting label
  String speedLabel = "Drive Speed";
  int speedLabelX = settingsMenuX + (settingsMenuWidth - (speedLabel.length() * settingsCharWidth - fontPadding)) / 2;
  int speedLabelY = brightnessLabelY + 70;
  tft.setCursor(speedLabelX, speedLabelY);
  tft.print(speedLabel);

  // Calculate size arrow positions
  int speedArrowX1 = settingsMenuX + settingsArrowMarginX;
  int speedArrowX2 = settingsMenuX + settingsMenuWidth - settingsArrowMarginX - settingsArrowWidth;
  int speedArrowY = speedLabelY + 20;
  speedLeftArrow.setBounds(speedArrowX1, speedArrowY, settingsArrowWidth, settingsArrowHeight);
  speedLeftArrow.setDirection(ARROW_LEFT);
  speedLeftArrow.setTriangleSize(12, 18);  // Explicit triangle size
  speedLeftArrow.setBgColor(settingsMenuBackgroundColor);
  speedLeftArrow.setTriangleColor(ILI9341_WHITE);
  speedLeftArrow.draw(tft);
  speedRightArrow.setBounds(speedArrowX2, speedArrowY, settingsArrowWidth, settingsArrowHeight);
  speedRightArrow.setDirection(ARROW_RIGHT);
  speedRightArrow.setTriangleSize(12, 18);  // Explicit triangle size
  speedRightArrow.setBgColor(settingsMenuBackgroundColor);
  speedRightArrow.setTriangleColor(ILI9341_WHITE);
  speedRightArrow.draw(tft);

  // Calculate and store speed value position
  int maxSpeedChars = 8;  // "Standard" is the longest label
  int maxSpeedWidth = maxSpeedChars * settingsCharWidth - fontPadding;
  speedValue.setPosition(
      settingsMenuX + (settingsMenuWidth - maxSpeedWidth) / 2,
      speedArrowY + (settingsArrowHeight - settingsTextSize * FONT_CHAR_HEIGHT) / 2,
      maxSpeedWidth
  );
  speedValue.setColors(settingsMenuBackgroundColor, settingsMenuTextColor);
  speedValue.setTextSize(settingsTextSize);
  speedValue.setFontMetrics(FONT_CHAR_WIDTH, FONT_CHAR_HEIGHT);
  speedValue.setValue(DRIVE_SPEED_LABELS[driveSpeed]);
  speedValue.draw(tft);

  // Draw grid size setting label
  String distanceLabel = "Drive Distance";
  int distanceLabelX = settingsMenuX + (settingsMenuWidth - (distanceLabel.length() * settingsCharWidth - fontPadding)) / 2;
  int distanceLabelY = speedLabelY + 70;
  tft.setCursor(distanceLabelX, distanceLabelY);
  tft.print(distanceLabel);

  // Calculate size arrow positions
  int distanceArrowX1 = settingsMenuX + settingsArrowMarginX;
  int distanceArrowX2 = settingsMenuX + settingsMenuWidth - settingsArrowMarginX - settingsArrowWidth;
  int distanceArrowY = distanceLabelY + 20;
  distanceLeftArrow.setBounds(distanceArrowX1, distanceArrowY, settingsArrowWidth, settingsArrowHeight);
  distanceLeftArrow.setDirection(ARROW_LEFT);
  distanceLeftArrow.setTriangleSize(12, 18);  // Explicit triangle size
  distanceLeftArrow.setBgColor(settingsMenuBackgroundColor);
  distanceLeftArrow.setTriangleColor(ILI9341_WHITE);
  distanceLeftArrow.draw(tft);
  distanceRightArrow.setBounds(distanceArrowX2, distanceArrowY, settingsArrowWidth, settingsArrowHeight);
  distanceRightArrow.setDirection(ARROW_RIGHT);
  distanceRightArrow.setTriangleSize(12, 18);  // Explicit triangle size
  distanceRightArrow.setBgColor(settingsMenuBackgroundColor);
  distanceRightArrow.setTriangleColor(ILI9341_WHITE);
  distanceRightArrow.draw(tft);

  // Set up distance value display
  int maxSizeChars = 8;  // "Standard" is the longest label
  int maxSizeWidth = maxSizeChars * settingsCharWidth - fontPadding;
  distanceValue.setPosition(
      settingsMenuX + (settingsMenuWidth - maxSizeWidth) / 2,
      distanceArrowY + (settingsArrowHeight - settingsTextSize * FONT_CHAR_HEIGHT) / 2,
      maxSizeWidth
  );
  distanceValue.setColors(settingsMenuBackgroundColor, settingsMenuTextColor);
  distanceValue.setTextSize(settingsTextSize);
  distanceValue.setFontMetrics(FONT_CHAR_WIDTH, FONT_CHAR_HEIGHT);
  distanceValue.setValue(DRIVE_DISTANCE_LABELS[driveDistance]);
  distanceValue.draw(tft);
}

void updateBrightnessDisplay() {
  brightnessValue.setValue(String(displayBrightness) + "%");
  brightnessValue.draw(tft);
}

void updateDriveSpeedDisplay() {
  speedValue.setValue(DRIVE_SPEED_LABELS[driveSpeed]);
  speedValue.draw(tft);
}

void updateDriveDistanceDisplay() {
  distanceValue.setValue(DRIVE_DISTANCE_LABELS[driveDistance]);
  distanceValue.draw(tft);
}

bool isPointInRect(int x, int y, int rectX, int rectY, int rectWidth, int rectHeight) {
  return x >= rectX && x <= rectX + rectWidth && y >= rectY && y <= rectY + rectHeight;
}

bool isTouchInGrid(int x, int y) {
  // Subtract one to maintain previous < comparisons
  return isPointInRect(x, y, offsetX, offsetY, gridWidth - 1, gridHeight - 1);
}

// Function to calculate direction between two points
int getDirection(int startRow, int startCol, int endRow, int endCol) {
  if (endRow < startRow) return 0;  // Up
  if (endCol > startCol) return 1;  // Right
  if (endRow > startRow) return 2;  // Down
  if (endCol < startCol) return 3;  // Left
  return currentDirection;           // No change
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

  // Get current and next points
  PathCell current = path[currentPathIndex];
  PathCell next = path[currentPathIndex + 1];

  // Calculate target direction
  int targetDirection = getDirection(current.row, current.col, next.row, next.col);

  switch (driveState) {

    // Stopped driving state
    case STOPPED:
      {

        // Print current cell being processed
        printCurrentCell(current, next);

        // Redraw current cell
        drawGridCells(current.row, current.row, current.col, current.col);

        // Check if aligned to target direction
        if (currentDirection == targetDirection) {

          // Set driving flags
          driveState = DRIVING;
          moveStartTime = millis();

          // Here you would add actual motor control:
          // driveForward();

          Serial.println("Driving");
        }
        // Not aligned to target direction
        else {

          // Set turning flags
          driveState = TURNING;
          moveStartTime = millis();

          // Get turn direction
          int turn = calculateTurn(currentDirection, targetDirection);

          // Here you would add actual motor control:
          // if (turn < 0) turnLeft();
          // if (turn > 0) turnRight();

          Serial.print("Turning ");
          Serial.println(turn < 0 ? "left" : "right");
        }
        break;
      }

    // Driving state
    case DRIVING:
      {

        // Check if driving movement is complete
        if (millis() - moveStartTime >= FORWARD_MOVE_TIME) {

          // Update current path point
          currentPathIndex++;
          moveStartTime = millis();  // Reset timer for next movement

          // Check if current path point is the last
          if (currentPathIndex >= pathLength - 1) {

            // Here you would stop motors:
            // stopMotors();
            Serial.println("Path complete");

            // Redraw current cell
            drawGridCells(next.row, next.row, next.col, next.col);

            // Update UI and driving state
            uiState = COMPLETE;
            driveState = STOPPED;
            drawStartButton();
          }

          // Current path point is not the last
          else {

            // Calculate next direction
            current = path[currentPathIndex];
            next = path[currentPathIndex + 1];
            int nextDirection = getDirection(current.row, current.col, next.row, next.col);

            // Print current cell being processed
            printCurrentCell(current, next);

            // Redraw current cell
            drawGridCells(current.row, current.row, current.col, current.col);

            // If direction changes, stop for turn
            if (nextDirection != currentDirection) {
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

          // Update direction and start driving
          currentDirection = targetDirection;
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
      drawGridCells(0, numRows - 1, 0, numCols - 1);

      // Draw button
      drawStartButton();

      // Debounce delay
      delay(250);
    }

    // Check if undo button touched while in idle state
    else if (undoButton.contains(pixelX, pixelY) && uiState == IDLE) {
      Serial.println("Undo button touched");

      // Reset grid values and path to default cells
      resetGridValues();
      resetDefaultPath();

      // Redraw entire grid
      drawGridCells(0, numRows - 1, 0, numCols - 1);

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
      int gridCol = (pixelX - offsetX) / cellSize;
      int gridRow = (pixelY - offsetY) / cellSize;
      Serial.println(gridCol);
      Serial.println(gridRow);
      Serial.println(" ");

      // If cell is selectable, add to path and redraw surrounding cells
      if (isSelectable(gridRow, gridCol)) {
        pathAdd(gridRow, gridCol);
        drawGridCells(gridRow - 2, gridRow + 2, gridCol - 2, gridCol + 2);
      }
    }

    // Check if in settings state
    else if (uiState == SETTINGS) {

      // Check if touch is in brightness right arrow
      if (brightnessRightArrow.contains(pixelX, pixelY)) {
        // Increase brightness and redraw value
        displayBrightness = min(100, displayBrightness + 10);
        setBrightness();
        updateBrightnessDisplay();
        delay(250);
      }

      // Check if touch is in brightness left arrow
      else if (brightnessLeftArrow.contains(pixelX, pixelY)) {
        // Decrease brightness and redraw value
        displayBrightness = max(10, displayBrightness - 10);
        setBrightness();
        updateBrightnessDisplay();
        delay(250);
      }

      // Check if touch is in size right arrow
      else if (distanceRightArrow.contains(pixelX, pixelY)) {
        // Increase drive distance and redraw value
        if (driveDistance < DISTANCE_EXTENDED) {
          driveDistance = (DriveDistance)(driveDistance + 1);
          updateDriveDistanceDisplay();
        }
        delay(250);
      }

      // Check if touch is in size left arrow
      else if (distanceLeftArrow.contains(pixelX, pixelY)) {
        // Decrease drive distance and redraw value
        if (driveDistance > DISTANCE_COMPACT) {
          driveDistance = (DriveDistance)(driveDistance - 1);
          updateDriveDistanceDisplay();
        }
        delay(250);
      }

      // Check if touch is in speed right arrow
      else if (speedRightArrow.contains(pixelX, pixelY)) {
        // Increase drive speed and redraw value
        if (driveSpeed < SPEED_FAST) {
          driveSpeed = (DriveSpeed)(driveSpeed + 1);
          updateDriveSpeedDisplay();
        }
        delay(250);
      }

      // Check if touch is in speed left arrow
      else if (speedLeftArrow.contains(pixelX, pixelY)) {
        // Decrease drive speed and redraw value
        if (driveSpeed > SPEED_SLOW) {
          driveSpeed = (DriveSpeed)(driveSpeed - 1);
          updateDriveSpeedDisplay();
        }
        delay(250);
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
      currentPathIndex = 0;
      currentDirection = 0;
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
