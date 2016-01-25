function f = mhighboost(alpha, boost)
    filt=fspecial('unsharp',alpha);
	f = (1.0-1.0/boost)*filt + 1.0/boost;
end
