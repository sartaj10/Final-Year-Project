function [learnset] = createFeatureVector()
    learnset = []; c={};
    imagefiles = dir('Custom1\*.jpg');
    c={c,imagefiles.name};
    c=c(2:end);
    c=sort_nat(c);
    len = length(imagefiles);
    
    for i=1:len
        img = imread(c{i});
        a = [];
        for j=7:10
            T = kratchoukmoment(j, j, 2, img);
            val=abs(sum(T(:)))/10e+18;
            a = [a,val];
        end
        learnset = [learnset;a];
    end
end