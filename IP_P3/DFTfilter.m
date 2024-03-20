clear; close all;
%%
img_path = "project3/Knee.pgm";
img = imread(img_path);
[MM, NN] = size(img);

D0 = 10;

subplot(3, 3, 1)
imshow(img)
title('f(x,y)')
%% padding
padded_img = zeros(2*MM, 2*NN);
for i = 1:MM
    for j = 1:NN
        padded_img(i, j) = img(i, j);
    end
end
M = 2 * MM -1;
N = 2 * NN -1;
% scale to [0, 1]
padded_img = padded_img/255.;

subplot(3, 3, 2)
imshow(padded_img, [0,1])
title('f_p(x,y)')


%% shift
for x = 1:M
    for y = 1:N
         padded_img(x, y) = padded_img(x, y) * (-1)^(x+y);
    end
end
subplot(3, 3, 3)
imshow(padded_img, [0,1])
title('shifted f_p(x,y)')

%% DFT

% F = fft2(img);

P_real = zeros(M, N);
P_img = zeros(M, N);
for y = 1:N
    for u = 1:M
        for x = 1:M
            P_real(u, y) = P_real(u, y) + padded_img(x, y) * cos(2*pi*(u-1)*(x-1)/M) ;
            P_img(u, y) = P_img(u, y) - padded_img(x, y) * sin(2*pi*(u-1)*(x-1)/M);
        end
    end
end
% P = complex(P_real, P_img);
% figure(2)
% imshow(log10(abs(P)));

F_real  = zeros(M, N);
F_img   = zeros(M, N);
for x = 1:M
    for v = 1:N
        for y = 1:N
            % F(x, v) = F(x, v) + P(x, v) * cos(2*pi*(v-1)*(y-1)/N)  - 1j * P(x, v) * sin(2*pi*(v-1)*(y-1)/N);
            F_real(x, v)    = F_real(x, v) + P_real(x, y) * cos(2*pi*(v-1)*(y-1)/N) + P_img(x, y) * sin(2*pi*(v-1)*(y-1)/N) ;
            F_img(x, v)     = F_img(x, v) + P_img(x, y) * cos(2*pi*(v-1)*(y-1)/N) - P_real(x, y) * sin(2*pi*(v-1)*(y-1)/N);

        end
    end
end
F = complex(F_real, F_img);

subplot(3, 3, 4)
imshow(log10(abs(F)));
title('F(u, v)')
%% LPF
u = 0:(M-1);
v = 0:(N-1);
u = u - floor(M/2);
v = v - floor(N/2);
[U, V]  = ndgrid(u, v);
H       = double(sqrt(U.^2 + V.^2) <= D0);

subplot(3, 3, 5)
imshow(abs(H), [0,1]);
title('H(u, v)')

G_real  = H.*F_real;
G_img   = H.*F_img;
G       = complex(G_real, G_img);

subplot(3, 3, 6)
imshow(log10(abs(G)));
title('G(u, v)')
%% IDFT

% f_restored = ifft2(G);

p_real  = zeros(M, N);
p_img   = zeros(M, N);
for v = 1:N
    for x = 1:M
        for u = 1:M
            p_real(x, v)    = p_real(x, v) + G_real(u, v) * cos(2*pi*(u-1)*(x-1)/M) - G_img(u, v) * sin(2*pi*(u-1)*(x-1)/M) ;
            p_img(x, v)     = p_img(x, v) + G_real(u, v) * sin(2*pi*(u-1)*(x-1)/M) + G_img(u, v) * cos(2*pi*(u-1)*(x-1)/M);
        end
    end
end
f_real  = zeros(M, N);
f_img   = zeros(M, N);
for u = 1:M
    for y = 1:N
        for v = 1:N
          f_real(u, y)  = f_real(u, y) + p_real(u, v) * cos(2*pi*(v-1)*(y-1)/N) - p_img(u, v) * sin(2*pi*(v-1)*(y-1)/N) ;
          f_img(u, y)   = f_img(u, y) + p_img(u, v) * cos(2*pi*(v-1)*(y-1)/N) + p_real(u, v) * sin(2*pi*(v-1)*(y-1)/N);
        end
    end
end

f_real      = f_real / (M * N);
f_img       = f_img / (M * N);
f_restored  = (f_real + 1j*f_img);

subplot(3, 3, 7)
imshow(abs(f_restored))
title('shifted g_p(x,y)')
%% shift back
for x = 1:M
    for y = 1:N
         f_restored(x, y) = f_restored(x, y) * (-1)^(x+y);
    end
end

subplot(3, 3, 8)
imshow(abs(f_restored))
title('g_p(x,y)')
%% crop
f_restored = f_restored(1:MM, 1:NN);

subplot(3, 3, 9)
imshow(abs(f_restored))
title('g(x,y)')

%% save image
imwrite(uint8(abs(f_restored)*255), sprintf('LPF_%d.pgm', D0))