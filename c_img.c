#include "c_img.h"
#include <stdio.h>
#include <math.h>

// takes in image double pointer, height and width of image 
// allocates memory for image, sets dimensions
//allocates raster RGB per pixel in bytes 
void create_img(struct rgb_img **im, size_t height, size_t width){
    *im = (struct rgb_img *)malloc(sizeof(struct rgb_img));
    (*im)->height = height;
    (*im)->width = width;
    (*im)->raster = (uint8_t *)malloc(3 * height * width);
}


// reads in 2 bytes from a BIN file and store as integer value 
int read_2bytes(FILE *fp){
    uint8_t bytes[2];
    fread(bytes, sizeof(uint8_t), 1, fp);
    fread(bytes+1, sizeof(uint8_t), 1, fp);
    return (  ((int)bytes[0]) << 8)  + (int)bytes[1];
}

//writes an integer as 2 bytes in a BIN file 
void write_2bytes(FILE *fp, int num){
    uint8_t bytes[2];
    bytes[0] = (uint8_t)((num & 0XFFFF) >> 8);
    bytes[1] = (uint8_t)(num & 0XFF);
    fwrite(bytes, 1, 1, fp);
    fwrite(bytes+1, 1, 1, fp);
}


/*Reads an image from a BIN file (the format produced by the Python
 conversion script) and creates an image structure.
 
assigns the image through the pointer im.*/
// returned pointer to image 
void read_in_img(struct rgb_img **im, char *filename){
    FILE *fp = fopen(filename, "rb");
    size_t height = read_2bytes(fp);
    size_t width = read_2bytes(fp);
    create_img(im, height, width);
    fread((*im)->raster, 1, 3*width*height, fp);
    fclose(fp);
}

// writes image data directly to the BIn file in the expected output format 
// returns nothing  
void write_img(struct rgb_img *im, char *filename){
    FILE *fp = fopen(filename, "wb");
    write_2bytes(fp, im->height);
    write_2bytes(fp, im->width);
    fwrite(im->raster, 1, im->height * im->width * 3, fp);
    fclose(fp);
}

//gets RGB value of (x,y) at channel req;
// colour == 0 → the red channel.

//colour == 1 → the green channel.

//colour == 2 → the blue channel. 
// calculate index raster 
// return type uint8_t - VALUE OF THE COLOUR 0-255 
uint8_t get_pixel(struct rgb_img *im, int y, int x, int colour){ // get colour value
    return im->raster[3 * (y*(im->width) + x) + colour];
}

// SET RGB value at a given location 
void set_pixel(struct rgb_img *im, int y, int x, int r, int g, int b){
    im->raster[3 * (y*(im->width) + x) + 0] = r;
    im->raster[3 * (y*(im->width) + x) + 1] = g;
    im->raster[3 * (y*(im->width) + x) + 2] = b;
}

//free memory once done 
void destroy_image(struct rgb_img *im)
{
    free(im->raster);
    free(im);
}

// print gradient energy 
void print_grad(struct rgb_img *grad){
    int height = grad->height;
    int width = grad->width;
    for(int i = 0; i < height; i++){
        for(int j = 0; j < width; j++){
            printf("%d\t", get_pixel(grad, i, j, 0));
        }
    printf("\n");    
    }
}