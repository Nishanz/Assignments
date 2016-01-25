function angle = find_angle(im)
    [y,x] = find(im);
    mx = mean(x);
    my = mean(y);
    s1=bsxfun(@minus,x,mx);
    s2=bsxfun(@minus,y,my);
    C = [ mean(s1.*s1) mean(s1.*s2); mean(s1.*s2) mean(s2.*s2)];
    [V D] = eig(C);
    [~, mxi] = max(diag(D));
    angle = atan2(V(2,mxi),V(1,mxi))*180/pi;
end
