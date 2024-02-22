/********************************************************
***IMPORTANT NOTE***
If you have problems with the provided sample code,
part of the reason might be due to the function "fopen".
Please try changing "r/w" to "rb/wb" or the other way
when you use this function.
*********************************************************/

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/malloc.h>  
#include <memory.h>
#include <numeric>
using namespace std;

#define max(x, y) ((x>y) ? (x):(y))
#define min(x, y) ((x<y) ? (x):(y))


int xdim;
int ydim;
int maxraw;
unsigned char *image;
unsigned char *I;
unsigned char *I_new;
float sigma;

void ReadPGM(FILE*);
void WritePGM(FILE*);
// unsigned int filtermsk(unsigned char* , unsigned char* , int , unsigned int );
// unsigned char* slice_patch(unsigned char* , unsigned int , unsigned int , int );
unsigned int filtermsk(float* msk, int kernal_size, unsigned int mask_sum, int coor_x, int coor_y);

int main(int argc, char **argv)
{
  FILE *fp;

  if (argc != 5){
    printf("Usage: MyProgram <input_ppm> <output_ppm>  <Kernal size> <sigma> \n");
    printf("       <input_ppm>: PGM file \n");
    printf("       <output_ppm>: PGM file \n");
    printf("      <Kernal size>: kernal size \n");
    printf("      <sigma>: sigma \n");
    exit(0);              
  }

    // Load kernal_size Sigma
    int kernal_size = std::stof(argv[3]);
    printf("kernal_size: %d\n", kernal_size);
    sigma = std::stof(argv[4]);
    if (sigma <= 0){
        sigma *= -1;
    }
    printf("Sigma: %.2f\n", sigma);


  /* begin reading PGM.... */
  printf("begin reading PGM.... \n");
  if ((fp=fopen(argv[1], "rb"))==NULL){
    printf("read error...\n");
    exit(0);
  }
  ReadPGM(fp);

    
  // your application here 
      
    int M = ydim; // Your image height
    int N = xdim; // Your image width   

    // Mask
    // ignore the constant part of the equation (only exp part)
    // multiple 100 to make the values of kernal not vanish
      int center_coor = kernal_size/2;
      float Mask_sum = 0;
      float *Mask;
      Mask = (float*)malloc(sizeof(float)*kernal_size*kernal_size);
      for (int i = 0; i < kernal_size; i++) {
        for (int j = 0; j < kernal_size; j++) {
            Mask[i * kernal_size + j] = 
              (100*(exp(-0.5 * (pow(abs(i - center_coor)/sigma, 2) + pow(abs(j - center_coor)/sigma, 2)))));
            Mask_sum += Mask[i * kernal_size + j];
            // printf("(%d, %d), %f, ", i, j, Mask[i * kernal_size + j]);
        }
      }
      // printf("Mask sum = %f\n", Mask_sum);

    // padding
      unsigned int M_pad = M+center_coor*2;
      unsigned int N_pad = N+center_coor*2;
      I = (unsigned char*)malloc(sizeof(unsigned char)*N_pad*M_pad);
      for (int i = 0; i < M; i++) {
          for (int j = 0; j < N; j++) {
            I[(i + center_coor) * N + (j + center_coor)] = image[i*N + j];
          }
      }      
    
    // create empty image
      I_new = (unsigned char*)malloc(sizeof(unsigned char)*N*M);
    // Filtering
      for (int i = 0; i < M; i++) {
          for (int j = 0; j < N; j++) {            
            I_new[i * N + j] = filtermsk(Mask, kernal_size, Mask_sum, i, j);
          }
      }
  
  /* Begin writing PGM.... */
  printf("Begin writing PGM.... \n");
  if ((fp=fopen(argv[2], "wb")) == NULL){
     printf("write pgm error....\n");
     exit(0);
   }
  WritePGM(fp);

  free(image);
  free(I);
  free(I_new);

  return (1);
}



void ReadPGM(FILE* fp)
{
    int c;
    int i,j;
    int val;
    unsigned char *line;
    char buf[1024];


    while ((c=fgetc(fp)) == '#')
        fgets(buf, 1024, fp);
     ungetc(c, fp);
     if (fscanf(fp, "P%d\n", &c) != 1) {
       printf ("read error ....");
       exit(0);
     }
     if (c != 5 && c != 2) {
       printf ("read error ....");
       exit(0);
     }

     if (c==5) {
       while ((c=fgetc(fp)) == '#')
         fgets(buf, 1024, fp);
       ungetc(c, fp);
       if (fscanf(fp, "%d%d%d",&xdim, &ydim, &maxraw) != 3) {
         printf("failed to read width/height/max\n");
         exit(0);
       }
       printf("Width=%d, Height=%d \nMaximum=%d\n",xdim,ydim,maxraw);

       image = (unsigned char*)malloc(sizeof(unsigned char)*xdim*ydim);
       getc(fp);

       line = (unsigned char *)malloc(sizeof(unsigned char)*xdim);
       for (j=0; j<ydim; j++) {
          fread(line, 1, xdim, fp);
          for (i=0; i<xdim; i++) {
            image[j*xdim+i] = line[i];
         }
       }
       free(line);

     }

     else if (c==2) {
       while ((c=fgetc(fp)) == '#')
         fgets(buf, 1024, fp);
       ungetc(c, fp);
       if (fscanf(fp, "%d%d%d", &xdim, &ydim, &maxraw) != 3) {
         printf("failed to read width/height/max\n");
         exit(0);
       }
       printf("Width=%d, Height=%d \nMaximum=%d,\n",xdim,ydim,maxraw);

       image = (unsigned char*)malloc(sizeof(unsigned char)*xdim*ydim);
       getc(fp);

       for (j=0; j<ydim; j++)
         for (i=0; i<xdim; i++) {
            fscanf(fp, "%d", &val);
            image[j*xdim+i] = val;
         }

     }

     fclose(fp);
}

void WritePGM(FILE* fp)
{
  int i,j;
  
//   fprintf(fp, "P5\n%d %d\n%d\n", xdim, ydim, 255);
//   for (j=0; j<ydim; j++)
//     for (i=0; i<xdim; i++) {
//       fputc(image[j*xdim+i], fp);
//     }
printf("%d, %d\n", xdim, ydim);
  fprintf(fp, "P5\n%d %d\n%d\n", xdim, ydim, 255);
  for (j=0; j<ydim; j++)
    for (i=0; i<xdim; i++) {
      fputc(I_new[j*xdim+i], fp);
    }

  fclose(fp);
  printf("Finish writing");
  
}

unsigned int filtermsk(float* msk, int kernal_size, unsigned int mask_sum, int coor_x, int coor_y) {
    float sum_value = 0;
    for (int i = 0; i < kernal_size; i++) {
        for (int j = 0; j < kernal_size; j++) {
            int x_idx = coor_x + i;
            int y_idx = coor_y + j;
            float tmp = static_cast<float>(I[x_idx * xdim + y_idx]) * (msk[i * kernal_size + j]);
            sum_value += tmp;
        }
    }
    return static_cast<unsigned int>(sum_value / mask_sum);
}