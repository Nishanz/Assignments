function [ vpatch ] = gen_tensor( im )
% GEN_TENSOR: Given an image, generate the structure tensor vector for it
% containing 196 structure tensors, each of size 16x16
    im = imresize(im,[224 224]);
    sz = size(im);
    vpatch = zeros(2,2,196); %Can make 196 variable, 196 baecause 16x16 size -> 14x14 = 196 blocks
    count = 0;

    for i = 1:16:sz(1)
        for j = 1:16:sz(2)
            count = count+1;
            % disp([i j]);
            patch = im(i:i+15,j:j+15);
            [gx, gy] = imgradientxy(patch);  % 'grad' and 'dir' are magnitude and direction of gradient image
            
            gx2 = gx.*gx;
            gxgy = gx.*gy;
            gy2 = gy.*gy;
            
            G = zeros(2,2);
            G(1,1)=sum(sum(gx2));
            G(1,2)=sum(sum(gxgy));
            G(2,1)=sum(sum(gxgy));
            G(2,2)=sum(sum(gy2));
            
            n = norm(G,'fro');   % Frobenius norm
            n = nearestSPD(n);
            T = G / n;
            vpatch(:,:,count)=T;
        end
    end
end