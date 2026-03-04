# 🧱 Bare-Metal Tetris on LPC1768 (ARM Cortex-M3)

> A complete, bare-metal implementation of the classic Tetris game developed in C for the NXP LPC1768 microcontroller (LandTiger Board).

![Project Status](https://img.shields.io/badge/Status-Completed-success)
![Platform](https://img.shields.io/badge/Platform-NXP%20LPC1768-blue)
![Language](https://img.shields.io/badge/Language-Embedded%20C-orange)
![Language](https://img.shields.io/badge/Language-ASM-orange)

## 📋 Overview

This project was developed for the **Computer Systems Architecture** course. It demonstrates a fully functional game engine running directly on hardware without an Operating System. The architecture relies heavily on **Interrupt Service Routines (ISRs)** and hardware timers to manage game physics, audio synthesis, and user input, ensuring high performance and low latency.

## ✨ Key Features

* **Bare-Metal Architecture:** No OS involved. Direct manipulation of registers (CMSIS standard).
* **Event-Driven System:**
    * **RIT (Repetitive Interrupt Timer):** Acts as the main scheduler for joystick polling and gravity physics (50ms tick).
    * **External Interrupts (EINT):** Handle critical inputs (Start, Pause, Hard Drop) with prioritized execution.
* **Optimized Graphics Engine:** Implements a **Partial Rendering Algorithm** on the GLCD. Instead of clearing the whole screen, only the moving pixels are redrawn, ensuring a high frame rate on the parallel bus display.
* **Audio Synthesis:** Background music and SFX generated via **Timer0** hardware toggling (Square wave synthesis).
* **Dynamic Difficulty:** Game speed is adjustable in real-time using the on-board **Potentiometer (ADC)**.
* **Advanced Mechanics:** Includes Power-ups and penalty systems (Malus) based on player performance.

## 🎮 How to Play

### Controls

| Component | Action | Description |
| :--- | :--- | :--- |
| **Joystick LEFT** | Move Left | Shift the tetromino to the left. |
| **Joystick RIGHT** | Move Right | Shift the tetromino to the right. |
| **Joystick UP** | Rotate | Rotate the piece 90° clockwise. |
| **KEY1 (Button)** | Start / Restart | Starts the game or restarts after Game Over. |
| **KEY2 (Button)** | **Hard Drop** | Instantly drops the piece to the bottom (High Priority). |
| **Potentiometer** | Speed Control | Rotate to increase/decrease game speed in real-time. |

### Game Mechanics (Extra Features)

In addition to standard Tetris rules, this implementation includes specific gameplay modifiers:

* **⚡ Power-ups (Every 5 lines cleared):**
    * **Half-Clear (Yellow Block):** Instantly clears the top 50% of the board blocks.
    * **Slow-Down (Magenta Block):** Temporarily slows down gravity for 15 seconds.
* **💀 Malus (Every 10 lines cleared):**
    * The board shifts up by one row.
    * A "garbage" row (random grey blocks) is generated at the bottom, reducing play space.

## 🛠️ Technical Architecture

### 1. Scheduler (RIT)
The **Repetitive Interrupt Timer** fires every 50ms. It handles:
* **Input Polling:** Reads the joystick state. Since mechanical contacts are noisy, the 50ms interval acts as a natural **debouncing filter**.
* **Gravity:** Increments the fall counter based on the speed defined by the ADC.

### 2. Analog-to-Digital Converter (ADC)
The ADC reads the potentiometer value to set the global speed variable `gameSpeedDivider`.
* **Noise Filtering:** A software filter (`ADC_CleanNoise`) is applied to the raw 12-bit value to prevent speed jitter caused by electrical noise.
* **Visual Feedback:** The current speed level is visualized on the on-board LEDs.

### 3. Random Number Generation (RNG)
To ensure random pieces every game, the **RIT Counter** value is captured at the exact moment the user presses `KEY1` (Start). This unpredictable hardware timer value is used as the seed for `srand()`.

## 📂 File Structure

* `sample.c`: Main entry point. Initializes peripherals and enters `__ASM("wfi")` (Wait For Interrupt) to save power.
* `tetris.c`: Core game logic (Finite State Machine, Collision Detection, Rendering).
* `IRQ_RIT.c`: Handles Joystick input and physics tick.
* `IRQ_adc.c`: Handles speed adjustment from the potentiometer.
* `IRQ_button.c`: Handles Start and Hard Drop inputs.
* `IRQ_timer.c`: Handles audio frequency generation.
* `GLCD.c`: Low-level drivers for the LCD display.

## 🚀 Build & Run

1.  Open the project file (`.uvproj`) in **Keil uVision 5**.
2.  Ensure the target is set to **LandTiger (LPC1768)**.
3.  Compile the project (Build).
4.  Flash the board using the USB ISP or J-Link debugger.
5.  Press **Reset** on the board and enjoy!

## 👨‍💻 Author

**Domenico di Stasio**
*Computer Engineer - Computer Architectures*

