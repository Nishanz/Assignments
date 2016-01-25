im=imread('vegan.jpg');
im2=imread('soy-dessert.jpg');
Nimg=size(im2);
Nfilt=size(im);
x=-1;y=-1;
mxval=9999;
for i=1:N(1)-Nfilt(1)
    for j=1:N(2)-Nfilt(2)
        if (im2(i:i+Nfilt(1),j:j+Nfilt(2))-im(:,:)<mxval)
            mxval=im2(i:i+Nfilt(1),j:j+Nfilt(2))-im(:,:);
            x=i;y=j;
        end
    end
end
disp(x,y);