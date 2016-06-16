function [result] = test_image(a, learn_array, label_array, k)
    img = imread(a);
    sol =[];
    for j=7:10
        T = kratchoukmoment(j, j, 2, img);
        val=abs(sum(T(:)))/10e+18;
        sol = [sol,val];
    end
    result = knnclassification( sol,learn_array, label_array, k);
end