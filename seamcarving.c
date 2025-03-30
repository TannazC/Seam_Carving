#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "c_img.h"
#include "seamcarving.h"

/*
 Helper Function: compute_gradient_component
 
 Computes the difference in a specific color channel along the given axis ('x' or 'y')
 for the pixel at (y, x) in the image. This function uses wrap-around for edge pixels.
 Takes in the image pointer, x and y position, colour required and axis to move on (computing on x or y difference) */
static int compute_gradient_component(const struct rgb_img *im, int y, int x, int color, char axis) {
    int width = im->width; // get info from struct
    int height = im->height;
    int delta = 0; //set delta to 0; delta is the change on the respective axis

    if (axis == 'x') {
        // Determine left and right neighbors with wrap-around
        //USED TERNARY OPERATOR HERE
        //condition ? value_if_true : value_if_false;
        int left = (x == 0) ? width - 1 : x - 1; // if x is at left edge, left is width-1, wraps around; else subtract from x
        int right = (x == width - 1) ? 0 : x + 1; // x is at the right edge, wrap to beginning 0. else, add 1 to x right side
        delta = get_pixel((struct rgb_img *)im, y, right, color) - get_pixel((struct rgb_img *)im, y, left, color); // get pixel info using left and right as x replacements

    } else if (axis == 'y') {
        // Determine top and bottom neighbors with wrap-around, same algorithm as above
        int top = (y == 0) ? height - 1 : y - 1;
        int bottom = (y == height - 1) ? 0 : y + 1;
        delta = get_pixel((struct rgb_img *)im, bottom, x, color) - get_pixel((struct rgb_img *)im, top, x, color);
    }
    
    // return the uint8_t colour int difference
    return delta;
}

/*
 Function: calc_energy
 
 Computes the gradient energy for each pixel in the image and stores it in grad*
 The energy at pixel (y, x) is computed as:
     energy(y, x) = sqrt(Δ²ₓ(y, x) + Δ²ᵧ(y, x))
 
 where Δ²ₓ is the sum of the squares of the differences in the x direction,
 and Δ²ᵧ is for the y direction.
 
 The computed energy is then scaled by dividing by 10 and stored
 as a grayscale value (i.e. same value for R, G, and B) in the output image.
 */
void calc_energy(struct rgb_img *im, struct rgb_img **grad) {
    int width = im->width; // obtain info from struct
    int height = im->height;
    
    // Allocate memory for the energy image 
    create_img(grad, height, width); // MISSING PARAMETER 
    
    for (int y = 0; y < height; y++) { // for y
        for (int x = 0; x < width; x++) { // for x
            // Compute gradients in the x direction for each color channel
            int r_x = compute_gradient_component(im, y, x, 0, 'x'); //RGB x
            int g_x = compute_gradient_component(im, y, x, 1, 'x');
            int b_x = compute_gradient_component(im, y, x, 2, 'x');
            
            // Compute gradients in the y direction for each color channel
            int r_y = compute_gradient_component(im, y, x, 0, 'y'); //RGB y
            int g_y = compute_gradient_component(im, y, x, 1, 'y');
            int b_y = compute_gradient_component(im, y, x, 2, 'y');
            
            // Compute the squared gradient magnitudes in each direction
            int delta_x = r_x * r_x + g_x * g_x + b_x * b_x; // can use pow() instead but will be a double.. need to typecast
            int delta_y = r_y * r_y + g_y * g_y + b_y * b_y; //Δ2x(y,x)=Rx(y,x)^2+Gx(y,x)^2+Bx(y,x)^2
            
            // Compute the energy using the dual-gradient energy formula
            double energy = sqrt(delta_x + delta_y); // SQRT (Δ2x(y,x)+Δ2y(y,x))
        
            
            // Scale the energy by dividing by 10 and type cast it to uint8_t
            uint8_t scaled_energy = (uint8_t)(energy / 10); 
            
            // Set the pixel in the energy image (all RGB channels set to the same grayscale value)
            set_pixel(*grad, y, x, scaled_energy, scaled_energy, scaled_energy);
        }
    }
}



/*Part 2: Cost Array
Define the function dynamic_seam(struct rgb_img *grad, double **best_arr) which allocates and computes the dunamic array *best_arr.

(*best_arr)[i*width+j] contains the minimum cost of a seam from the top of grad to the point (i,j)

goes from top row and moves to bottom row,
.*/

#include <float.h>  // For DBL_MAX if needed

/*Helper function to compute the minimum cost among neighbors in the previous row.
skips first row, begins at second row, gets min of the three spots directly above and adjacent/ diagonal to position
gets min of that; replaces position with original value + MIN of top
this creates a minimum cost seam path
*/

// takes in seam path, previous row number, column we are working with, width to check left and right existance
static double min_neighbors(double *best_arr, int prev_row, int col, int width) {
    
    double min_val = DBL_MAX;  // Start with a very high value to keep adjusting the minimum value

    // Check the left neighbor (if it exists)
    if (col > 0) { // if we r not at original position
        double left = best_arr[prev_row * width + (col - 1)]; // get it this way because best_arr is technically a 1D array... move through rows one by one
        // DO NOT do (prev_row-1)
        if (left < min_val) { // set min
            min_val = left;
        }
    }
    
    // Check the middle neighbor (always exists bc we skip first row 0)
    // same algorithm as above 
    double mid = best_arr[prev_row * width + col];
    if (mid < min_val) {
        min_val = mid;
    }
    
    // Check the right neighbor (if it exists).
    if (col < width - 1) {
        double right = best_arr[prev_row * width + (col + 1)];
        if (right < min_val) {
            min_val = right;
        }
    }
    
    return min_val; // return adjusted min value to compute 
}

// creates a best_arr with a cheap seam path
// takes in gradient energy image, double pointer to best_arr location 
void dynamic_seam(struct rgb_img *grad, double **best_arr)
{
    int width = grad->width; // get info from struct 
    int height = grad->height;

    // Allocate memory for best_arr
    *best_arr = (double *)malloc(sizeof(double) * height * width); 

    // Initialize first row of best_arr with the same values as grad's first row.
    for (int j = 0; j < width; j++) { // go one by one and add
        (*best_arr)[j] = get_pixel(grad, 0, j, 0);  // Use any RGB because all the same value
    }

    // Fill in the remaining rows.
    for (int i = 1; i < height; i++) { // go in per column per row
        for (int j = 0; j < width; j++) {
            double pixel_grad = get_pixel(grad, i, j, 0); // get value of position of interest
            double min_cost = min_neighbors(*best_arr, i - 1, j, width); // get min of three positions above, i-1 = prev_row
            (*best_arr)[i * width + j] = pixel_grad + min_cost; // compute and STORE in *best_arr[] double pointer
        }
    }
}



/*Part 3: Recover the seam
Write a function

void recover_path(double *best, int height, int width, int **path);

This function allocates a path through the minimum seam as defined by the array best.

For the best array above, the path is [3, 4, 3, 2, 2].*/


// no helper req
// follows best/ cheapest seam path 
// takes in best pointer ??, dimensions, path pointer 
#include <stdlib.h>

// Helper function that returns the best column index for row 'row' given
// that the seam in the next row is at 'center'. It checks the three possible neighbors:
// left, center, and right.

static int find_best_neighbor(double *best_arr, int row, int center, int width) {
    int best_col = center; // set column to the centre 
    double min_cost = best_arr[row * width + center]; // set min cost to centre 

    // Check left neighbor if it exists.
    if (center - 1 >= 0) { // not at end
        double left_cost = best_arr[row * width + (center - 1)]; // get left value
        if (left_cost < min_cost) { // take min
            best_col = center - 1;
            min_cost = left_cost;
        }
    }
    
    // Check right neighbor if it exists. // same algorithm as above 
    if (center + 1 < width) {
        double right_cost = best_arr[row * width + (center + 1)];
        if (right_cost < min_cost) {
            best_col = center + 1;
        }
    }
    
    return best_col; // return best column to go with / path to follow next
}

void recover_path(double *best, int height, int width, int **path) {
    // Allocate space for the seam path (one column index per row)
    *path = (int *)malloc(sizeof(int) * height);

    // Start with the bottom row: find the column with the smallest cost.
    int min_index = 0; // set minimum to 0
    double min_cost = best[(height - 1) * width];  // First element in the bottom row
    for (int j = 1; j < width; j++) { // iterate each position
        double cost = best[(height - 1) * width + j]; // obtain cost
        if (cost < min_cost) { // reset set min each time
            min_cost = cost;
            min_index = j; // index of column 
             }
    }
   
    (*path)[height - 1] = min_index; // set first value of path to the starting position minimum

    // 2. Trace back upward from the bottom row.
    for (int i = height - 2; i >= 0; i--) { // move backward, start at height -2 to subtract bottom row and begin one above it (offset 1 by height)
        int prev_index = (*path)[i + 1]; // get rid of offset, get prev row
        // Use our helper function to choose the best neighbor in row i
        int best_j = find_best_neighbor(best, i, prev_index, width); // compute best col
        (*path)[i] = best_j; //send value to seam path array
    }
}


/*Part 4: Write a function that removes the seam from the best path
void remove_seam(struct rgb_img *src, struct rgb_img **dest, int *path);

The function creates the destination image, and writes to it the source image, with the seam removed.*/

// takes in original image,dest is new pic generated, path was just created 
void remove_seam(struct rgb_img *src, struct rgb_img **dest, int *path) {
    int height = src->height; // get info from struct
    int width = src->width;

    // Create the destination image with one less column.
    create_img(dest, height, width - 1);

    // Process each row.
    for (int i = 0; i < height; i++) { // each row

        // The column to remove in the current row.
        int seam_col = path[i]; 

        // new_j is the column index for the destination image -> need this because index is less by 1/ diff from original j
        for (int j = 0, new_j = 0; j < width; j++) {
            // Skip the pixel in the seam.
            if (j == seam_col) continue; // if we get to the seam path, dont include the pixel
            //ELSE
            // Copy the pixel (R, G, B) from original image at (i, j) to dest at (i, new_j).
            uint8_t r = get_pixel(src, i, j, 0);  //get pixel value at all 3 channels 
            uint8_t g = get_pixel(src, i, j, 1);
            uint8_t b = get_pixel(src, i, j, 2);
            set_pixel(*dest, i, new_j, r, g, b); //set pixel info in dest image
            new_j++;
        }
    }
}


int main(){

    struct rgb_img *im;
    struct rgb_img *cur_im;
    struct rgb_img *grad;
    double *best;
    int *path;

    read_in_img(&im, "HJoceanSmall.bin");
    
    for(int i = 0; i < 5; i++){
        printf("i = %d\n", i);
        calc_energy(im,  &grad);
        dynamic_seam(grad, &best);
        recover_path(best, grad->height, grad->width, &path);
        remove_seam(im, &cur_im, path);

        char filename[200];
        sprintf(filename, "img%d.bin", i);
        write_img(cur_im, filename);


        destroy_image(im);
        destroy_image(grad);
        free(best);
        free(path);
        im = cur_im;
    }
    destroy_image(im);
}



