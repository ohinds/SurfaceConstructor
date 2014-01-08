% calculates the aspect ratio of the boundary of a flat surface
%
% Oliver Hinds <oph@bu.edu>
% 2004-11-05

function aspect = aspectRatio(flatSurf)
  % check for args
  if(nargin < 1)
    fprintf('usage: aspect = aspectRatio(flatSurf)\n');
  end
  
  bn = boundaryVertices(flatSurf);  
  coords = flatSurf.vertices(bn,1:2);
%  coords = flatSurf.vertices(:,1:2);

  aspect.e = eig(cov(coords));
  aspect.ratio = sqrt(max(aspect.e)/min(aspect.e));
  
return

