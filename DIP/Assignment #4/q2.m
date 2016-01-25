%% Loading and Preprocessing

%Read images
im1=imread('yogasan/y1.jpg');
im2=imread('yogasan/y2.jpg');
im3=imread('yogasan/y3.jpg');
im4=imread('yogasan/y4.jpg');

% This is the input image, we can vary it to handle new asanas as well as 
% new photos of the same asana.
im = im2;
level = graythresh(im);
BW = imcomplement(im2bw(im,level));

% Resizing gives a fixed size input to give us input shape and size
% invariance.
im = imresize(im, [500 400]);

%% Proposed Algorithm - Scale, translation, colour, size, brightness invariance

% Calculate the bounding box and crop. This provides translation invariance
% in all axes. Rotation invariance is not required as it is always assumed
% that ground is below and images are always upright.
CC = bwconncomp(BW);
stats = regionprops(CC);
BW2=imcrop(BW,stats(1).BoundingBox);
BW3 = bwmorph(BW2,'remove');

%Resizing again provides with scale invariance.
BW3inp = imresize(BW3, [500 400]);

%Repeating the same procedure with all images.
level = graythresh(im1);
BW = imcomplement(im2bw(im1,level));
im1 = imresize(im1, [500 400]);
CC = bwconncomp(BW);
stats = regionprops(CC);
BW2=imcrop(BW,stats(1).BoundingBox);
BW3 = bwmorph(BW2,'remove');
BW31 = imresize(BW3, [500 400]);

level = graythresh(im2);
BW = imcomplement(im2bw(im2,level));
im2 = imresize(im2, [500 400]);
CC = bwconncomp(BW);
stats = regionprops(CC);
BW2=imcrop(BW,stats(1).BoundingBox);
BW3 = bwmorph(BW2,'remove');
BW32 = imresize(BW3, [500 400]);

level = graythresh(im3);
BW = imcomplement(im2bw(im3,level));
im3 = imresize(im3, [500 400]);
CC = bwconncomp(BW);
stats = regionprops(CC);
BW2=imcrop(BW,stats(1).BoundingBox);
BW3 = bwmorph(BW2,'remove');
BW33 = imresize(BW3, [500 400]);

level = graythresh(im4);
BW = imcomplement(im2bw(im4,level));
im4 = imresize(im4, [500 400]);
CC = bwconncomp(BW);
stats = regionprops(CC);
BW2=imcrop(BW,stats(1).BoundingBox);
BW3 = bwmorph(BW2,'remove');
BW34 = imresize(BW3, [500 400]);

mxval = 10000000;

% Calculating the RMSE difference between the resultant feature images.
if sum(sum(255*(abs(BW3inp(:,:)-BW31(:,:)))))<mxval
    mxval=sum(sum(255*(abs(BW3inp(:,:)-BW31(:,:)))));
    val = 1;
end

if sum(sum(255*(abs(BW3inp(:,:)-BW32(:,:)))))<mxval
    mxval=sum(sum(255*(abs(BW3inp(:,:)-BW32(:,:)))));
    val = 2;
end

if sum(sum(255*(abs(BW3inp(:,:)-BW33(:,:)))))<mxval
    mxval=sum(sum(255*(abs(BW3inp(:,:)-BW33(:,:)))));
    val = 3;
end

if sum(255*(sum(abs(BW3inp(:,:)-BW34(:,:)))))<mxval
    mxval=sum(255*(sum(abs(BW3inp(:,:)-BW34(:,:)))));
    val = 4;
end

disp(val);