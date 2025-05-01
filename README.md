# Seam Carving - Content-Aware Image Resizing
**Author:** Tannaz Chowdhury -> ESC190 Prof. Micheal Guerzhoy  
**GitHub:** TannazC  
**Date:** 2025

## Overview
This project implements seam carving, a content-aware image resizing algorithm. Rather than blindly scaling or cropping, seam carving intelligently removes paths of least importance (called "seams") from an image. This technique preserves critical image features like objects and edges, making it ideal for smart resizing.

Seam carving is widely used in image editing software like Adobe Photoshop and was first introduced in 2007. This implementation performs vertical seam carving by computing energy values across an image and iteratively removing seams with the lowest energy.

## Features
- Computes dual-gradient energy of each pixel
- Builds a cumulative energy cost map using dynamic programming
- Identifies a vertical seam with the minimum total energy
- Removes the identified seam from the image
- Repeats the process multiple times to progressively resize the image

---

## Files Included

### C Source Files
- **seamcarving.c** – Core logic for computing energy, finding seams, and modifying images.
- **seamcarving.h** – Header file declaring all seam carving functions.
- **c_img.c** – Handles reading, writing, and managing custom `.bin` image format.
- **c_img.h** – Header file defining the RGB image structure and helper functions.

### Python Scripts
- **convert.py** – Reads a PNG image, converts it to `.bin` format using `png2bin`, then reads back the binary for verification.
- **png2bin.py** – Converts standard PNG files to binary format and vice versa for compatibility with the C code.

### Binary/Media
- `HJoceanSmall.png`, `HJoceanSmall.bin`, `3x4.png`, etc. – Sample image files used for testing.

---

## How It Works

### Part 1: Dual-Gradient Energy
The function `calc_energy()` computes the energy of each pixel based on color differences with its neighbors. High energy means visually important.
- Energy is calculated using the formula:
  \[ E(y, x) = \sqrt{\Delta^2_x + \Delta^2_y} \]
- Edge pixels use wrap-around logic.
- The result is stored as a grayscale image (all RGB channels equal).

### Part 2: Dynamic Seam
The function `dynamic_seam()` creates a 1D array where each entry contains the minimum energy required to reach that pixel from the top. It uses dynamic programming to compute the cheapest seam path.

### Part 3: Recover Seam Path
The function `recover_path()` traces back from the bottom of the energy map to find the path (one pixel per row) that forms the seam with the least total energy.

### Part 4: Remove Seam
The function `remove_seam()` generates a new image with the identified seam removed.

### Part 5: Multiple Seam Removals (Test Loop)
The provided main loop in `seamcarving.c` removes 5 vertical seams from a binary image, writes each intermediate result to a new `.bin` file, and displays the filenames.

---

## Languages & Tools
- **C** – Image processing and seam carving implementation
- **Python** – PNG to BIN conversion utilities
- **PIL (Python Imaging Library)** – Used in Python scripts for image manipulation
- **Valgrind** – Optional memory debugging (recommended for testing memory safety)

---

## How to Use
1. Convert your PNG to `.bin` using `convert.py`:
   ```bash
   python convert.py
   ```

2. Compile the C code (example):
   ```bash
   gcc -Wall -std=c99 seamcarving.c c_img.c -o seamcarve
   ```

3. Run the seam carving program:
   ```bash
   ./seamcarve
   ```

4. Convert `.bin` results back to `.png` using `png2bin.py`, or preview them using custom viewer.

---

## Notes
- You must implement all functions in `seamcarving.h` within `seamcarving.c`.
- Ensure all memory allocations are freed appropriately.
- The image dimensions and binary format must match expectations in `c_img.h`.

---

## Acknowledgments
Original assignment design: Josh Hug  
C Port and adaptations: Michael Guerzhoy

This project is used in educational settings to teach content-aware algorithms, dynamic programming, and systems-level image processing.


