function resample(img_path, out_path, t)
    % img = imread('proj-1/MRI-brain.pgm');
    % t = 0.1;
    img = imread(img_path);
    [M, N] = size(img);
    M_p = round(M * t);
    N_p = round(N * t);
    I_new = zeros(M_p, N_p, 'uint8');
    
    for x = 1:M_p
        for y = 1:N_p
            x0 = floor(x / t);
            y0 = floor(y / t);
            x1 = x0 + 1;
            y1 = y0 + 1;
            
            x0 = max(1, min(x0, M - 1));
            x1 = max(1, min(x1, M));
            y0 = max(1, min(y0, N - 1));
            y1 = max(1, min(y1, N));
            
            x_ = max(0, (x/t - x0)/1);
            y_ = max(0, (y/t - y0)/1);
            % x_ = x/M_p;
            % y_ = y/N_p;
    
            I00 = img(x0, y0);
            I01 = img(x0, y1);
            I10 = img(x1, y0);
            I11 = img(x1, y1);
    
            T1 = y_ * I01 + (1-y_)*I00;
            T2 = y_ * I11 + (1-y_)*I10;
    
            I = x_ * T2 + (1 - x_) * T1;
            
            % I = y/N_p*x/M_p*I11 + x/M_p*(1-y/N_p)*I10+(1-x/M_p)*y/N_p*I01+(1-x/M_p)*(1-y/N_p)*I00;
            I_new(x, y) = uint8(I);
        end
    end
    % imshow(I_new)
    % psnr(imresize(img, t, 'bicubic'), I_new)
    % psnr(imresize(img, t, 'bicubic'), imresize(img, t, 'bilinear'))
    % outputFilename = sprintf('output_%d.png', t);
    outputFilename = out_path;
    imwrite(I_new, outputFilename);
end
    