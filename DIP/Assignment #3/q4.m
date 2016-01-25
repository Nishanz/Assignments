%% Cylindrical Transform

im = single(imread('map.gif'));
sz = size(im);
disp(sz);
im_new = zeros(sz(1),sz(2),'uint8');

p = [-1/2+1/sz(2):1/sz(2):1/2];
q = [1/2-1/sz(2):-1/sz(1):-1/2];

[X,Y] = meshgrid(p,q);

Xq = X;
Yq = asin(Y);

im_new = interp2(X,Y,im,Xq,Yq);
figure;
subplot(1,2,1);
imshow(im,[]);
title('Original');
subplot(1,2,2);
imshow(im_new,[]);
title('Applying Cylindrical Transform');

%% Mercator Transform

syms x
f(x) = log(tan(x/2 + pi/4));

im_new = zeros(sz(1),sz(2),'uint8');
sz(1)=sz(1)/2;
sz(2)=sz(2)/2;
lim = pi/2 - 0.5705 ;

p = [-1+1/sz(2):1/sz(2):1];
q = [-lim+lim/sz(1):1/sz(1):lim];

[X,Y] = meshgrid(p,q);

stepval = (eval(f(lim))-eval(f(-lim)))/(2*sz(1));
qval = [eval(f(-lim))+stepval:stepval:eval(f(lim))];

[xval,yval] = meshgrid(p,qval);
Xq = xval;
Yq = 2*(atan(exp(yval))-pi/4);

im_new = interp2(X,Y,im,Xq,Yq);
figure;
subplot(1,2,1);
imshow(im,[]);
title('Original');
subplot(1,2,2);
imshow(im_new,[]);
title('Applying Mercator Transform');