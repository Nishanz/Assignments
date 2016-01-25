%% Load and initialize Variables.
clc;clear;
tic;
im=single(imread('soy-dessert.jpg'));
im2=single(imread('vegan-modified.jpg'));
im=imresize(im,0.25);
IM=im;
im2=imresize(im2,0.25);
Nimg=size(im2);
Nfilt=size(im);
mxval=0;
iim1=single(im2(:,:));
iim2=single(im2(:,:));

%% Implementation of the norm correlation 
% Subtract mean and then divide by the standard deviation as mentioned in Wikipedia Method-1.

mb=mean(mean(im));
sb=bsxfun(@power,sum(sum(bsxfun(@power,(im-mb),2))),0.5);
im = bsxfun(@rdivide,bsxfun(@minus,im,mb),sb);

x=-1;y=-1;
mimg=mean(mean(im2));

%% Calculate the Integral Images for the Big Image.

% Edges have to be handled seperately.
for i=2:Nimg(1)
    iim2(i,1)=iim2(i-1,1)+sqrt((im2(i,1)-mimg)*(im2(i,1)-mimg));
end
for j=2:Nimg(2)
    iim2(1,j)=iim2(1,j-1)+sqrt((im2(1,j)-mimg)*(im2(1,j)-mimg));
end

% Calculate the Standard Deviation Sum by formula.
for i=2:Nimg(1)
    for j=2:Nimg(2)
        iim2(i,j)=iim2(i-1,j)+iim2(i,j-1)-iim2(i-1,j-1)+sqrt((im2(i,j)-mimg)*(im2(i,j)-mimg));
    end
end

% Edges have to be handled seperately.
for i=2:Nimg(1)
    iim1(i,1)=iim1(i-1,1)+im2(i,1);
end
for j=2:Nimg(2)-1
    iim1(1,j)=iim1(1,j-1)+im2(1,j);
    
end

% Calculate the Sum by formula
for i=2:Nimg(1)-1
    for j=2:Nimg(2)
        iim1(i,j)=iim1(i-1,j)+iim1(i,j-1)-iim1(i-1,j-1)+im2(i,j);
    end
end

filtno = Nfilt(1)*Nfilt(2);

%% Sliding Window
for i=1:Nimg(1)-Nfilt(1)
    for j=1:Nimg(2)-Nfilt(2)
        % For each window, calculate mean and standard deviation in O(1)
        ma=iim1(i+Nfilt(1)-1,j+Nfilt(2)-1)-iim1(i,j+Nfilt(2)-1)-iim1(i+Nfilt(1)-1,j)+iim1(i,j);
        sa=iim2(i+Nfilt(1)-1,j+Nfilt(2)-1)-iim2(i,j+Nfilt(2)-1)-iim2(i+Nfilt(1)-1,j)+iim2(i,j);
        
        % Normalize the image and find the co-relation. It gives you the
        % normalized co-relation which is our required function.
        im_new = bsxfun(@rdivide,im2(i:i+Nfilt(1)-1,j:j+Nfilt(2)-1),sa);
        out = sum(sum(bsxfun(@times,im_new,im)));
        if out>mxval
            mxval=out;
            x=i;y=j;
        end
    end
end
   
%% Show the result window and the original image.

i=x;j=y;
figure;imshow(uint8(IM));
figure;imshow(uint8(im2(i:i+Nfilt(1)-1,j:j+Nfilt(2)-1)));
toc;
