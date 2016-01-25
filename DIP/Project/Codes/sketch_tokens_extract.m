function [E2] = sketch_tokens_extract(im)

addpath(genpath('./SketchTokens-master/'));
addpath(genpath('./toolbox-master/'));
load('./SketchTokens-master/models/forest/modelFull.mat');

%% detect sketch tokens and extract edges (see stDetect.m and stToEdges.m)

tic; st = stDetect( im, model );
E = stToEdges( st, 1 );

sz=size(E);
level = graythresh(E);
threshold=level;

for i=1:sz(1)
    for j=1:sz(2)
        if E(i,j)<threshold
            E(i,j)=0;
        else
            E(i,j)=1;
        end
    end
end

E2 = bwareaopen(E, 30);
%figure(2);im(im2double(E2));
end
