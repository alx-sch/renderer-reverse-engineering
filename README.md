# Reverse-Engineering a C Graphics Library

## 🚀 Project Overview

This project is a case study in C-based **reverse-engineering**. The starting point was a single, pre-compiled "black-box" graphics library (`librenderer.dylib`) provided without any headers, documentation or source code.

### The Challenge

The goal was to analyze this binary and write a host application in C that could successfully interface with it. This required a ground-up investigation to:

- **Discover the API:** Identify the exported functions from the binary.
- **Deduce Functionality:** Determine the purpose of each function (e.g., `XX`, `XX`, `XX`) and their required arguments.
- **Reconstruct the State:** The library's functions operated on an unknown data structure. A core part of the challenge was to reverse-engineer the exact memory layout of this struct (its size, fields and offsets).
- **Implement a Host:** Write a `main.c` program to correctly allocate the state structure, initialize the library, run its main event loop and handle proper memory cleanup.

The final measure of success was to correctly initialize the renderer and display its embedded animation in a window.

### My Thought Process

XX
