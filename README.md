# Seam Carving – Content-Aware Image Resizing  
**Author:** Tannaz Chowdhury  
**Course:** ESC190 (University of Toronto)  
**Instructor:** Prof. Michael Guerzhoy  
**GitHub:** TannazC  
**Year:** 2025  

---

## 📌 Overview  
This project implements **seam carving**, a content-aware image resizing algorithm. Seam carving resizes images by removing the least noticeable pixels along connected vertical paths called seams—rather than simple cropping or scaling. This technique helps preserve the most visually significant content in an image, such as faces or objects.

The goal is to remove vertical seams from `.bin` image files, intelligently shrinking the image while maintaining visual structure.

---

## 🔧 Features
- Calculates the dual-gradient energy function for each pixel
- Uses dynamic programming to compute cumulative seam costs
- Recovers the lowest-energy vertical seam path
- Removes that seam from the image
- Iteratively processes and updates the image to shrink it

---

## 📁 File Descriptions

### C Source Files
| File | Description |
|------|-------------|
| `seamcarving.c` | Contains all core seam carving logic: energy computation, dynamic programming, seam recovery, and seam removal. |
| `seamcarving.h` | Header file for declaring seam carving functions used across `seamcarving.c`. |
| `c_img.c` | Handles reading, writing, allocating, modifying, and freeing `.bin` RGB images. |
| `c_img.h` | Header file that defines the image structure `rgb_img` and related utility function prototypes. |

### Python Scripts
| File | Description |
|------|-------------|
| `png2bin.py` | Converts `.png` files to custom `.bin` image format and back again using `PIL`. |
| `usepng2bin.py` | Sample script to preview `.png` images and convert them into `.bin` format for use in C. Demonstrates bidirectional conversion. |

### Binary Image Samples
| File | Description |
|------|-------------|
| `3x4 (1).bin`, `6x5 (1).bin` | Example small `.bin` images for testing functionality and debugging. |
| `HJoceanSmall.bin` | Main test image for seam removal (original size before carving). |

### Other
| File | Description |
|------|-------------|
| `LICENSE` | License for the project. |
| `README.md` | This documentation file. |
| `seamcarving_compiled` | Output executable generated after compilation (may be renamed during builds). |

---

## 🚀 How It Works

### 1. **Energy Calculation**
- Implemented in: `calc_energy()` (in `seamcarving.c`)
- Computes the dual-gradient energy of every pixel using x/y gradients in R, G, B channels.
- Edge pixels wrap around using modular arithmetic.
- Energy is scaled and stored as grayscale in all three RGB channels.

### 2. **Dynamic Seam Cost Map**
- Implemented in: `dynamic_seam()`
- Creates a 1D array that represents the minimum energy needed to reach each pixel from the top row.

### 3. **Seam Path Recovery**
- Implemented in: `recover_path()`
- Backtracks from the last row of the best energy array to find the optimal seam path (one pixel per row).

### 4. **Seam Removal**
- Implemented in: `remove_seam()`
- Creates a new image with the identified seam removed (one pixel fewer per row).

### 5. **Multiple Iterative Seams**
- Main loop in `seamcarving.c` demonstrates removing 5 vertical seams from an image in succession and saving the intermediate outputs.

---

## 💻 How to Use

### 📝 Step-by-Step

1. **Convert a PNG to BIN**  
   ```bash
   python usepng2bin.py
   ```

2. **Compile the C Code**
   ```bash
   gcc -Wall -std=c99 seamcarving.c c_img.c -o seamcarving_compiled
   ```

3. **Run the Seam Carving Program**
   ```bash
   ./seamcarving_compiled
   ```

4. **Convert `.bin` Output to `.png` (Optional)**
   ```bash
   python png2bin.py
   ```

---

## 🛠 Languages and Libraries

- **C** – For all core image processing logic
- **Python 3** – For converting `.png` to `.bin` formats
- **PIL (Pillow)** – Python Imaging Library for manipulating image data
- **Valgrind (Optional)** – For memory error checking during C execution

---

## 📎 Notes
- Ensure you free all dynamically allocated memory.
- Use `destroy_image()` after processing each image.
- Seam carving is vertical-only in this implementation, but can be extended to horizontal seams.
- Energy calculations wrap around the image boundaries.
- Image files must follow the custom `.bin` format: `[2B height][2B width][3B * width * height RGB values]`.

---

## 📚 Acknowledgments
- Original design: Josh Hug  
- C adaptation & coursework use: Prof. Michael Guerzhoy, University of Toronto  
- Implementation and documentation: Tannaz Chowdhury (ESC190, 2025)

This project is a valuable introduction to image processing, dynamic programming, and systems-level memory management.
