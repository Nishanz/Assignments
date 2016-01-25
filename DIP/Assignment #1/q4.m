[im,map1]=imread('bell.jpg');
figure;
subplot(1,2,1),imshow(im,map1);
for i=0.0:0.2:1.0
    for j=2:3:50
        filt=mhighboost(i,j);
        im_filt = imfilter(im, filt);
        subplot(1,2,2),imshow(im_filt,map1);
        s=strcat('Filter: Unsharp ',' Alpha: ',num2str(i),' Weight Factor: ',num2str(j));
        title(gca,s);
        pause(1);
    end
end
for i=0:0.2:1
    for j=2:3:50
        filt=mhighboost_laplacian(i,j);
        im_filt = imfilter(im, filt);
        subplot(1,2,2),imshow(im_filt,map1);
        s=strcat('Filter: Laplacian ',' Alpha: ',num2str(i),' Weight Factor: ',num2str(j));
        title(gca,s);
        pause(1);
    end
end
