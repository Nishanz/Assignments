%% Load and initialize Variables.
tic;
im=single(imread('soy-dessert.jpg'));
im2=single(imread('vegan-modified.jpg'));
im=imresize(im,0.25);
IM=im;
im2=imresize(im2,0.25);
Nimg=size(im2);
Nfilt=size(im);
x=-1;y=-1;
mxval=0;

%% Implementation of the norm correlation 
% Subtract mean.
ma=mean(mean(im));
im = bsxfun(@minus,im,ma);

% Calculate the L2 norm
sa=bsxfun(@power,sum(sum(bsxfun(@times,im,im))),0.5);

%Calculate the matrix with det = 1 for the image.
im = bsxfun(@rdivide,im,sa);

for i=1:Nimg(1)-Nfilt(1)
    for j=1:Nimg(2)-Nfilt(2)
        % Apply the filter to each window and take the one which has
        % maximal co-relation with the input.
        out = normcorr(im2(i:i+Nfilt(1)-1,j:j+Nfilt(2)-1),im);
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
