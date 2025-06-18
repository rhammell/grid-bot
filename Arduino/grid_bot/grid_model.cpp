#include "grid_model.h"
#include <Adafruit_GFX.h>

GridModel::GridModel() {
  // Initialize member variables
  numRows = 0;
  numCols = 0;
  cellSize = 30; // Default cell size
  gridWidth = 0;
  gridHeight = 0;
  offsetX = 0;
  offsetY = 0;
  
  // Initialize grid data
  gridVals = nullptr;
  
  // Initialize path data
  path = nullptr;
  pathLength = 0;
  currentPathIndex = 0;
  currentDirection = 0;
  
  // Initialize colors (these will be set properly when grid is initialized)
  backgroundColor = 0x0000; // Black
  selectedColor = 0x0000;   // Black
  emptyColor = 0xFFFF;      // White
  gridColor = 0x8410;       // Dark grey
  selectableColor = 0x07E0; // Green
  arrowColor = 0xFFFF;      // White
}

GridModel::~GridModel() {
  // Clean up grid memory
  if (gridVals != nullptr) {
    for (int i = 0; i < numRows; i++) {
      delete[] gridVals[i];
    }
    delete[] gridVals;
    gridVals = nullptr;
  }
  
  // Clean up path memory
  if (path != nullptr) {
    delete[] path;
    path = nullptr;
  }
}

void GridModel::initGrid(int screenWidth, int screenHeight, int buttonHeight, int buttonMargin) {
  // Calculate grid size based on available screen space
  int availableHeight = screenHeight - buttonHeight - 1 - buttonMargin;
  numRows = (availableHeight - 1) / cellSize;
  numCols = ((screenWidth - 1) / cellSize) - (((screenWidth - 1) / cellSize) % 2 == 0 ? 1 : 0);

  // Calculate grid dimensions and offsets
  gridWidth = cellSize * numCols;
  gridHeight = cellSize * numRows;
  offsetX = (screenWidth - gridWidth - 1) / 2;
  offsetY = (screenHeight - gridHeight - 1 - buttonHeight - buttonMargin) / 2;

  // Initialize 2D grid of cell values
  gridVals = new bool*[numRows];
  for (int i = 0; i < numRows; i++) {
    gridVals[i] = new bool[numCols];
  }

  // Reset all grid values to false
  resetGridValues();
  
  // Initialize path
  initPath();
}

void GridModel::resetGridValues() {
  for (int i = 0; i < numRows; i++) {
    for (int j = 0; j < numCols; j++) {
      gridVals[i][j] = false;
    }
  }
}

void GridModel::initPath() {
  // Initialize array of path points
  path = new PathCell[numRows * numCols];
  
  // Set initial default path points
  resetDefaultPath();
}

void GridModel::resetDefaultPath() {
  // Set path length to zero
  pathLength = 0;
  currentPathIndex = 0;

  // Add two points at center bottom of grid
  int startRow = numRows - 1;
  int startCol = numCols / 2;
  pathAdd(startRow, startCol);
  pathAdd(startRow - 1, startCol);
}

void GridModel::pathAdd(int row, int col) {
  // Add coordinate to the list of path points
  path[pathLength] = { row, col };
  pathLength += 1;

  // Update the grid value of point
  gridVals[row][col] = true;
}

bool GridModel::isSelectable(int row, int col) {
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

void GridModel::resetPath() {
  resetGridValues();
  resetDefaultPath();
}

bool GridModel::getGridValue(int row, int col) {
  if (isInGridBounds(row, col)) {
    return gridVals[row][col];
  }
  return false;
}

void GridModel::setGridValue(int row, int col, bool value) {
  if (isInGridBounds(row, col)) {
    gridVals[row][col] = value;
  }
}

bool GridModel::isCellActivated(int row, int col) {
  return getGridValue(row, col);
}

PathCell GridModel::getPathCell(int index) {
  if (index >= 0 && index < pathLength) {
    return path[index];
  }
  return { -1, -1 }; // Invalid cell
}

PathCell* GridModel::getPath() {
  return path;
}

int GridModel::getPathLength() {
  return pathLength;
}

int GridModel::getCurrentPathIndex() {
  return currentPathIndex;
}

void GridModel::setCurrentPathIndex(int index) {
  if (index >= 0 && index < pathLength) {
    currentPathIndex = index;
  }
}

PathCell GridModel::getCurrentPathCell() {
  return getPathCell(currentPathIndex);
}

PathCell GridModel::getNextPathCell() {
  return getPathCell(currentPathIndex + 1);
}

bool GridModel::isPathComplete() {
  return currentPathIndex >= pathLength - 1;
}

int GridModel::getCurrentDirection() {
  return currentDirection;
}

void GridModel::setCurrentDirection(int direction) {
  currentDirection = direction;
}

int GridModel::getNumRows() {
  return numRows;
}

int GridModel::getNumCols() {
  return numCols;
}

int GridModel::getCellSize() {
  return cellSize;
}

int GridModel::getGridWidth() {
  return gridWidth;
}

int GridModel::getGridHeight() {
  return gridHeight;
}

int GridModel::getOffsetX() {
  return offsetX;
}

int GridModel::getOffsetY() {
  return offsetY;
}

uint16_t GridModel::getBackgroundColor() {
  return backgroundColor;
}

uint16_t GridModel::getSelectedColor() {
  return selectedColor;
}

uint16_t GridModel::getEmptyColor() {
  return emptyColor;
}

uint16_t GridModel::getGridColor() {
  return gridColor;
}

uint16_t GridModel::getSelectableColor() {
  return selectableColor;
}

uint16_t GridModel::getArrowColor() {
  return arrowColor;
}

bool GridModel::isInGridBounds(int row, int col) {
  return row >= 0 && row < numRows && col >= 0 && col < numCols;
}

void GridModel::clearAll() {
  resetGridValues();
  resetDefaultPath();
} 