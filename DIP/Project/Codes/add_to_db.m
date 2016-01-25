function [ ] = add_to_db( folder_name )
% ADD_TO_DB: Add the images in 'folder' to pre-existing database 'imdb'
    tic;
    clc;
    data_dir = '/home/ameya/DIP/data/';
    
    old_imdb = load('small_imdb.mat');  % 'old_imdb' is a struct containing a field 'imdb'
    old_imdb = old_imdb.imdb;           % Now, 'old_imdb' is a struct containing a field 'images'
    old_features_db = load('features_db_simple_shapes.mat');    % 'old_features_db' is a struct containing a field 'features'
    old_features = old_features_db.features;
    
    % get class name
    classNames = {folder_name}
    numClasses = length(classNames);
    classes = cell(1,numClasses);
    
    numImages=0;
    files = dir(fullfile(data_dir, classNames{1}));
    files(1:2) = [];
    numImages = numImages+length(files);

    % total number of images
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

        classes{i} = folder_name;   % Stores mapping b/w class labels of images and folder names
    end

    clear className classNames files folders i j k numClasses numImages image
    
    % new_imdb has been created
    new_imdb.images.names = names;
    new_imdb.images.data =  data;
    new_imdb.images.labels= labels;
    new_imdb.images.sets =  sets;
    
    % create new_features_db
    new_features = create_features_db(new_imdb);
    
    
    % 'imdb' is the final DB formed by concatenation of old imdb (i.e, old_imdb) and new
    % data's imdb (i.e, new_imdb)
    imdb.images.names = [old_imdb.images.names  new_imdb.images.names];
    imdb.images.data =  cat(4,old_imdb.images.data,new_imdb.images.data);
    imdb.images.labels= [old_imdb.images.labels  new_imdb.images.labels];
    imdb.images.sets =  [old_imdb.images.sets   new_imdb.images.sets];

    imdb.meta.classes = [old_imdb.meta.classes  classes];
    imdb.meta.sets = {'train','val','test'};
    
    % Concatenate old and new feature DBs
    features = cat(4, old_features, new_features);
    
    save('small_imdb.mat','imdb','-v7.3');
    save('features_db_simple_shapes.mat','features','-v7.3');
    
    clear names data labels sets classes
    toc;

end

