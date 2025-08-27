# Sim2D_Visual

[WATCH THE DEMO ON YOUTUBE!](https://youtu.be/7Ij95vuk4KU)

## Overview

This project focuses on implementing simple **2D simulations** in modern **C++20** with real-time visualization using the [raylib](https://www.raylib.com/) graphics library.  
The work is split into three parts:  
1. Simulating the **heat equation** using an explicit finite-difference method.  
2. Simulating the **wave equation** using a leapfrog integration scheme.  
3. Implementing **Conway’s Game of Life**, a classical cellular automaton.  

The project was carried out as a **personal project** to become familiar with C++ development, scientific programming practices, and real-time visualization.

## Goals

- Gain practical experience with **C++20** and **CMake** project workflows.  
- Implement and visualize simple physical models (heat diffusion and wave propagation).  
- Explore **cellular automata** as a discrete simulation method.  
- Practice writing **modular, commented, and maintainable code**.  
- Use **raylib** for rendering and user interaction in real time.  

## Approach

- The project is structured with clear separation of concerns:
  - `src/` → main program entry and rendering loop.  
  - `include/` → header files implementing simulation logic (heat, wave, life).  
  - `CMakeLists.txt` → build system configuration.  
- A simple menu allows switching between simulation modes:  
  1. **Heat equation** — explicit finite-difference update scheme.  
  2. **Wave equation** — leapfrog method for time integration.  
  3. **Game of Life** — grid-based update rules for cellular automata.  
- Rendering handled by **raylib**, which manages the window, drawing loop, and input.  
- Input processed through raylib functions (`IsKeyPressed`, `IsKeyDown`, etc.).  

### Implementation Details

- Default grid resolution: **256 × 256**, scaled for display.  
- Time integration schemes:
  - Heat: forward Euler, with adjustable diffusion constant.  
  - Wave: leapfrog method for stable oscillatory behavior.  
- Visualization:
  - Each grid cell mapped to a pixel, colored via a colormap.  
  - Frame rate set to **120 FPS** for smooth updates.  
- User input:
  - `1` → Heat equation.  
  - `2` → Wave equation.  
  - `3` → Game of Life.  
  - `ESC` → Quit program.  

## Key Results

- Successfully implemented **interactive visualizations** of PDE-based simulations.  
- Heat diffusion shows smooth spreading consistent with physical expectations.  
- Wave equation produces oscillatory behavior with stable leapfrog integration.  
- Game of Life evolves recognizable patterns (oscillators, gliders).  
- The project runs interactively on macOS, Linux, and Windows.  

## Conclusions

- This project provided valuable hands-on experience with **modern C++** project setup and numerical methods.  
- Even simple PDE solvers combined with visualization offer intuitive insight into dynamics.  
- **raylib** proved to be a lightweight yet effective library for scientific visualization and real-time interaction.  
- The modular structure allows easy extension with new simulation models.  

## How to Run

Requirements:
- **CMake** (≥ 3.16)  
- **C++20 compiler** (g++, clang, or MSVC)  
- **raylib**  

### Build & Run
```bash
# macOS (Homebrew)
brew install cmake raylib
```

# Ubuntu/Debian
```bash
sudo apt-get update
sudo apt-get install -y cmake g++ build-essential libraylib-dev
```

# Windows (vcpkg)
```bash
vcpkg install raylib
```

# Build
```bash
mkdir build && cd build
cmake ..
cmake --build .
```

# Run 
```bash
./sim2d        # Linux/macOS
.\Debug\sim2d.exe   # Windows Debug
.\Release\sim2d.exe # Windows Release
```

# Acknowledgements
Personal project by Giorgos Kritopoulos, carried out to practice C++ development and explore real-time simulation and visualization.

Date: 26 August 2025
