% distanceToContour calculates the distance from a point to the
% closest point on the contour, possibly between vertices. by
% default, the contour is assumed to be open (closed == 0).
%
% d = distanceToContour(p,c,closed)
%
% Oliver Hinds <oph@bu.edu>
% 2005-12-06

function [mind,minp] = distanceToContour(p,c,closed)

  if(nargin < 2)
    fprintf('usage: d = distanceToContour(p,c,[closed])\n');
  end
  
  if(nargin < 3)
    closed = 0;
  end
  
  if(closed)
    c(end+1,:) = c(1,:);
  end
  
  mind = 2^100;
  minp = [0 0];

  
  for(v=1:size(c,1)-1)
    [curd curp] = distanceToSegment(p,c(v:v+1,:));
    
    if(abs(curd) < abs(mind))
      mind = curd;
      minp = curp;
    end
  end
  
return

function [d,p] = distanceToSegment(v,s)

% build vectors for angle calculation
  v1 = s(2,:)-s(1,:);
  v2 = v-s(1,:);
  
  dv1 = sqrt(sum(v1.^2));
  dv2 = sqrt(sum(v2.^2));
  dv3 = sqrt(sum((v-s(2,:)).^2));
  
  theta = angleBetween(s(2,:),s(1,:),v);  
  d = dv2*sin(theta);
  r = sqrt(dv2^2-d^2)/dv1;
  
  if(theta > pi/2 | r > 1.0)
    [d pid] = min([dv2 dv3]);
    p = s(pid,:);
    return;
  end
  
  p = s(1,:) + r*(s(2,:)-s(1,:));
return

% from
% http://www.csit.fsu.edu/~burkardt/m_src/geometry/angle_deg_2d.m
% by John Burkardt
function theta = angleBetween(a,b,c)
  p(1) = (c(1) - b(1)) * (a(1) - b(1)) + (c(2) - b(2)) * (a(2) - b(2));  
  p(2) = (c(1) - b(1)) * (a(2) - b(2)) - (c(2) - b(2)) * (a(1) - b(1));

  if(p(1) == 0.0 & p(2) == 0.0)
    theta = 0.0;
    return
  end

  theta = atan2(p(2), p(1));

  if(theta < 0.0)
    theta = theta + 2.0 * pi;
  end

  if(theta > pi)
    theta = 2.0 * pi - theta;
  end
return

function d = dist(a,b)
  d = sqrt(sum((a-b).^2));
return