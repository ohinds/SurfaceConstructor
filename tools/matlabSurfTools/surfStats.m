% calculates the surface statistics of a surfStruct used for
% comparison of surfaces
%
% Oliver Hinds <oph@bu.edu>
% 2004-11-05

function [area, perim, fperim, aspect, ellipse] ...
      = surfStats(surfStruct,flatSurf)
  % check for args
  if(nargin < 1)
    fprintf('usage: [area, perimeter, aspect, ellipse] = surfStats(surfStruct,flatSurf)\n');
  end
  
  area = surfaceArea(surfStruct);
  perim = perimeter(surfStruct);
  fperim = perimeter(flatSurf);
  aspect = aspectRatio(flatSurf);
  ellipse = fitEllipse(flatSurf);
  
return

