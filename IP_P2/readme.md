# Project 2: 2D Image Bilateral Filtering
CS/EE-712: Image Processing Spring 2024  
Student: Shyang-En Weng  
Programming Language: MATLAB, C++

## Code explaination
### Gaussian Filter
1. Read the input image specified by img_path.
2. Define the sigma_s
3. Generate the mask based on 2-D gaussian, and normalize the sum of the weights to 1 (For C++. normalization will be activate during the filtering)
4. increase the reference image with zero padding (size: M by N to M+floor(kernal_size/2)*2 by N+floor(kernal_size/2)*2) for the boundary
5. do the filtering by sum of pixelwise multiplication of patch and mask for each pixel (in function "filtermsk")
6. save image
### Bilateral Filter
1. Read the input image specified by img_path.
2. Define the sigma_s and sigma_r
3. Generate the distance part of mask based on 2-D gaussian 
4. increase the reference image with zero padding (size: M by N to M+floor(kernal_size/2)*2 by N+floor(kernal_size/2)*2) for the boundary
5. complete the mask with function part and normalize the sum of the weights to 1 (in function "filtermsk") (For C++. normalization will be activate during the filtering)
6. do the filtering by sum of pixelwise multiplication of patch and mask for each pixel (in function "filtermsk")
7. save image
## Usage
### MATLAB Code
```
% gaussian_filtering(img_path, out_path, kernal_size sigma);
gaussian_filtering('test-img.pgm', 'gaussian_img.pgm', 5, 2);
% bilateral_filtering(img_path, out_path, kernal_size, sigma_s, sigma_r);
bilateral_filtering('test-img.pgm', 'bilateral_img.pgm', 5, 18, 30);
```
### C++ Code
```
// gaussian_filtering <input_ppm> <output_ppm> <Kernal size> <sigma>
gaussian_filtering test-img.pgm gaussian_img_5_18.pgm 5 18
// bilateral_filtering <input_ppm> <output_ppm> <Kernal size> <sigmar> <sigmas>
bilateral_filtering test-img.pgm bilateral_img_5_18_30.pgm 5 18 30
```

## Attachments
1. readme.md (readme file)
2. gaussian_filtering.m & gaussian_filtering.cpp
3. bilateral_filtering.m & bilateral_filtering.cpp
4. gaussian_img_5_18.pgm(output images, kernel: 5x5, sigma_s = 18)
5. bilateral_img_5_18_30.pgm (output images, kernel: 5x5, sigma_s = 18, sigma_r = 30)
6. test-img.pgm (input image)