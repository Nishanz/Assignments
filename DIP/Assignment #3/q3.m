%% Plain Pixel Replacement in Chroma Keying by Thresholding

im = imread('ameya.jpg');
im2 = imread('taj.jpg');
im= imresize(im,[500 750]);
%im2 = imresize(im2,[750 500]);

imb = im2bw(im,0.2);
se = strel('disk',25);
imb = imopen(imb,se);
sz = size(imb);

output = zeros(sz(1),sz(2),3,'uint8');

for i = 1:sz(1)
    for j = 1:sz(2)
        if imb(i,j)==true
            output(i,j,:)=im2(i,j,:);
        else
            output(i,j,:)=im(i,j,:);
        end
    end
end
figure;imshow(imb,[]);
figure;imshow(im2,[]);
figure;imshow(output,[]);
            