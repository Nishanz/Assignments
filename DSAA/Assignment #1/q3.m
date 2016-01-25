%% High-Pass Filtering

% Read image input
im=imread('cameraman.tif');

% Load convolution kernel
h = [-1 -2 -1; ...
      0  0  0; ... 
      1  2  1];

% Filter the image with the given filter
highpass = imfilter(im,h);

% Plot the results
figure;
subplot(1,2,1);
imshow(im,[]);
title('Original Image');
subplot(1,2,2);
imshow(highpass,[]);
title('Horizontal High-passed Image');

%% Comments
% The above filter is the Sobel Filter for Edge Detection
% It is an high-pass filter, but acts only along the horizontal direction,
% i.e. it is used as a horizontal edge detector
%
