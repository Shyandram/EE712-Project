# Project 1: Digital Image Re-Sampling
CS/EE-712: Image Processing Spring 2024  
Student: Shyang-En Weng  
Programming Language: MATLAB, C++

## Code explaination
1. Read the input image specified by img_path.
2. Define the resampling factor t (t > 0)
3. Create a new empty image I_new with (M' (M_p) and N' (N_p)) using the resampling factor.
4. Use the input image coordinate as reference and define the four neighboring pixels (I00, I01, I10, I11), and the position of Ixy in this grid (x_, y_).  
(Slightly different from the lecture slides. My algorithm defines the I00 as the upper left corner instead of lower left corner, but the idea remains the same.)
5. Iterate through each pixel in the new image with bilinear interpolation to estimate the pixel value based on the surrounding pixels in the original image.  
```
T1 = y_ * I01 + (1 - y_) * I00;
T2 = y_ * I11 + (1 - y_) * I10;
I = x_ * T2 + (1 - x_) * T1;
```
6. Write the resampled image to an output file.

## Usage
### C++ Code
```
// compile the resample.cpp first
// resample <input_ppm> <output_ppm> <scaling_factor>
resample MRI-brain.pgm out-3.pgm 3
resample MRI-brain.pgm out-0.3.pgm 0.3
```
### MATLAB Code
```
% resample(img_path, out_path, resampling_factor);
resample('MRI-brain.pgm', 'out-3.bmp', 3)
resample('MRI-brain.pgm', 'out-3.bmp', 0.3)
```

## Attachments
1. readme.md (readme file)
2. resample.cpp (code) & resample (executable file)
3. resample.m (code)
4. out-0.3.pgm (output images, t = 0.3)
5. out-3.pgm (output images, t = 3)
6. MRI-brain.pgm (input image)