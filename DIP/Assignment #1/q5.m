img1 = double(imread('face.png'))/255;
img1(img1<0) = 0;
img1(img1>1) = 1;
sigma1 = 20; 
sigma2 = 0.1;
bflt_img1 = mybfilter(img1,5,sigma1,sigma2);
bflt_img2 = mybfilter(img1,10,sigma1,sigma2);
bflt_img3 = mybfilter(img1,15,sigma1,sigma2);
figure;imshow(bflt_img1);
figure;imshow(bflt_img2);
figure;imshow(bflt_img3);

