clc;clear;

% IM_SEARCH: Given a sketch image, search for an image that looks like it
% using the structure tensor descriptor.
    if(nargin == 0)
        original_sketch_path = '/home/ameya/DIP/sketches/sketches_';
        db_path = '/home/ameya/DIP/images/images_';
    end
    %load('mapped_dual_database.mat');
    disp('Loading data...');
    load('features_db_new_with_token.mat'); %TODO
    disp('Data loaded!');
    
    while(1)
        sketch_num = input('Enter sketch no.(-1 to quit): ');
        if(sketch_num == -1)
            disp('Quitting...');
            break;
        end
        sketch_path = strcat(original_sketch_path, num2str(sketch_num),'.png');
        
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
        
        % Search in DB for similar structure tensor
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
            im_path = strcat(db_path, num2str(index(i)), '.png');
            subplot(6,4,i+1);
            imshow(imread(im_path));
            title(strcat(num2str(i), ') Score:', num2str(sorted(i))));
        end
    end

