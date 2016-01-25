function [ dist ] = tensor_compare(fvec1, fvec2)
% tensor_compare: Compute similarity b/w two structure tensors vectors
% (each vector has 196 normalized structure tensors, each of size 2x2)
    nvec = size(fvec1,3);  % nvec = 196 here
    dist = 0;
    for i=1:nvec
        diff = fvec1(:,:,i) - fvec2(:,:,i);
        dist = dist + norm(diff,'fro'); % frobenius norm
    end
end