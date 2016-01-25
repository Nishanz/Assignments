load('core_imdb_filelisted.mat')
filelist_train=cell(1,5670);
filelist_test=cell(1,2730);
no=1;
j=1;
k=1;

dataval='images';
data=imdb.images.data;
sets=imdb.images.sets;

for i=1:8400
    filename=['./images/' dataval '_' num2str(no) '.png'];
    disp(filename);
    if sets(i) == 1
    filelist_train{:,j}=filename;
    j=j+1;
    else
    filelist_test{:,k}=filename;
    k=k+1;
    end
    imwrite(data(:,:,:,i),filename,'png');
    no=no+1;
end
filelist.train_list=filelist_train;
filelist.test_list=filelist_test;

imdb.images.info=filelist;

save('core_imdb_filelisted.mat','imdb','-v7.3');