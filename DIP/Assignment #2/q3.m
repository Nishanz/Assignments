%% Normal Image.
% Read, convert to fourier domain and display.
clc;clear;
tic;
im=single(imread('rupee-symbol.jpg'));
IM = mat2gray(log(abs(fftshift(fft2(im)))+1));
figure;imshow(IM,[]);

%% Scale = 50
scale = 2;

%% f[ x,y ] -> a*f[ x,y ]
%  We expect a change in the magnitude spectra
%  by the factor of 'a' 
%
% $$\ F(u,v) = \sum_{n=-\infty}^{\infty} \sum_{m=-\infty}^{\infty} f(m,n)e^{-j2\pi(\frac{um}{M}  + \frac{vn}{N})}$
%
% $$\ F'(u,v) = \sum_{n=-\infty}^{\infty} \sum_{m=-\infty}^{\infty} a*f(m,n)e^{-j2\pi(\frac{um}{M}  + \frac{vn}{N})}$
%
% $$\ F'(u,v) = a*(\sum_{n=-\infty}^{\infty} \sum_{m=-\infty}^{\infty} f(m,n)e^{-j2\pi(\frac{um}{M}  + \frac{vn}{N})})$
%
% $$\ F'(u,v) = a*F(u,v)$ 
%
im2=scale*im;
IM2 = mat2gray(log(abs(fftshift(fft2(im2)))+1));
figure;imshow(IM2,[]);

%% f[ x,y ] -> f[ a*x,a*y ]
% We resize the image by a factor of 'a'.

im3=imresize(im,scale);
IM3 = mat2gray(log(abs(fftshift(fft2(im3)))+1));
figure;imshow(IM3,[]);

%% f[ x,y ] -> f[ x+x0,y+y0 ]
% We expect no change in the magnitude spectra of the DFT becuase of the
% circular shift property
%
% $$\ F(u,v) = \sum_{n=-\infty}^{\infty} \sum_{m=-\infty}^{\infty} f(m,n)e^{-j2\pi(\frac{um}{M}  + \frac{vn}{N})}$
%
% For the shifted image, say f',
%
% $$\ F'(u,v) = \sum_{n=-\infty}^{\infty} \sum_{m=-\infty}^{\infty} f'(m,n)e^{-j2\pi(\frac{um}{M}  + \frac{vn}{N})}$
%
% $$\ F'(u,v) = \sum_{n=-\infty}^{\infty} \sum_{m=-\infty}^{\infty} f(m+m_{0},n+n_{0})e^{-j2\pi(\frac{u(m+m_{0})}{M} + \frac{v(n+n_{0})}{N})}$
% 
% $$\ F'(u,v) = (\sum_{n=-\infty}^{\infty} \sum_{m=-\infty}^{\infty} f(m,n)e^{-j2\pi(\frac{um}{M}  + \frac{vn}{N})})e^{-j2\pi(\frac{um_{0}}{M} + \frac{vn_{0}}{N})}$
%
% $$\ F'(u,v) = F(u,v)e^{-j2\pi(\frac{um_{0}}{M} + frac{vn_{0}}{N})}$
%
% $$\ | F'(u,v) | = | F(u,v) |$

% let m0 and n0 be 5 and 8 repsectively (any value may be taken)
im4=imtranslate(im,[scale, scale, scale]);
IM4 = mat2gray(log(abs(fftshift(fft2(im4)))+1));
figure;imshow(IM4,[]);

%% f[ x,y ] -> f[ x, M-y ]
%
% *The DFT is expected to be reflected along the vertical line too.*
%
% $$\ F(u,v) = \sum_{n=-\infty}^{\infty} \sum_{m=-\infty}^{\infty} f(m,n)e^{-j2\pi(\frac{um}{M}  + \frac{vn}{N})}$
%
% $$\ F'(u,v) = \sum_{n=-\infty}^{\infty} \sum_{m=-\infty}^{\infty} f(m,N-n)e^{-j2\pi(\frac{um}{M}  + \frac{v(N - n)}{N})}$
%
for i=1:size(im,3)
    im5(:,:,i)=fliplr(im(:,:,i));
end
IM5 = mat2gray(log(abs(fftshift(fft2(im5)))+1));
figure;imshow(IM5,[]);

%% Difference between scaled and original image.
figure;imshow(abs(IM2-IM),[]);

%% Difference between resized and original image.
%figure;imshow(abs(IM3-IM),[]);

%% Difference between translated and original image.
figure;imshow(abs(IM4-IM),[]);

%% Difference between flipped and original image.
figure;imshow(abs(IM5-IM),[]);
toc;
