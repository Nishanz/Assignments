function []=show_tensor(fvec1)
count=1;
im=zeros(28,28);
for i = 1:2:28
for j=1:2:28
im(i:i+1, j:j+1) = fvec1(:,:,count);
count=count+1;
end
end
imshow(im,[]);
end