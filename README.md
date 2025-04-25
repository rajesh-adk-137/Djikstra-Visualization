

## Dijkstra Pathfinding Visualizer

An interactive, real-time visualization of Dijkstra’s shortest-path algorithm, built in C++ with SFML. Watch the algorithm explore a grid, handle weighted cells, and highlight the optimal path.

---

### Table of Contents

1. [Features](#features)  
2. [Screenshots](#screenshots)  
3. [Prerequisites](#prerequisites)  
4. [Building & Setup](#building--setup)  
5. [Usage & Controls](#usage--controls)  
6. [Future Improvements](#future-improvements)  
7. [License](#license)  

---

### Features

- **Random Obstacles & Dynamic Weights**  
  - Press **D** to toggle **dynamic weights**: each non-obstacle cell has a 50% chance of cost 2 instead of the default 1.  
  - Automatically regenerates a fresh obstacle layout when toggled.

- **Custom Obstacle Drawing Mode**  
  - Click and drag on the grid to paint walls wherever you like.  
  - Toggle back to random generation at any time by pressing **D** again.

- **Start & End Point Selection**  
  - **First click**: select the **start** cell.  
  - **Second click**: select the **end** cell.  
  - **Third click**: resets and becomes the new start.

- **Animated Exploration**  
  - Witness Dijkstra’s algorithm step through each visited cell.  

- **Shortest-Path Highlighting**  
  - Upon completion, the shortest route from start to end is overlaid.  
  - If no path exists, a prominent “No Path” graphic is displayed.

- **Sound Effects**  
  - Feedback sounds for button clicks, obstacle placement, path-found chime, and algorithm start.

---

### Screenshots

| Random Grid View                                        | Custom Walls Drawing                                     |
|:--------------------------------------------------------:|:---------------------------------------------------------:|
| ![Random Grid View](https://github.com/user-attachments/assets/6f3c1156-2efc-4bc6-88e7-2c943046f619) | ![Custom Walls Drawing](https://github.com/user-attachments/assets/f93b4858-3ece-47dc-8884-c1687f42b012) |

| Path Found Highlight                                     | No Path Indicator                                         |
|:--------------------------------------------------------:|:---------------------------------------------------------:|
| ![Path Found](https://github.com/user-attachments/assets/e61c24c9-ad53-4eda-8e58-143572fe37e6) | ![Path Not Found](https://github.com/user-attachments/assets/cc82be20-21ee-4761-95e7-a93ea6fecedc) |

---

### Prerequisites

- **Operating System:** Windows 10 or 11  
- **IDE:** Visual Studio 2019 or 2022 (Community or higher)  
- **Libraries:** SFML 2.5.x (Graphics, Window, System, Audio modules)

---

### Building & Setup

1. **Clone or download** this repository.  
2. **Open** `DijkstraVisualizer.sln` in Visual Studio.  
3. **Configure SFML paths**:  
   - **Project → Properties → C/C++ → Additional Include Directories**  
     ```
     C:\Path\To\SFML\include
     ```  
   - **Project → Properties → Linker → General → Additional Library Directories**  
     ```
     C:\Path\To\SFML\lib
     ```  
   - **Project → Properties → Linker → Input → Additional Dependencies**  
     ```
     sfml-graphics.lib  
     sfml-window.lib  
     sfml-system.lib  
     sfml-audio.lib  
     ```  
4. **Copy SFML DLLs** (`sfml-*.dll`) from your SFML `bin` folder into your project’s output directory (e.g., `Debug\` or `Release\`).  
5. **Build** the solution (Ctrl + Shift + B).  
6. **Run** the executable (F5).

<setting up sfml for first time can be frustrating in visual studio, it is recommended to follow tutorials from youtube for proper setup guide>


### Usage & Controls

- **Toggle Dynamic Weights & Regenerate Grid**  
  - **Key:** `D`  
  - **Effect:** Randomizes obstacles and toggles 1↔2 cell weights.

- **Draw Custom Obstacles**  
  - **Action:** Click & drag on grid.  

- **Select Start & End Points**  
  1. **Click** on a cell → sets **Start**.  
  2. **Click** again → sets **End**.  
  3. **Click** a third time → resets and treats that as new Start.

- **Run Dijkstra’s Algorithm**  
  - **Effect:** Begins the animated exploration.  

- **Exit**  
  - **Action:** Click the window’s close button.

---

### Future Improvements

- Integrate A*, Bellman–Ford, or other pathfinding algorithms for comparison.  
- Allow runtime configuration of grid size and cell dimensions.  
- Add right-click eraser mode for obstacles.  

---

### License

This project is open-source under the **MIT License**. See [LICENSE](LICENSE) for details.
