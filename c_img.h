/*
C Image Module
Author: Tannaz Chowdhury  
GitHub: TannazC  
Date: 2025  

This file provides image-handling utility functions to work with a
custom RGB image format in binary (.bin) form. Functions include:
- Reading/writing binary image data
- Accessing and modifying pixel values
- Creating and destroying image structures
- Visualizing pixel gradients for debugging

Used as a backend for the seam carving project.
*/

#include "c_img.h"       // Include custom image struct and function prototypes
#include <stdio.h>        // For file I/O
#include <math.h>         // For mathematical operations (used elsewhere)

// ----------------------------------------
// Function: create_img
// ----------------------------------------
/*
   Allocates memory for an image and initializes the height, width,
   and RGB raster data. Each pixel contains 3 bytes (R, G, B).
*/
void create_img(struct rgb_img **im, size_t height, size_t width){
    *im = (struct rgb_img *)malloc(sizeof(struct rgb_img));     // Allocate memory for image struct
    (*im)->height = height;                                     // Set height
    (*im)->width = width;                                       // Set width
    (*im)->raster = (uint8_t *)malloc(3 * height * width);      // Allocate memory for RGB data (3 bytes per pixel)
}

// ----------------------------------------
// Function: read_2bytes
// ----------------------------------------
/*
   Reads 2 bytes from a binary file and combines them into a 16-bit integer.
   Used to decode image width and height stored in .bin format.
*/
int read_2bytes(FILE *fp){
    uint8_t bytes[2];
    fread(bytes, sizeof(uint8_t), 1, fp);       // Read first byte
    fread(bytes+1, sizeof(uint8_t), 1, fp);     // Read second byte
    return (((int)bytes[0]) << 8) + (int)bytes[1]; // Combine as big-endian integer
}

// ----------------------------------------
// Function: write_2bytes
// ----------------------------------------
/*
   Splits a 16-bit integer into two bytes and writes them to a binary file.
   Used to encode image width and height.
*/
void write_2bytes(FILE *fp, int num){
    uint8_t bytes[2];
    bytes[0] = (uint8_t)((num & 0xFFFF) >> 8);   // Higher byte (most significant)
    bytes[1] = (uint8_t)(num & 0xFF);            // Lower byte (least significant)
    fwrite(bytes, 1, 1, fp);                     // Write first byte
    fwrite(bytes+1, 1, 1, fp);                   // Write second byte
}

// ----------------------------------------
// Function: read_in_img
// ----------------------------------------
/*
   Reads a binary image file and loads it into memory. The image format
   includes 2 bytes each for height and width, followed by RGB pixel data.
*/
void read_in_img(struct rgb_img **im, char *filename){
    FILE *fp = fopen(filename, "rb");                      // Open binary file for reading
    size_t height = read_2bytes(fp);                        // Read image height
    size_t width = read_2bytes(fp);                         // Read image width
    create_img(im, height, width);                          // Allocate image struct and raster
    fread((*im)->raster, 1, 3 * width * height, fp);        // Read all RGB data into raster
    fclose(fp);                                             // Close file
}

// ----------------------------------------
// Function: write_img
// ----------------------------------------
/*
   Writes an image to a binary file in the expected output format:
   [2 bytes height][2 bytes width][3 * H * W RGB values]
*/
void write_img(struct rgb_img *im, char *filename){
    FILE *fp = fopen(filename, "wb");                      // Open file for binary writing
    write_2bytes(fp, im->height);                          // Write height
    write_2bytes(fp, im->width);                           // Write width
    fwrite(im->raster, 1, im->height * im->width * 3, fp); // Write all RGB data
    fclose(fp);                                            // Close file
}

// ----------------------------------------
// Function: get_pixel
// ----------------------------------------
/*
   Returns the value (0-255) of the specified color channel at pixel (y, x).
   Channels: 0 = Red, 1 = Green, 2 = Blue
*/
uint8_t get_pixel(struct rgb_img *im, int y, int x, int colour){
    return im->raster[3 * (y * im->width + x) + colour];  // Compute index and return value
}

// ----------------------------------------
// Function: set_pixel
// ----------------------------------------
/*
   Sets the RGB values of the pixel at (y, x) to the specified values.
*/
void set_pixel(struct rgb_img *im, int y, int x, int r, int g, int b){
    im->raster[3 * (y * im->width + x) + 0] = r;  // Set Red
    im->raster[3 * (y * im->width + x) + 1] = g;  // Set Green
    im->raster[3 * (y * im->width + x) + 2] = b;  // Set Blue
}

// ----------------------------------------
// Function: destroy_image
// ----------------------------------------
/*
   Frees all memory used by the image struct.
   Order matters: free raster first, then the struct.
*/
void destroy_image(struct rgb_img *im){
    free(im->raster);   // Free RGB data
    free(im);           // Free image struct
}

// ----------------------------------------
// Function: print_grad
// ----------------------------------------
/*
   Prints the grayscale energy (gradient) values of the image,
   assuming values are stored in the red channel.
*/
void print_grad(struct rgb_img *grad){
    int height = grad->height;
    int width = grad->width;
    for(int i = 0; i < height; i++) {              // For each row
        for(int j = 0; j < width; j++) {           // For each column
            printf("%d\t", get_pixel(grad, i, j, 0)); // Print red channel value
        }
        printf("\n");                              // Newline after each row
    }
}
