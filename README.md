# grid-bot

Grid-bot is a two wheeled Arduino based robot that follows a user drawn path on a touchscreen. The project includes an on-board UI and is compiled with the standard Arduino IDE.

## Hardware requirements

* **Microcontroller** – Any Arduino compatible board with enough pins (e.g. UNO, MEGA or similar).
* **Display** – 320×240 ILI9341 TFT display wired to pins 17–19 with an LED pin on 20.
* **Touchscreen** – Resistive touch panel using the `TouchScreen` library connected to pins 22, 23, A4 and A7.
* **Motors** – Two DC drive motors connected through a motor driver (such as an L298 based shield or breakout).
* **Power** – Suitable battery pack for the motors and microcontroller.

## Compiling with the Arduino IDE

1. Install the Arduino IDE.
2. Install the required libraries via the Library Manager:
   * **Adafruit GFX Library**
   * **Adafruit ILI9341**
   * **TouchScreen**
3. Open `Arduino/grid_bot/grid_bot.ino` in the IDE.
4. Select the board and serial port that match your microcontroller.
5. Click **Upload** to compile and flash the firmware.

## Using the UI

When the robot powers up the display shows a grid and three buttons: **Undo**, **Start** and **Settings**.

1. **Draw a path** – Tap cells on the grid. Each new cell must be adjacent to the previous one. A default start cell is provided at the bottom centre of the grid.
2. **Undo** – Press the **Undo** button to clear the drawn path and return to the default two starting cells.
3. **Settings** – Tap **Settings** to adjust display brightness, drive speed and drive distance, then tap the button again to return to the grid.
4. **Start** – Press **Start** to begin a short countdown. Once the countdown reaches zero the robot will execute the path. While running the button changes to **Stop** and can be pressed to abort.

After the last point in the path is reached the button displays **Done!** and the robot returns to the idle state ready for a new path.
