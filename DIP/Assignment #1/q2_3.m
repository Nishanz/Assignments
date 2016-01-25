img = imread('office.jpg');

% Take size of image and compute number of pixels. %
im_size = size(img);
num_pixels = im_size(1)*im_size(2);
outimg=img;
for k=1:3
im=img(:,:,k);
% Take histograms of each channel. %
hist = imhist(im(:,:));


% Find cummulative histograms. %
sum = 0;
cum_hist = [];
for i=1:256
    sum = sum + hist(i);
    cum_hist(i) = sum;
end

% Normalization. %
cum_hist = cum_hist/(im_size(1)*im_size(2));

% Make step function. %
step_func = [];
for i=1:256
    if(i<=100)
        step_func(i) = 0;
    else
        step_func(i) = num_pixels/180;
    end
end

% Normalize the step function. %
step_func = step_func/(im_size(1)*im_size(2));

% Find cummulative of step function. %
cum_step = [];
sum_step = 0;
for i=1:256
    sum_step = sum_step + step_func(i);
    cum_step(i) = sum_step;
end

map = zeros(256,1);

for i=1:256
    
    [~, closest_match] = min(abs(cum_step - cum_hist(i)));
    map(i) = closest_match(1);
end

out = zeros(im_size(1), im_size(2));
for i=1:im_size(1)
    for j=1:im_size(2)
        out(i,j) = map(im(i,j)+1);
    end
end
outimg(:,:,k)=out;
end
final_image = uint8(outimg);
figure, imshow(im)
figure, imshow(final_image)