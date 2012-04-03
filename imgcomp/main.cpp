#include "imgcomp.h"
#include <stdio.h>
#define IMGCOMP_PSNR 	1
#define IMGCOMP_SNR 	2
#define IMGCOMP_SSIM 	3
#define IMGCOMP_MSE 	4

#define IMGCOMP_PICNOTEQUAL 	-1
#define IMGCOMP_FILE1FAIL 	-2
#define IMGCOMP_FILE2FAIL 	-3

int main(int argc, char* argv[]){

if (argc < 2) {printf("use format:\nimgcomp filename1 filename2 [compare type]\n");exit(0);}

printf("PSNR : %f\n", imgcomp(argv[1], argv[2], IMGCOMP_PSNR));
printf("MSE : %f\n", imgcomp(argv[1], argv[2], IMGCOMP_MSE));

}
