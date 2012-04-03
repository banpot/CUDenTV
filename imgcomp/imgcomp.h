#include <cv.h>
#include <highgui.h>
#include <iostream>
#include <string.h>
#include <cstdio>
#include <omp.h>
#include <stdio.h>

#define IMGCOMP_PSNR 	1
#define IMGCOMP_SNR 	2
#define IMGCOMP_SSIM 	3
#define IMGCOMP_MSE 	4

#define IMGCOMP_PICNOTEQUAL 	-1
#define IMGCOMP_FILE1FAIL 	-2
#define IMGCOMP_FILE2FAIL 	-3

double **r1;
double **g1;
double **b1;
double **r2;
double **g2;
double **b2;
double mse, psnr, snr;
cv::Vec3b vec3b;
int height,width,height2,width2; 


float imgcomp(char* filename1, char* filename2, int CompareValueType){

// Debug
//printf("Filename 1: %s\nFilename 2: %s\n", filename1, filename2);

// Fetching filename 1

cv::Mat_<cv::Vec3b> img = cv::imread(filename1, 1);
//printf("Data size is (#cols, #rows) = (%d, %d)\n", img.cols, img.rows);
//cv::Mat_<int> labels(width * height, 1);
width = img.cols;
height = img.rows;

// 2D array allocation for image #1
r1 = (double**)malloc(height*sizeof(double *));
g1 = (double**)malloc(height*sizeof(double *));
b1 = (double**)malloc(height*sizeof(double *));

for (int i=0;i<height;i++){
r1[i] = (double*)malloc(height*sizeof(double));
g1[i] = (double*)malloc(height*sizeof(double));
b1[i] = (double*)malloc(height*sizeof(double));
}


for (int y = 0; y < img.rows; ++y) {
	for (int x = 0; x < img.cols; ++x)
	{
		vec3b = img(y, x);
		r1[y][x] = double( vec3b[0]);
		g1[y][x] = double( vec3b[1]);
		b1[y][x] = double( vec3b[2]);
	}
}


cv::Mat_<cv::Vec3b> img2 = cv::imread(filename2, 1);
//printf("Data size is (#cols, #rows) = (%d, %d)\n", img2.cols, img2.rows);
width2 = img2.cols;
height2 = img2.rows;

// 2D array allocation for image #2
r2 = (double**)malloc(height*sizeof(double *));
g2 = (double**)malloc(height*sizeof(double *));
b2 = (double**)malloc(height*sizeof(double *));

for (int i=0;i<height;i++){
r2[i] = (double*)malloc(height*sizeof(double));
g2[i] = (double*)malloc(height*sizeof(double));
b2[i] = (double*)malloc(height*sizeof(double));
}

// Image not equal = exit
if (width!=width2 || height!=height2) {printf("\nTwo images is not in equal size.\n");return(IMGCOMP_PICNOTEQUAL);}


for (int y = 0; y < img2.rows; ++y) {
	for (int x = 0; x < img2.cols; ++x)
	{
		vec3b = img2(y, x);
		r2[y][x] = double( vec3b[0]);
		g2[y][x] = double( vec3b[1]);
		b2[y][x] = double( vec3b[2]);
	}
}

// start computation
for (int i=0;i<height;i++){
for (int j=0;j<width;j++){
mse+=(r1[i][j] - r2[i][j])*(r1[i][j] - r2[i][j]);
mse+=(g1[i][j] - g2[i][j])*(g1[i][j] - g2[i][j]);
mse+=(b1[i][j] - b2[i][j])*(b1[i][j] - b2[i][j]);
}
}

mse/=(3*width*height);
psnr = 20 * log(255) - 10*log(mse);
if (CompareValueType==IMGCOMP_PSNR) {return (psnr);}
if (CompareValueType==IMGCOMP_MSE) {return (mse);}

} // End function
