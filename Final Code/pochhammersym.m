function o = pochhammersym(a,k)
if k<0
    print (' formula is different for -ve k value');
end
h=a;
for j = 0 : k-2
    a(j+2)=a(j+1)*(h+j+1);
end
o=a(end);