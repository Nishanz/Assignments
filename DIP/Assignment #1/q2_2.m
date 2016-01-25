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

% Make ramp function. %
ramp_func = [];
for i=1:256
    if(i<=100)
        ramp_func(i) = 0;
    elseif (i<151)
        ramp_func(i) = ramp_func(i-1) + ((im_size(1)*im_size(2))/255)*(i-100)/50;
    else
        ramp_func(i) = ramp_func(i-1);
    end
end

% Normalize the ramp function. %
ramp_func = ramp_func/(im_size(1)*im_size(2));

% Find cummulative of ramp function. %
cum_ramp = [];
sum_ramp = 0;
for i=1:256
    sum_ramp = sum_ramp + ramp_func(i);
    cum_ramp(i) = sum_ramp;
end

map = zeros(256,1);

for i=1:256
    
    [~, closest_match] = min(abs(cum_ramp - cum_hist(i)));
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