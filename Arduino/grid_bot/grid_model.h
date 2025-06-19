#ifndef GRID_MODEL_H
#define GRID_MODEL_H

#include <Arduino.h>

// Path cell structure
struct PathCell {
  int row;
  int col;
};

class GridModel {
private:
  // Grid dimensions
  int numRows;
  int numCols;
  int cellSize;
  int gridWidth;
  int gridHeight;
  int offsetX;
  int offsetY;
  
  // Grid data
  bool** gridVals;
  
  // Path data
  PathCell* path;
  int pathLength;
  int currentPathIndex;
  int currentDirection;
  
  // Grid colors
  uint16_t backgroundColor;
  uint16_t selectedColor;
  uint16_t emptyColor;
  uint16_t gridColor;
  uint16_t selectableColor;
  uint16_t arrowColor;

public:
  // Constructor and destructor
  GridModel();
  ~GridModel();
  
  // Initialization methods
  void initGrid(int availableWidth, int availableHeight);
  void resetGridValues();
  void initPath();
  void resetDefaultPath();
  
  // Path manipulation methods
  void pathAdd(int row, int col);
  bool isSelectable(int row, int col);
  void resetPath();
  
  // Grid state methods
  bool getGridValue(int row, int col);
  void setGridValue(int row, int col, bool value);
  bool isCellActivated(int row, int col);
  
  // Path access methods
  PathCell getPathCell(int index);
  PathCell* getPath();
  int getPathLength();
  int getCurrentPathIndex();
  void setCurrentPathIndex(int index);
  PathCell getCurrentPathCell();
  PathCell getNextPathCell();
  bool isPathComplete();
  
  // Direction methods
  int getCurrentDirection();
  void setCurrentDirection(int direction);
  int getNextDirection();
  
  // Grid dimension getters
  int getNumRows();
  int getNumCols();
  int getCellSize();
  int getGridWidth();
  int getGridHeight();
  int getOffsetX();
  int getOffsetY();
  
  // Color getters
  uint16_t getBackgroundColor();
  uint16_t getSelectedColor();
  uint16_t getEmptyColor();
  uint16_t getGridColor();
  uint16_t getSelectableColor();
  uint16_t getArrowColor();
  
  // Utility methods
  bool isInGridBounds(int row, int col);
  void clearAll();
};

#endif // GRID_MODEL_H 