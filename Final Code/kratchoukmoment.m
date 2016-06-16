function T = kratchoukmoment(g,h,p,img)
[nx,ny,~]=size(img);
img = rgb2gray(img);
img=im2double(img);

if nx ~= ny
    fprintf('img is not square');
end
for x = 0 : nx - 1
    for n = 0 : g-1
%         w = nchoosek(nx-1,x)*p^x*(1-p)^(nx-x-1);
        rho(n+1) = (-1)^n*(1/p-1)^n*(factorial(n)/pochhammersym(-nx+1,n));
			A(n+1,x+1) = sqrt(1/rho(n+1))*hypergeom2F1(-n,-x,-(nx-1),1/p,0.01);
    end
end
for y = 0 : ny - 1
    for n = 0 : h-1
%         w = nchoosek(ny-1,y)*p^y*(1-p)^(ny-y-1);
        rho(n+1) = (-1)^n*(1/p-1)^n*(factorial(n)/pochhammersym(-ny+1,n));
        B(n+1,y+1) = sqrt(1/rho(n+1))*hypergeom2F1(-n,-y,-(ny-1),1/p,0.01);
    end
end
for i = 0 : g-1 
    for j = 0 : h-1
            px = A(i + 1,:);
            py = B(j + 1,:);
            Q = px' * py;
            temp = (img .* Q);
            T(i + 1,j + 1) = sum(temp(:));
    end
end
s=abs(sum(T(:)));