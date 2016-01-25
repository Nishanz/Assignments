%%Cylindrical Transform

im = imread('map.gif');
sz = size(im);
big=0.1

im_new = zeros(sz(1),sz(2),'uint8');

for i=1:360
    for j=1:sz(2)
        im_new(360-i+1,j)=im(round(radtodeg(asin((360-i)/360))*4)+1,j);
    end
end
for i=361:720
    for j=1:sz(2)
        im_new(i,j)=im((round(radtodeg(asin((i-360)/360))*360/90)+360),j);
    end
end
imshow(im,[]);
figure;imshow(abs(im_new),[]);

%% Mercator Transform

im_new = zeros(sz(1),sz(2),'uint8');

for i=1:360
    for j=1:sz(2)
        im_new(360-i+1,j)=im(round(4*abs(radtodeg(2*(atan(exp(degtorad((360-i)/4)))-pi/4))))+1,j);
    end
end
for i=361:720
    for j=1:sz(2)
        im_new(i,j)=im(round(4*abs(radtodeg(2*atan(exp(degtorad((360-i)/4)))-pi/4)))+360,j);
    end
end
imshow(im,[]);
figure;imshow(im_new,[]);