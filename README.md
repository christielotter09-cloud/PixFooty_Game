# 🎮 PixFooty – Embedded 2D Football Game on Microcontroller

## 📌 Overview
**PixFooty** is a real-time 2D football (soccer) game implemented in **C++** and designed to run on an **embedded microcontroller platform**. The game simulates a football match between a human player and a CPU-controlled opponent, using low-level hardware interaction for user input, display output, and timing control.

This project was developed as part of an embedded systems and real-time programming module, showcasing skills in **object-oriented programming**, **microcontroller interfacing**, **game engine logic**, and **AI decision-making** in constrained hardware environments.

---

## 🧠 Game Architecture
PixFooty is structured using **modular C++ classes**, each representing a critical component of the game engine:

| File               | Purpose |
|--------------------|---------|
| `Player.cpp` / `Player.h` | Handles player movement, control logic, and interactions |
| `Dec.cpp` / `Dec.h` | CPU opponent AI (decision engine) – calculates automated movements and responses |
| `Ball.cpp` / `Ball.h` | Physics and movement of the football object |
| `Joystick.cpp` / `Joystick.h` | Reads real-time input from joystick or buttons and maps it to game actions |
| `Map.cpp` / `Map.h` | Responsible for rendering the game field and boundary logic |
| `PixFootyEngine.cpp` / `PixFootyEngine.h` | Main game engine – manages game states, collision detection, scoring, and timing |
| `Utils.h` | Helper functions used across modules |

---

## 🎯 Core Features
- **Real-time game loop** with timed frame updates
- **Human vs AI gameplay** using `Dec.cpp` for CPU decision making
- **Joystick-based control input**
- **Embedded rendering logic** for field, player, and ball movement
- **Collision detection and scoring system**
- **Modular C++ architecture suitable for microcontroller deployment**

---

## 🛠 Technologies & Tools
- **Language:** C++
- **Platform:** STM32 NUCLEO or similar embedded microcontroller
- **Development Environment:** Mbed Studio / STM32CubeIDE
- **Peripherals:** Joystick/buttons, LED/pixel display (or serial output for simulation)
- **Programming Paradigm:** Object-Oriented Design

---

## 🚀 How to Build and Run
> This project is designed for embedded hardware
---

## 📚 Skills Demonstrated
`C++ (OOP) • Embedded Systems Programming • AI Logic Implementation • Game Engine Architecture • GPIO Input Processing • Real-Time Systems • Hardware Abstraction • Data Structures`

---

## 🌱 Future Extensions
- Add multiplayer mode
- Support OLED or LCD graphical display
- Add sound using PWM
- Implement stamina, fouls, or goalkeeper logic

---
## 🧾 Acknowledgements
This project was developed as part of my embedded systems coursework, exploring how **game logic and real-time processing** can be implemented on **microcontroller-based hardware platforms**.
