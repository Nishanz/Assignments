clc;clear;
im=single(imread('soy-dessert.jpg'));
im2=single(imread('vegan-modified.jpg'));
im3=im2;
Nimg=size(im2);
Nfilt=size(im);
mxval=0;
iim1=single(im2(:,:));
iim2=single(im2(:,:));

ma=mean(mean(im2));
sa=bsxfun(@power,sum(sum(bsxfun(@times,bsxfun(@minus,im2,ma),bsxfun(@minus,im2,ma)))),0.5);
im2 = bsxfun(@rdivide,bsxfun(@minus,im2,ma),sa);

mb=mean(mean(im));
sb=bsxfun(@power,sum(sum(bsxfun(@times,bsxfun(@minus,im,mb),bsxfun(@minus,im,mb)))),0.5);
im = bsxfun(@rdivide,bsxfun(@minus,im,mb),sb);
x=-1;y=-1;
mimg=mean(mean(im2));

for i=2:Nimg(1)
    iim2(i,1)=iim2(i-1,1)+sqrt((im2(i,1)-mimg)*(im2(i,1)-mimg));
end

for j=2:Nimg(2)
    iim2(1,j)=iim2(1,j-1)+sqrt((im2(1,j)-mimg)*(im2(1,j)-mimg));
end

for i=2:Nimg(1)
    for j=2:Nimg(2)
        iim2(i,j)=iim2(i-1,j)+iim2(i,j-1)-iim2(i-1,j-1)+sqrt((im2(i,j)-mimg)*(im2(i,j)-mimg));
    end
end

for i=2:Nimg(1)
    iim1(i,1)=iim1(i-1,1)+im2(i,1);
end

for j=2:Nimg(2)-1
    iim1(1,j)=iim1(1,j-1)+im2(1,j);
    
end

for i=2:Nimg(1)-1
    for j=2:Nimg(2)
        iim1(i,j)=iim1(i-1,j)+iim1(i,j-1)-iim1(i-1,j-1)+im2(i,j);
    end
end

filtno = Nfilt(1)*Nfilt(2);

for i=1:Nimg(1)-Nfilt(1)
    disp(i);
    for j=1:Nimg(2)-Nfilt(2)
        ma=iim1(i+Nfilt(1)-1,j+Nfilt(2)-1)-iim1(i,j+Nfilt(2)-1)-iim1(i+Nfilt(1)-1,j)+iim1(i,j);
        sa=iim2(i+Nfilt(1)-1,j+Nfilt(2)-1)-iim2(i,j+Nfilt(2)-1)-iim2(i+Nfilt(1)-1,j)+iim2(i,j);
        im_new = im2(i:i+Nfilt(1)-1,j:j+Nfilt(2)-1);
        out = sum(sum(bsxfun(@times,im_new,im)));
        if out>mxval
            mxval=out;
            x=i;y=j;
        end
    end
end
im2=im3;
        
