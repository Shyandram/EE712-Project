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
using namespace std;

#define max(x, y) ((x>y) ? (x):(y))
#define min(x, y) ((x<y) ? (x):(y))


int xdim;
int ydim;
int xdim_new;
int ydim_new;
int maxraw;
unsigned char *image;
unsigned char *I_new;
float t;

void ReadPGM(FILE*);
void WritePGM(FILE*);


int main(int argc, char **argv)
{
  int i, j;
  FILE *fp;

  if (argc != 4){
    printf("Usage: MyProgram <input_ppm> <output_ppm> <scaling_factor> \n");
    printf("       <input_ppm>: PGM file \n");
    printf("       <output_ppm>: PGM file \n");
    printf("    <scaling_factor>: scaling_factor t \n");
    exit(0);              
  }

    // Load t scaling factor (resampling factor)
    t = std::stof(argv[3]);
    // printf("Scaling Factor: %.2f\n", t);
    // std::cout << "Scaling factor: " << t << std::endl;
    // printf("Enter the Factor: ");
    // scanf("%f", &t);
    if (t <= 0){
        t *= -1;
    }
    printf("Scaling Factor: %.2f\n", t);


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
    int M_p = static_cast<int>(std::round(M * t)); // new image height
    int N_p = static_cast<int>(std::round(N * t)); // new image width
    xdim_new = N_p;
    ydim_new = M_p;
  printf("Ouput Img Size:\n %d * %d\n", xdim_new, ydim_new);

#undef max
#undef min

// create empty re-sampled image
I_new = (unsigned char*)malloc(sizeof(unsigned char)*N_p*M_p);
// Bilinear interpolation
for (int x = 0; x < M_p; ++x) {
        for (int y = 0; y < N_p; ++y) {
            // the coordinates of the four neighboring points
            int x0 = static_cast<int>(std::floor(x / t));
            int y0 = static_cast<int>(std::floor(y / t));
            int x1 = x0 + 1;
            int y1 = y0 + 1;

            // Boundary settings
            x0 = std::max(0, std::min(x0, M - 1));
            x1 = std::max(0, std::min(x1, M - 1));
            y0 = std::max(0, std::min(y0, N - 1));
            y1 = std::max(0, std::min(y1, N - 1));

            // Get the grid (I00 ~ I11) coordinates of the x and y
            float x_ = (static_cast<float>(x)/t - x0)/1;
            float y_ = (static_cast<float>(y)/t - y0)/1;

            // pixel values of the four neighboring points
            unsigned char I00 = image[x0 * xdim + y0];
            unsigned char I01 = image[x0 * xdim + y1];
            unsigned char I10 = image[x1 * xdim + y0];
            unsigned char I11 = image[x1 * xdim + y1];

            // Bilinear interpolation process
            double T1 = y_ * I01 + (1 - y_) * I00;
            double T2 = y_ * I11 + (1 - y_) * I10;

            double I = x_ * T2 + (1 - x_) * T1;

            I_new[x * N_p + y] = static_cast<unsigned char>(std::round(I));
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
printf("%d, %d\n", xdim_new, ydim_new);
  fprintf(fp, "P5\n%d %d\n%d\n", xdim_new, ydim_new, 255);
  for (j=0; j<ydim_new; j++)
    for (i=0; i<xdim_new; i++) {
      fputc(I_new[j*xdim_new+i], fp);
    }

  fclose(fp);
  printf("Finish writing");
  
}
