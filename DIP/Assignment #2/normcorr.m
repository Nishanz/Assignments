function [out] = normcorr(im2,im)
    mb=mean(mean(im2));
    im2 = bsxfun(@minus,im2,mb);
    sb=bsxfun(@power,sum(sum(bsxfun(@times,im2,im2))),0.5);
    im2 = bsxfun(@rdivide,im2,sb);
    out = abs(sum(dot(im,im2)));
end