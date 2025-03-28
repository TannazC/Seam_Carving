#if !defined(SEAMCARVING_H)
#define SEAMCARVING_H
#include "c_img.h"

// Calculates the dual-gradient energy for each pixel and 
//stores the result in a new image structure.
void calc_energy(struct rgb_img *im, struct rgb_img **grad);

//Uses dynamic programming to build a cost array where each 
//entry represents the minimum cumulative energy to reach that pixel from the top of the image.
void dynamic_seam(struct rgb_img *grad, double **best_arr);

//Recovers the minimum-energy seam by backtracking from the bottom of the cost array, returning 
//the column indices that define the seam.
void recover_path(double *best, int height, int width, int **path);

//Creates a new image by removing the identified seam from the source image.
void remove_seam(struct rgb_img *src, struct rgb_img **dest, int *path);

#endif