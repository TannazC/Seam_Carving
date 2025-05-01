/*
Seam Carving Algorithm Implementation - Main File 
Author: Tannaz Chowdhury  
GitHub: TannazC  
Date: 2025  
*/

#include <stdio.h>            // Required for printf and file IO
#include <stdlib.h>           // Required for malloc, free
#include <math.h>             // Required for sqrt
#include <float.h>            // For DBL_MAX constant
#include "c_img.h"            // Custom header to work with image pixel structs
#include "seamcarving.h"     // Header for seam carving function declarations

// ----------------------------------------
// Helper: Compute Gradient Component
// ----------------------------------------
/*
   This function calculates the difference in pixel values along either
   the x-axis or y-axis for a particular color channel (R, G, or B).
   It handles edge pixels using wrap-around logic.
*/
static int compute_gradient_component(const struct rgb_img *im, int y, int x, int color, char axis) {
    int width = im->width;     // Get image width
    int height = im->height;   // Get image height
    int delta = 0;             // Initialize delta (difference) to zero

    if (axis == 'x') {
        int left = (x == 0) ? width - 1 : x - 1;          // Wrap to last column if x == 0
        int right = (x == width - 1) ? 0 : x + 1;         // Wrap to first column if x is last
        delta = get_pixel((struct rgb_img *)im, y, right, color) - get_pixel((struct rgb_img *)im, y, left, color);  // Compute difference in x direction
    } else if (axis == 'y') {
        int top = (y == 0) ? height - 1 : y - 1;          // Wrap to last row if y == 0
        int bottom = (y == height - 1) ? 0 : y + 1;       // Wrap to first row if y is last
        delta = get_pixel((struct rgb_img *)im, bottom, x, color) - get_pixel((struct rgb_img *)im, top, x, color);  // Compute difference in y direction
    }
    return delta;  // Return the computed difference
}

// ----------------------------------------
// Function: calc_energy
// ----------------------------------------
/*
   This function calculates the energy of each pixel in the image.
   Energy is computed based on color gradients (change in color values).
   The energy is then scaled and saved as grayscale in the gradient image.
*/
void calc_energy(struct rgb_img *im, struct rgb_img **grad) {
    int width = im->width;    // Get image width
    int height = im->height;  // Get image height

    create_img(grad, height, width);  // Create a new grayscale image for energy values

    for (int y = 0; y < height; y++) {  // Loop through each row
        for (int x = 0; x < width; x++) {  // Loop through each column
            // Get gradients in both directions for all color channels
            int r_x = compute_gradient_component(im, y, x, 0, 'x');
            int g_x = compute_gradient_component(im, y, x, 1, 'x');
            int b_x = compute_gradient_component(im, y, x, 2, 'x');
            int r_y = compute_gradient_component(im, y, x, 0, 'y');
            int g_y = compute_gradient_component(im, y, x, 1, 'y');
            int b_y = compute_gradient_component(im, y, x, 2, 'y');

            // Total energy in x and y directions
            int delta_x = r_x * r_x + g_x * g_x + b_x * b_x;
            int delta_y = r_y * r_y + g_y * g_y + b_y * b_y;

            double energy = sqrt(delta_x + delta_y);  // Total energy using Euclidean distance
            uint8_t scaled = (uint8_t)(energy / 10);  // Scale down to fit into uint8_t

            set_pixel(*grad, y, x, scaled, scaled, scaled);  // Set grayscale energy value
        }
    }
}

// ----------------------------------------
// Function: dynamic_seam
// ----------------------------------------
/*
   Builds a 1D array where each cell holds the minimum energy to reach that pixel
   from the top of the image.
*/
static double min_neighbors(double *best_arr, int prev_row, int col, int width) {
    double min_val = DBL_MAX;  // Initialize to max possible double

    // Compare left, center, and right neighbors from the row above
    if (col > 0 && best_arr[prev_row * width + col - 1] < min_val)
        min_val = best_arr[prev_row * width + col - 1];

    if (best_arr[prev_row * width + col] < min_val)
        min_val = best_arr[prev_row * width + col];

    if (col < width - 1 && best_arr[prev_row * width + col + 1] < min_val)
        min_val = best_arr[prev_row * width + col + 1];

    return min_val;  // Return the smallest neighbor value
}

void dynamic_seam(struct rgb_img *grad, double **best_arr) {
    int width = grad->width;
    int height = grad->height;
    *best_arr = (double *)malloc(sizeof(double) * height * width);  // Allocate 1D array for cost matrix

    // Initialize the top row directly from energy image
    for (int j = 0; j < width; j++) {
        (*best_arr)[j] = get_pixel(grad, 0, j, 0);  // Only need one channel, since grayscale
    }

    // Fill out the rest of best_arr using dynamic programming
    for (int i = 1; i < height; i++) {
        for (int j = 0; j < width; j++) {
            double energy = get_pixel(grad, i, j, 0);  // Current energy value
            double min_cost = min_neighbors(*best_arr, i - 1, j, width);  // Best from top 3 neighbors
            (*best_arr)[i * width + j] = energy + min_cost;  // Accumulate cost
        }
    }
}

// ----------------------------------------
// Function: recover_path
// ----------------------------------------
/*
   Backtracks from the bottom row of best_arr to recover the seam
   with minimum total energy.
*/
static int find_best_neighbor(double *best_arr, int row, int center, int width) {
    int best_col = center;  // Start at current column
    double min_val = best_arr[row * width + center];

    // Check left neighbor
    if (center > 0 && best_arr[row * width + center - 1] < min_val) {
        best_col = center - 1;
        min_val = best_arr[row * width + center - 1];
    }

    // Check right neighbor
    if (center < width - 1 && best_arr[row * width + center + 1] < min_val) {
        best_col = center + 1;
    }

    return best_col;  // Return best neighbor column index
}

void recover_path(double *best, int height, int width, int **path) {
    *path = (int *)malloc(sizeof(int) * height);  // Allocate array to store path (one col per row)

    // Start from the minimum value in the last row
    int min_index = 0;
    double min_cost = best[(height - 1) * width];

    for (int j = 1; j < width; j++) {
        double cost = best[(height - 1) * width + j];
        if (cost < min_cost) {
            min_cost = cost;
            min_index = j;
        }
    }

    (*path)[height - 1] = min_index;  // Set last element in path

    // Backtrack up the rows to find the full seam
    for (int i = height - 2; i >= 0; i--) {
        int prev_index = (*path)[i + 1];
        (*path)[i] = find_best_neighbor(best, i, prev_index, width);
    }
}

// ----------------------------------------
// Function: remove_seam
// ----------------------------------------
/*
   Removes one vertical seam from the source image by skipping
   the pixel in the path and copying others to a new image.
*/
void remove_seam(struct rgb_img *src, struct rgb_img **dest, int *path) {
    int height = src->height;
    int width = src->width;
    create_img(dest, height, width - 1);  // Allocate image with one less column

    for (int i = 0; i < height; i++) {
        int seam_col = path[i];  // Column to be skipped in this row
        for (int j = 0, new_j = 0; j < width; j++) {
            if (j == seam_col) continue;  // Skip the seam pixel

            // Get RGB values from source image
            uint8_t r = get_pixel(src, i, j, 0);
            uint8_t g = get_pixel(src, i, j, 1);
            uint8_t b = get_pixel(src, i, j, 2);

            set_pixel(*dest, i, new_j, r, g, b);  // Copy to destination
            new_j++;  // Move to next destination column
        }
    }
}

// ----------------------------------------
// Main Function: Seam Carving Execution
// ----------------------------------------
/*
   Carves out 5 seams from an image, writing each step to disk.
*/
int main() {
    struct rgb_img *im;       // Input image pointer
    struct rgb_img *cur_im;   // Updated image after removing seam
    struct rgb_img *grad;     // Energy map
    double *best;             // Best path cost table
    int *path;                // Path array of seam columns

    read_in_img(&im, "HJoceanSmall.bin");  // Read image from binary file

    for (int i = 0; i < 5; i++) {
        printf("i = %d\n", i);                        // Output current step
        calc_energy(im, &grad);                       // Step 1: compute energy
        dynamic_seam(grad, &best);                    // Step 2: compute best seam costs
        recover_path(best, grad->height, grad->width, &path); // Step 3: backtrack to find seam path
        remove_seam(im, &cur_im, path);               // Step 4: create new image with seam removed

        char filename[200];
        sprintf(filename, "img%d.bin", i);            // Construct output filename
        write_img(cur_im, filename);                  // Save the new image

        destroy_image(im);                            // Free old image
        destroy_image(grad);                          // Free energy image
        free(best);                                   // Free best cost array
        free(path);                                   // Free seam path array
        im = cur_im;                                  // Continue using updated image
    }

    destroy_image(im);  // Final cleanup
    return 0;
}
