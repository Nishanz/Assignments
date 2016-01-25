function f = mhighboost_laplacian(alpha, boost)
    filt=fspecial('laplacian',alpha);
	f = (1.0-1.0/boost)*filt + 1.0/boost;
end