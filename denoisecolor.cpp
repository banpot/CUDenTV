// g++ -I/usr/include/opencv -lopencv_core -lopencv_highgui -lopencv_ml -lm denoisecolor.cpp

#include <cv.h>
#include <highgui.h>
#include <iostream>
#include <string.h>
#include <cstdio>

using namespace std;

#define ALPHA 6
#define MAXITE 30
#define BETA 0.0001
#define h 1
#define FILENAME "nitz.jpg"
#define NOISELEVEL 0.00

double z_r[2000][2000];
double z_g[2000][2000];
double z_b[2000][2000];

double r[2000][2000];
double g[2000][2000];
double b[2000][2000];

double ux_r[2000][2000];
double ux_g[2000][2000];
double ux_b[2000][2000];

double uy_r[2000][2000];
double uy_g[2000][2000];
double uy_b[2000][2000];

double d_r[2000][2000];
double d_g[2000][2000];
double d_b[2000][2000];

double dd_r[2000][2000];
double dd_g[2000][2000];
double dd_b[2000][2000];
///double** r,g,b;  

int width;
int height;

double U_value(int y, int x, int mode){
if (mode==0){
if (x > width-1) {return r[y][width-1];}
if (y > height-1) {return r[height-1][x];}
if (y < 0) {return r[0][x];}
if (x < 0) {return r[y][0];}
return r[y][x];
}
if (mode==1){
if (x > width-1) {return g[y][width-1];}
if (y > height-1) {return g[height-1][x];}
if (y < 0) {return g[0][x];}
if (x < 0) {return g[y][0];}
return g[y][x];
}
if (mode==2){
if (x > width-1) {return b[y][width-1];}
if (y > height-1) {return b[height-1][x];}
if (y < 0) {return b[0][x];}
if (x < 0) {return b[y][0];}
return b[y][x];
}
}

double D_value(int y, int x, int mode){
if (mode==0){
if (x > width-1) {return d_r[y][width-1];}
if (y > height-1) {return d_r[height-1][x];}
if (y < 0) {return d_r[0][x];}
if (x < 0) {return d_r[y][0];}
return d_r[y][x];
}
if (mode==1){
if (x > width-1) {return d_g[y][width-1];}
if (y > height-1) {return d_g[height-1][x];}
if (y < 0) {return d_g[0][x];}
if (x < 0) {return d_g[y][0];}
return d_g[y][x];
}
if (mode==2){
if (x > width-1) {return d_b[y][width-1];}
if (y > height-1) {return d_b[height-1][x];}
if (y < 0) {return d_b[0][x];}
if (x < 0) {return d_b[y][0];}
return d_b[y][x];
}
}

float gennoise(){

/* random number in range 0 - 1 not including 1 */
float random = 0;

/* the white noise */
float noise = 0;

long int q = 15;
float c1 = (1 << q) - 1;
float c2 = ((int)(c1 / 3)) + 1;
float c3 = 1.f / c1;


for (int i = 0; i < 15; i++)
{
    random = ((float)rand() / (float)(RAND_MAX));
    noise = (2 * ((random * c2) + (random * c2) + (random * c2)) - 3.f * (c2 - 1.f)) * c3;
}

return noise;

}



int main(){
	string pPath;
	pPath = getenv ("HOME");
	cv::Mat_<cv::Vec3b> img = cv::imread(FILENAME, 1);
	printf("Data size is (#cols, #rows) = (%d, %d)\n", img.cols, img.rows);
	cv::Mat_<int> labels(width * height, 1);
	width = img.cols;
	height = img.rows;

// Create and populate color matrix
	cv::Mat_<float> color_matrix(width*height, 3);
	cv::Vec3b vec3b;
	int index = 0;
	for (int y = 0; y < img.rows; ++y) {
		for (int x = 0; x < img.cols; ++x)
		{
			vec3b = img(y, x);
			r[y][x] = float( vec3b[0]);
			g[y][x] = float( vec3b[1]);
			b[y][x] = float( vec3b[2]);

			//z_r[y][x] = float( vec3b[0]);
			//z_g[y][x] = float( vec3b[1]);
			//z_b[y][x] = float( vec3b[2]);

			r[y][x] += (gennoise()*NOISELEVEL*255);	
			if (r[y][x]>255){r[y][x]=255;}
			if (r[y][x]<0){r[y][x]=0;}
			z_r[y][x] = r[y][x];


			g[y][x] += (gennoise()*NOISELEVEL*255);	
			if (g[y][x]>255){g[y][x]=255;}
			if (g[y][x]<0){g[y][x]=0;}
			z_g[y][x] = g[y][x];


			b[y][x] += (gennoise()*NOISELEVEL*255);	
			if (b[y][x]>255){b[y][x]=255;}
			if (b[y][x]<0){b[y][x]=0;}
			z_b[y][x] = b[y][x];

		}
	}

IplImage *imgRGB = cvLoadImage(FILENAME); 
CvScalar s;

for (int i=0;i<height;i++){
for (int j=0;j<width;j++){

s=cvGet2D(imgRGB,i,j); // get the (i,j) pixel value
//printf("B=%f, G=%f, R=%f\n",s.val[0],s.val[1],s.val[2]);
s.val[0]=r[i][j];
s.val[1]=g[i][j];
s.val[2]=b[i][j];
cvSet2D(imgRGB,i,j,s); // set the (i,j) pixel value
}
}
cvSaveImage("noisy.jpg", imgRGB);



 for (int ite=0;ite<MAXITE;ite++){

printf("=== Iteration #%d ===\n", ite);



// compute Ux, Uy
printf("Computing Ux, Uy ...\n");
	for (int y = 0; y < img.rows; ++y) {
		for (int x = 0; x < img.cols; ++x)
		{
			ux_r[y][x] = U_value(y,x,0) - U_value(y,x+1,0);
			ux_g[y][x] = U_value(y,x,1) - U_value(y,x+1,1);
			ux_b[y][x] = U_value(y,x,2) - U_value(y,x+1,2);

			uy_r[y][x] = U_value(y,x,0) - U_value(y+1,x,0);
			uy_g[y][x] = U_value(y,x,1) - U_value(y+1,x,1);
			uy_b[y][x] = U_value(y,x,2) - U_value(y+1,x,2);
		}
	}

// compute D, DIV, DDUX, DDUY
printf("Computing D ...\n");
	for (int y = 0; y < img.rows; ++y) {
		for (int x = 0; x < img.cols; ++x)
		{
		d_r[y][x] = 1/sqrt(ux_r[y][x]*ux_r[y][x] + uy_r[y][x]*uy_r[y][x] + BETA);
		d_g[y][x] = 1/sqrt(ux_g[y][x]*ux_g[y][x] + uy_g[y][x]*uy_g[y][x] + BETA);
		d_b[y][x] = 1/sqrt(ux_b[y][x]*ux_b[y][x] + uy_b[y][x]*uy_b[y][x] + BETA);
		}
	}

//printf("%f->%f\n%f->%f\n%f->%f \n", z_r[10][10],r[10][10] , z_g[10][10],g[10][10], z_b[10][10],b[10][10]);

// GS
printf("Computing Gauss-Seidel ...\n");
   for (int k = 0; k < 10; ++k) {
printf("Iteration # %d...\n", k);
	for (int y = 0; y < img.rows; ++y) {
		for (int x = 0; x < img.cols; ++x)
		{
		r[y][x] = z_r[y][x] + (ALPHA/(h*h))*(D_value(y,x,0)*U_value(y+1,x,0) + D_value(y,x,0)*U_value(y,x+1,0) + D_value(y-1,x,0)*U_value(y-1,x,0) + D_value(y,x-1,0)*U_value(y,x-1,0) );
		g[y][x] = z_g[y][x] + (ALPHA/(h*h))*(D_value(y,x,1)*U_value(y+1,x,1) + D_value(y,x,1)*U_value(y,x+1,1) + D_value(y-1,x,1)*U_value(y-1,x,1) + D_value(y,x-1,1)*U_value(y,x-1,1) );
		b[y][x] = z_b[y][x] + (ALPHA/(h*h))*(D_value(y,x,2)*U_value(y+1,x,2) + D_value(y,x,2)*U_value(y,x+1,2) + D_value(y-1,x,2)*U_value(y-1,x,2) + D_value(y,x-1,2)*U_value(y,x-1,2) );


r[y][x] = r[y][x]/(1 + (ALPHA/(h*h))*(2*D_value(y,x,0) + D_value(y-1,x,0) + D_value(y,x-1,0)));
g[y][x] = g[y][x]/(1 + (ALPHA/(h*h))*(2*D_value(y,x,1) + D_value(y-1,x,1) + D_value(y,x-1,1)));
b[y][x] = b[y][x]/(1 + (ALPHA/(h*h))*(2*D_value(y,x,2) + D_value(y-1,x,2) + D_value(y,x-1,2)));
//		r[y][x] = r[y][x]/(1+)
		}
	}
}

printf("Writing Picture ...\n");
printf("%f->%f\n%f->%f\n%f->%f \n", z_r[10][10],r[10][10] , z_g[10][10],g[10][10], z_b[10][10],b[10][10]);
// Set pixel


IplImage *imgRGB = cvLoadImage(FILENAME); 
CvScalar s;

for (int i=0;i<height;i++){
for (int j=0;j<width;j++){

s=cvGet2D(imgRGB,i,j); // get the (i,j) pixel value
//printf("B=%f, G=%f, R=%f\n",s.val[0],s.val[1],s.val[2]);
s.val[0]=r[i][j];
s.val[1]=g[i][j];
s.val[2]=b[i][j];
cvSet2D(imgRGB,i,j,s); // set the (i,j) pixel value
}
}

cvSaveImage("woman_4_clusters.jpg", imgRGB);

} // End MAIN FOR



	// Write the output to a file
	//cv::imwrite("woman_4_clusters.jpg", result);


/*
vec3b = img(1, 2);
int r_1 = float( vec3b[0] );
int g_1 = float( vec3b[1] );
int b_1 = float( vec3b[2] );

printf("%d %d %d", r_1,g_1,b_1 );
*/


//	printf("Data type = %d, %d\n", color_matrix.type(), CV_32F);

	//cv::Mat_<cv::Vec3b> result(height, width);
	//result.setTo(cv::Scalar(0));

}
