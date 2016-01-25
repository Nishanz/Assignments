%% Loading and Preprocessing

im = imread('lenna.png');
im = single(im);
% Add Gaussian Noise as stated in the question.
noise = imnoise(im,'gaussian',0,0.05);
im = im -200 + noise;

%% DWT 

%Applying the wavelet transform
[cA,cH,cV,cD] = dwt2(im,'haar');
figure;
subplot(2,2,1);imshow(cA,[]);title('LL transform');
subplot(2,2,2);imshow(cH,[]);title('LH transform');
subplot(2,2,3);imshow(cV,[]);title('HL transform');
subplot(2,2,4);imshow(cD,[]);title('HH transform');

%% IDWT 

%Remove the other three and only retain the LL transform.
final = idwt2(cA,zeros(size(cH)),zeros(size(cV)),zeros(size(cD)),'haar');

%% Output

%The image gets cleaned. Works best for binary images.
figure;
subplot(2,1,1);imshow(im,[]);title('Image with Noise');
subplot(2,1,2);imshow(final,[]);title('Cleaned Image')