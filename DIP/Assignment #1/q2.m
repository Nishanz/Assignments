im=imread('office.jpg');
[im_eq, hist, probc] = help_q2(im);
[im_eq(:,:,1), hist, probc] = help_q2(im(:,:,1));
[im_eq(:,:,2), hist, probc] = help_q2(im(:,:,2));
[im_eq(:,:,3), hist, probc] = help_q2(im(:,:,3));
imshow(im_eq);figure;
subplot(2,3,1),bar(im(:,:,1));
subplot(2,3,2),bar(im(:,:,2));
subplot(2,3,3),bar(im(:,:,3));
subplot(2,3,4),bar(im_eq(:,:,1));
subplot(2,3,5),bar(im_eq(:,:,2));
subplot(2,3,6),bar(im_eq(:,:,3));

