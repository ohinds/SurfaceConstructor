% calculate the error of the fit of a set of vertex coordinates to
% an analytically determined ellipse. wgrid is a grid of weights
% that attempts to account for the desity of the vertices along the
% boundary of some flattened surface. see getWgrid.m for details
% 
% Oliver Hinds <oph@bu.edu>
% 2005-05-23



function e = ellipseErr(x,coords,wgrid,wmapx,wmapy)
  
  a = x(1);
  b = x(2);
  tx = x(3);
  ty = x(4);
  r = x(5);
  
  e = 0;

  if(r < 0 | r > 2*pi)
    e = 100000;
  end
  
  for(v=1:size(coords,1))

    if(nargin > 2)
      wx = find(coords(v,1) < wmapx);
      if(length(wx) < 1)
	wx = size(wmapx,1);
      else 
	wx = wx(1);
      end
      
      wy = find(coords(v,2) < wmapy);
      if(length(wy) < 1)
	wy = size(wmapy,1);
      else 
	wy = wy(1);
      end
      
      w = wgrid(wx,wy);
    else
      w = 1;
    end
      
    tcoords = coords(v,1:2)';
    tcoords(1) = tcoords(1)-tx;
    tcoords(2) = tcoords(2)-ty;
    tcoords = inv([cos(r), sin(r); -sin(r), cos(r)]) * tcoords;
    
    e = e + w*abs(((tcoords(1)/a)^2 + (tcoords(2)/b)^2) - 1);
  end

%   cla;
%   plot(coords(:,1), coords(:,2),'g.');
%   hold on
%   plot_ellipse(a,b,tx,ty,r,'r');
%   pause(0.01);
  
return;
