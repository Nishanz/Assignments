

DatasetSize = size(ImageProjectedFeatures,1);
[ImageSampleCount,ImageUniqueLabel] = hist(ImageProjectedLabels,unique(ImageProjectedLabels));   
[IDX,D] = knnsearch(ImageProjectedFeatures,SketchProjectedFeatures,'k',DatasetSize);
PrecisionArray=[];RecallArray=[];

for i=1:size(IDX,1)    
    thisSketchLabel = SketchProjectedLabels(i);
    cc =0;
    pr = [];re=[];
    for j =1:size(IDX,2)
        if (ImageProjectedLabels(IDX(i,j)) == thisSketchLabel)
            cc=cc+1;
        end
        pr(j) = cc/j;re(j) = cc/ImageSampleCount(find(thisSketchLabel == ImageUniqueLabel));
            
    end
    PrecisionArray(i,:) = pr;RecallArray(i,:) = re;
end

Precision = mean(PrecisionArray,1);Recall = mean(RecallArray,1);