function [im_eq, histo, probc] = help_q2(im)
    
    unique_pixels=256;
    histo=zeros(unique_pixels,1);
    probc=zeros(unique_pixels,1,'double');
    im_size=size(im);
    this_sum=0;
    im_eq=zeros(im_size(1),im_size(2),'uint8');
    total_pixels=(im_size(1)*im_size(2));

    for i=1:im_size(1)
    for j=1:im_size(2)
        value=im(i,j)+1;
        histo(value)=histo(value)+1;
    end
    end

    for i=1:unique_pixels
        this_sum=this_sum+histo(i);
        probc(i)=this_sum/total_pixels;
    end

    histo=zeros(unique_pixels,1);
    out=zeros(unique_pixels,1);
    bin_size=255;
    
    for i=1:unique_pixels
       out(i)=round(probc(i)*bin_size); 
    end
    
    for i=1:im_size(1)
    for j=1:im_size(2)
        im_eq(i,j)=out(im(i,j)+1);
    end
    end
end


