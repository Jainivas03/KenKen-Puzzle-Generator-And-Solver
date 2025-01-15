# KenKen-Puzzle-Generator-And-Solver

This project implements a KenKen puzzle solver in C++ with a user-friendly interface for input and output. The solver uses backtracking to find a solution that satisfies all row, column, and cage constraints.

## Features
- **Dynamic Puzzle Size**: The user can specify the size of the grid (`n x n`).
- **Cage Constraints**: Supports the following operations within cages:
  - Addition (`+`)
  - Subtraction (`-`)
  - Multiplication (`*`)
  - Division (`/`)
- **Colorful Output**: Displays puzzle cages with distinct colors for better visualization.
- **Backtracking Algorithm**: Efficiently explores possible solutions while respecting constraints.
