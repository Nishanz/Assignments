clc;clear;

% IM_SEARCH: Given a sketch image, search for an image that looks like it
% using the structure tensor descriptor.
    if(nargin == 0)
        original_sketch_path = '/home/ameya/DIP/sketch/'
        %db_path = '/home/ameya/DIP/images/images_';
    end
    %load('mapped_dual_database.mat');
    disp('Loading data...');
    
    % Load db containing small versions of original images
    load('small_imdb.mat');
    % Load the db containing ssketch tokens of original images
    load('features_db_simple_shapes.mat');
    disp('Data loaded!');
    
    original_sketch_path = '/home/ameya/DIP/sketch/';
    while(1)
        sketch_name = input('Enter sketch name (q to quit): ', 's');
        if(sketch_name == 'q')
            disp('Quitting...');
            break;
        end
        sketch_path = strcat(original_sketch_path, sketch_name);
        
        % Generate Structure Tensor for sketch image
        im = imread(sketch_path);
        im = im2double(im);
        if(size(im,3) == 3)
            im = rgb2gray(im);
        end
        fvec1 = gen_tensor(im);
        if(~isempty(find(isnan(fvec1))))
            disp(i);
            disp('See this shit!');
        end
        
        % Search in DB for similar structure tensoruntitled2.m
        db=features;
        sz=size(features,4);
        dist_arr = [];
        for i = 1:sz
            %if mod(i,100)==0
            %    disp(i);
            %end
            fvec2 = db(:,:,:,i);
            dist = tensor_compare(fvec1, fvec2);
            dist_arr = [dist_arr; dist];
        end
        disp('Distances computed!');
        [sorted, index] = sort(dist_arr);
        
        % Display first 'ndisplay' results
        figure;
        subplot(6,4,1);
        imshow(im);  % The query sketch image
        title('Query Sketch');
        ndisplay = 23;
        for i = 1:ndisplay
            subplot(6,4,i+1);
            imshow(imdb.images.data(:,:,:,index(i)), []);
            title(strcat(num2str(i), ') Score:', num2str(sorted(i))));
        end
    end

