%% By Spatial Transforms

im = imread('octone.gif');
im=im2bw(im,0.4);
se = strel('disk',3);
im=imdilate(im,se);
imshow(im);

%% By Fourier Domain Transforms

im = imread('octone.gif');
