%% Load and initialize Variables.
clc;clear;
tic;
im=single(imread('soy-dessert.jpg'));
im2=single(imread('vegan-modified.jpg'));
im=imresize(im,0.25);
im2=imresize(im2,0.25);
Nimg=size(im2);
Nfilt=size(im);
x=-1;y=-1;
mxval=9999999;

%% Sliding Window
for i=1:Nimg(1)-Nfilt(1)
    for j=1:Nimg(2)-Nfilt(2)
        % Take eucledian norm in each window and take the least eucledian
        % norm window, take the intial co-ordinates of the window.
        if sum(sum(abs(im2(i:i+Nfilt(1)-1,j:j+Nfilt(2)-1)-im(:,:))))<mxval
            mxval=sum(sum(abs(im2(i:i+Nfilt(1)-1,j:j+Nfilt(2)-1)-im(:,:))));
            x=i;y=j;
        end
    end
end

%% Show the result window and the original image.

i=x;j=y;
figure;imshow(uint8(im));
figure;imshow(uint8(im2(i:i+Nfilt(1)-1,j:j+Nfilt(2)-1)));
toc;
