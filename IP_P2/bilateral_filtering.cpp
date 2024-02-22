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
unsigned int filtermsk(float* msk, int kernal_size, int coor_x, int coor_y, float sigma_s);

int main(int argc, char **argv)
{
  FILE *fp;

  if (argc != 6){
    printf("Usage: MyProgram <input_ppm> <output_ppm>  <Kernal size> <sigmar> <sigmas> \n");
    printf("       <input_ppm>: PGM file \n");
    printf("       <output_ppm>: PGM file \n");
    printf("      <Kernal size>: kernal size \n");
    printf("      <sigmar>: sigma r\n");
    printf("      <sigmas>: sigma s\n");
    exit(0);              
  }

    // Load kernal_size sigma_r sigma_s
    int kernal_size = std::stof(argv[3]);
    printf("kernal_size: %d\n", kernal_size);
    float sigma_r = std::stof(argv[4]);
    printf("Sigma r: %.2f\n", sigma_r);
    float sigma_s = std::stof(argv[5]);
    printf("Sigma s: %.2f\n", sigma_s);


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

    // distance part of Mask

    // The algorithm computes the distance part first and tries the function part while
    // filtering
    // ignore the coefficient of the constant part of the equation
    // multiple 100 to make the values of kernal not vanish
      int center_coor = kernal_size/2;
      float Mask_sum = 0;
      float *Mask;
      Mask = (float*)malloc(sizeof(float)*kernal_size*kernal_size);
      for (int i = 0; i < kernal_size; i++) {
        for (int j = 0; j < kernal_size; j++) {
            // printf("%f, ", distance_squared);
            Mask[i * kernal_size + j] = 
              (100*(exp(-0.5 * (pow(abs(i - center_coor)/sigma_r, 2) + pow(abs(j - center_coor)/sigma_r, 2)))));
            Mask_sum += Mask[i * kernal_size + j];
            // printf("(%d, %d), %f, ", i, j, Mask[i * kernal_size + j]);
        }
      }

    // padding
    // add zeros for the boundary
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
            I_new[i * N + j] = filtermsk(Mask, kernal_size, i, j, sigma_s);
          }
      }

  // save img  
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

// filtering process + complete mask generation
unsigned int filtermsk(float* msk, int kernal_size, int coor_x, int coor_y, float sigma_s) {
    
    // get the complete mask with function part (Mask)
    float sum_value = 0;
    int pixel_intensity ,center_intensity;
    float Mask_sum = 0;
    float *Mask;
    Mask = (float*)malloc(sizeof(float)*kernal_size*kernal_size);

    for (int ii = 0; ii<kernal_size; ii++){
      for (int jj = 0; jj < kernal_size; jj++){

        int x_idx = coor_x + ii;
        int y_idx = coor_y + jj;
        float pixel_intensity = static_cast<float>(I[(x_idx+ii) * xdim + (y_idx+jj)]);
        float center_intensity = static_cast<float>(I[(coor_x + static_cast<int>(kernal_size/2)) * xdim + (coor_y + static_cast<int>(kernal_size/2))]);
        // printf("(%d, %d), %u %u, ", x_idx+ii, y_idx+jj, pixel_intensity, center_intensity);
        Mask[ii * kernal_size + jj] = msk[ii * kernal_size + jj]*
          expf(-0.5 * (pow(abs(pixel_intensity - center_intensity)/sigma_s, 2)));
        Mask_sum += Mask[ii * kernal_size + jj];
        // printf("(%d, %d), %.1f, ", ii, jj, Mask[ii * kernal_size + jj]);
      }              
    }
    // printf("Mask_sum %f\n", Mask_sum);

    // filtering (Mask & the patches)
    for (int i = 0; i < kernal_size; i++) {
        for (int j = 0; j < kernal_size; j++) {
            int x_idx = coor_x + i;
            int y_idx = coor_y + j; 
            float pixel_intensity = static_cast<float>(I[(x_idx+i) * xdim + (y_idx+j)]);
            float tmp = (pixel_intensity) * (Mask[i * kernal_size + j]) / Mask_sum;
            sum_value += tmp;
        }
    }
    return static_cast<unsigned int>(sum_value);
}