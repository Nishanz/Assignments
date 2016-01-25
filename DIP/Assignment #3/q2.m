%% Applying TopHat Filtering

im = imread('circles.jpg');
im = im2bw(im,35/255);
cc1 = 0;
sz=size(im);
for i = 11:-1:4
    se = strel('disk',i);
    im2 = imtophat(im,se);
    im2 = im - im2 ;
    cc = bwconncomp(im2);
    % To see only this category object numbers, we have to subtract it from the cumulative sum.
    disp(cc.NumObjects-cc1);
    cc1=cc.NumObjects;
    figure;imshow(im2,[]);
end
    