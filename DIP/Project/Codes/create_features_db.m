function [features] = create_features_db(imdb)
    tic;    
    % Takes ~172 sec for small db (having simple shapes)
    sz=size(imdb.images.labels,2);
    features = zeros(2,2,196,sz);

    parfor i=1:sz
        im_sketch_tokens = sketch_tokens_extract(im2double(imdb.images.data(:,:,:,i)));
        image_tensor = gen_tensor(im_sketch_tokens);
        features(:,:,:,i)=image_tensor(:,:,:);
        disp(i);
    end
    % save('features_db_simple_shapes.mat','features','-v7.3');
    toc;
end