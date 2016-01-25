im1=imread('spot1-diff1.jpg');
im2=imread('spot1-diff2.jpg');
im3=imread('spot2-diff1.png');
im4=imread('spot2-diff2.png');
size1=size(im1);
out1=zeros(size1(1),size1(2),'uint8');
size2=size(im3);
out2=zeros(size2(1),size2(2),'uint8');
for i = 1:size1(1)
    for j = 1:size1(2)
        for m = 1:size1(3)
            if(abs(im1(i,j,m)-im2(i,j,m))>20)
                out1(i,j)=255;
            end
        end
    end
end
for i = 1:size2(1)
    for j = 1:size2(2)
        for m = 1:size2(3)
            if(abs(im3(i,j,m)-im4(i,j,m))>5)
                out2(i,j)=255;
            end
        end
    end
end
se = strel('disk',3);
out1 = imdilate(out1,se);
imshow(im1);
[label num]=bwlabel(imfill(out1,'holes'));
props=regionprops(label);
box=reshape([props.BoundingBox],[4 num]);
for cnt=1:num
rectangle('position',box(:,cnt),'edgecolor','r');
end
hold off
se = strel('disk',3);
out2 = imdilate(out2,se);
figure;imshow(im3);
[label num]=bwlabel(imfill(out2,'holes'));
props=regionprops(label);
box=reshape([props.BoundingBox],[4 num]);
for cnt=1:num
rectangle('position',box(:,cnt),'edgecolor','r');
end
hold off
