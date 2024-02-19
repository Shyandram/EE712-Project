function I_new = gaussian_filtering(img_path, out_path, kernal_size, sigma)
    %% load image
    % img = imread('proj_2/test-img.pgm');
    img = imread(img_path);

    [M, N] = size(img);
    I_new = zeros(M, N, "uint8");
    %% mask
    % kernal_size = 5;
    % sigma = 1;
    mask = zeros(kernal_size, kernal_size);
    center_coor = floor(kernal_size/2)+1;
    for i = 1:kernal_size
        for j = 1:kernal_size
            % Gaussian
            mask(i, j) = exp(-(((i-center_coor)/sigma)^2/2+((j-center_coor)/sigma)^2/2));
        end
    end
    % Make the weights sum to 1
    mask = mask/sum(mask, "all");
    
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
            I_new(i, j) = filtermsk(I(i:i+kernal_size-1, j:j+kernal_size-1), mask);
        end
    end

    %% show original and filtered images
    % figure(1);
    % imshow(img);
    % figure(2);
    % imshow(I_new);

    %% save img
    imwrite(I_new, out_path);
end

function y = filtermsk(x, msk)
    y = uint8(sum(x.*msk, "all"));
end