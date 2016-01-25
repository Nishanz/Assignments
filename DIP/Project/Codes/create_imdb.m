clear;clc

data_dir = '/Users/andas/Desktop/DIP_SBIR/data/';
% get all folders
folders = dir(data_dir); 
% remove . and .. directories from listing
folders(1:2) = [];

% get class names
classNames = {folders.name};
numClasses = length(classNames);
classes = cell(1,numClasses);


numImages=0;
for i = 1:numClasses
	files = dir(fullfile(data_dir, classNames{i}));
    files(1:2) = [];
	numImages = numImages+length(files);
end

% total number of images is 30607
names = cell(1,numImages);
data = zeros(224,224,3,numImages,'uint8');
labels = zeros(1,numImages,'uint16');
sets = ones(1,numImages,'uint8');

k = 1;
for i = 1:numClasses
    files = dir(fullfile(data_dir, classNames{i}));
    files(1:2) = [];
    numImages = length(files);
    
    for j = 1:numImages
        names{k} = fullfile(data_dir, classNames{i}, files(j).name);
        fprintf('%d %s\n',[k names{k}]);
        
        image = imread(names{k});
        
        if size(image,3)==1
            image2 = zeros(size(image,1), size(image,2), 3);
            for l=1:3
                %size(image)
                image2(:,:,l)=image(:,:);
                %size(image2)
            end
            image=image2;
        end
        
        for l=1:3
            data(:,:,l,k) = imresize( image(:,:,l), [224 224] );
        end
        labels(k) = i;
        
        if mod(j,3)==0
            sets(k) = 3;
        end
        k = k+1;
    end
   
    classes{i} = folders(i).name;   % Stores mapping b/w class labels of images and folder names
end


clear className classNames files folders i j k numClasses numImages image
% Update 'imdb' i.e, Image Database
imdb.images.names = names;
imdb.images.data = data;
imdb.images.labels = labels;
imdb.images.sets = sets;

imdb.meta.classes = classes;
imdb.meta.sets = {'train','val','test'};

clear names data labels sets classes
