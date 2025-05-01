/*
Seam Carving Header File
Author: Tannaz Chowdhury  
GitHub: TannazC  
Date: 2025  

This header file declares the core seam carving functions used in
seamcarving.c. It serves as an interface between files, allowing
functions defined in one source file to be used in another.

Each function here operates on image structures defined in c_img.h
and contributes to the dynamic seam carving process.
*/

#ifndef SEAMCARVING_H           // Include guard - prevents multiple includes
#define SEAMCARVING_H

#include "c_img.h"              // Required for struct rgb_img definitions

// ----------------------------------------
// Function: calc_energy
// ----------------------------------------
/*
   Calculates the dual-gradient energy for each pixel in the input image.
   Stores grayscale energy values in a new output image.
   
   Parameters:
     im   - pointer to the input image
     grad - pointer to the address of the gradient image to allocate and populate
*/
void calc_energy(struct rgb_img *im, struct rgb_img **grad);

// ----------------------------------------
// Function: dynamic_seam
// ----------------------------------------
/*
   Builds a dynamic programming cost array.
   Each entry contains the minimum cumulative energy to reach that pixel.

   Parameters:
     grad     - grayscale energy image
     best_arr - pointer to the address of the output 1D cost array
*/
void dynamic_seam(struct rgb_img *grad, double **best_arr);

// ----------------------------------------
// Function: recover_path
// ----------------------------------------
/*
   Backtracks from the bottom of the best_arr to find a vertical seam
   with the lowest energy. The seam is returned as an array of column indices.

   Parameters:
     best   - 1D cost array
     height - image height
     width  - image width
     path   - pointer to array to store seam path (column indices per row)
*/
void recover_path(double *best, int height, int width, int **path);

// ----------------------------------------
// Function: remove_seam
// ----------------------------------------
/*
   Removes a vertical seam from the input image, creating a new image
   with one fewer column.

   Parameters:
     src   - original source image
     dest  - pointer to the address of the new image with seam removed
     path  - array of column indices indicating seam pixels to remove
*/
void remove_seam(struct rgb_img *src, struct rgb_img **dest, int *path);

#endif  // End of include guard for SEAMCARVING_H
