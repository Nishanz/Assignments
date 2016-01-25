im=zeros(2000,2000);
n=50;
imshow(im);
while(n)
f = imfreehand(gca,'Closed',0);
n = n - 1;
end