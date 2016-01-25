img1 = double(imread('boy_smiling.jpg'))/255;
img1(img1<0) = 0;
img1(img1>1) = 1;
sigma1 = 20; 
sigma2 = 0.1;
for i=1:3
    bflt_img1 = myinversebfilter(img1(:,:,i),5,sigma1,sigma2);
    bflt_img2 = myinversebfilter(img1(:,:,i),10,sigma1,sigma2);
    bflt_img3 = myinversebfilter(img1(:,:,i),15,sigma1,sigma2);
end
figure;imshow(bflt_img1);
figure;imshow(bflt_img2);
figure;imshow(bflt_img3);

