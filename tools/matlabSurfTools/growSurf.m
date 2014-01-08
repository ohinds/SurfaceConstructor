% surfStruct = growSurf(surfStruct,vertexNormals[,epsilon])
% Oliver Hinds <oph@bu.edu>
% 2005-03-24

function surfStructGrown = growSurf(surfStruct,vertexNormals,epsilon)
  
  % validate
  if(nargin < 2)
    fprintf('usage: surfStruct = growSurf(surfStruct,vertexNormals[,epsilon])\n');
    return;
  end
  
  if(nargin < 3)
    epsilon = 0.001;
  end
  
  % move each vertex out a bit in the direction of its normal
  keyboard
  surfStructGrown = surfStruct;
  for(vertex=1:size(surfStruct.vertices,1))
    surfStructGrown.vertices(vertex,:) = surfStruct.vertices(vertex,:) ...
	+ epsilon*vertexNormals(vertex,:);
  end
return

%************************************************************************%
%%% $Source: /home/cvs/PROJECTS/SurfaceConstructor/tools/matlabSurfTools/growSurf.m,v $
%%% Local Variables:
%%% mode: Matlab
%%% fill-column: 76
%%% comment-column: 0
%%% End:
