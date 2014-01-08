% calculates the perimeter of a surfStruct
%
% b = perimeter(SURFSTRUCT) 
%
% Oliver Hinds <oph@bu.edu>
% 2004-11-05

function p = perimeter(surfStruct)
  % check for args
  if(nargin < 1)
    fprintf('usage: a = faceArea(surfStruct)');
  end
  
  p = 0;
  bn = boundaryVertices(surfStruct);
  for(v=1:size(bn,1))
    p = p+getDist(surfStruct.vertices(bn(v),:),surfStruct.vertices(bn(mod(v,size(bn,1)-1)+1),:));
  end
  
return

function d = getDist(v1,v2)
  d = sqrt(sum((v1-v2).^2));
return


