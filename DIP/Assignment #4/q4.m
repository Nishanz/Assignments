%% Loading and Preprocessing

im = imread('q4.png');
im = single(rgb2gray(im));

%% Applying the Ripple Transform 

[x, y] = meshgrid(1:size(im,2), 1:size(im,1));
x_new = x + 10*sin(2*pi*y/120);
y_new = y + 15*sin(2*pi*x/150);
final = interp2( x, y, im, x_new, y_new, 'linear');

%% Plot the results

subplot(1,2,1); imshow(im,[]); title('Original Image');
subplot(1,2,2); imshow(final,[]); title('Transformed Image');