function I_new = bilateral_filtering(img_path, out_path, kernal_size, sigma_s, sigma_r)
    %% load image
    % img = imread('proj_2/test-img.pgm');
    img = imread(img_path);

    [M, N] = size(img);
    I_new = zeros(M, N, "uint8");
    %% mask
    % The algorithm computes the distance part first and tries the function part while
    % filtering
    
    % kernal_size = 5;
    % sigma = 1;
    mask = zeros(kernal_size, kernal_size);
    center_coor = floor(kernal_size/2)+1;

    % distance part
    for i = 1:kernal_size
        for j = 1:kernal_size
            % Gaussian
            mask(i, j) = exp(-(((i-center_coor)/sigma_s)^2/2+((j-center_coor)/sigma_s)^2/2));
        end
    end
    %% padding
    % add zeros for the boundary
    I = zeros(M+(floor(kernal_size/2))*2, N+(floor(kernal_size/2))*2);
    for i = 1:M
        for j = 1:N
            I(i+ (floor(kernal_size/2)), j+ (floor(kernal_size/2))) = img(i , j );
        end
    end
    %% filtering
    for i = 1:M
        for j = 1:N
            % complete the mask with function part and do the filtering
            patch = I(i:i+kernal_size-1, j:j+kernal_size-1);
            I_new(i, j) = filtermsk(patch, mask, patch(center_coor, center_coor), sigma_r);
        end
    end

    %% show original and filtered images
    % figure(1);
    % imshow(img);
    % figure(3);
    % imshow(I_new);

    %% save img
    imwrite(I_new, out_path);
end

%% filtering process + complete mask generation
function y = filtermsk(x, msk, mid_f, sigma_r)
    % get the complete mask with function part
    msk = msk.* exp(-(((x-mid_f)./sigma_r).^2/2));
    % Make the weights sum to 1
    msk = msk/sum(msk, "all");
    
    % filtering
    y = uint8(sum(x.*msk, "all"));
end