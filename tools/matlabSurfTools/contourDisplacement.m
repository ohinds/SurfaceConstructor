% contour displacement calculated a displacement vector for each
% vertex in a contour relative to another contour
% 
% d = contourDisplacement(c1, c2)
%
% Oliver Hinds <oph@bu.edu>
% 2005-12-06

function [d p] = contourDisplacement(c1,c2)
  
  if(nargin < 2)
    fprintf('usage: d = contourDisplacement(c1,c2)\n');
  end
  
  d = [];
  p = [];
  
  for(v=1:size(c1,1))
    [d(end+1) p(:,end+1)] = distanceToContour(c1(v,:),c2);
  end
  
  p = p';
  
return
