%% Evaluating the Level in Bottles

%% Preprocessing
imb = imread('bottles.tif');

% Resizing gives invariance to different sized images.
imb = imresize(imb,[350 500]);
BW = im2bw(imb,0.8);

%Remove the unneccesary noise left by binary conversion
BW2 = bwareaopen(BW,100);figure;imshow(BW2);

%% Region Comparison - A very effective, but amazingly simple algorithm

% Calculate the areas occupied by the bottles.
CC = bwconncomp(BW2);
stats = regionprops(CC);

% These areas can be misleading as we have some bottles which are halfway
% inside the image. 
no = size(stats);
rects = zeros(no(1),4);
area = zeros(no(1),1);
areaf = zeros(no(1),1);
rat = zeros(no(1),1);

% Calculate the actual areas
for i = 1:no(1)
rects(i,:) = stats(i).BoundingBox;
area(i) = stats(i).Area;
end

%Calculate the total area occupied
for i = 1:no(1)
rects(i,3) = min([rects(i,3) 70]);
end

for i = 1:no(1)
rects(i,4) = 480;
end

for i = 1:no(1)
areaf(i) = rects(i,4)*rects(i,3);
end

%% Important Properties gained !

% Ratios of areas provide invariance to the areas of the bottles inside the
% image. This code can handle variable sized bottles with minor
% modifications and also can handle scaling as ratios of areas are
% invariant, translation as well does not matter, as this method is
% independent of the position or number of bottles in the image
for i = 1:no(1)
   rat(i)=area(i)/areaf(i);
   if(rat(i)<0.12)disp('Yes!');
   else disp('No!'); end 
end

