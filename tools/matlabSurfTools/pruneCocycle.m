% pruneCocycle takes two vertices on the boundary of a
% surface, finds the shortest path between them, prunes the faces
% adjacent to the path, then extracts the largest connected
% component and returns it as the new surface.
%
% prunedSurf = pruneCocycle(surfStruct, verts)
% 
% Oliver Hinds <oph@bu.edu>
% 2005-10-24

function prunedSurf = pruneBetweenVertices(surfStruct, verts)
  
  % validate
  if(nargin < 2)
    fprintf('usage: prunedSurf = pruneBetweenVertices(surfStruct, verts)\n');
    return;
  end
  
  if(~isSurf(surfStruct))
    return;
  end
  
  if(size(verts) ~= [2 1]) 
    fprintf('verts must be 2x1\n');
    fprintf('usage: prunedSurf = pruneBetweenVertices(surfStruct, verts)\n');
    return;
  end

  bn = boundaryVertices(surfStruct);
  
  % check that the verts are on the boundary
%keyboard
  if(sum((verts(1) == bn) + (verts(2) == bn)) ~= 2)
    fprintf('boths verts must be on the boundary\n');
    fprintf('usage: prunedSurf = pruneBetweenVertices(surfStruct, verts)\n');
    return;
  end    
  
  path = findShortestPath(surfStruct,verts);

  showVertex(surfStruct,path);
  
  prunedSurf = pruneVertices(surfStruct,path);
return

%************************************************************************%
%%% $Source: /home/cvs/PROJECTS/SurfaceConstructor/tools/matlabSurfTools/pruneCocycle.m,v $
%%% Local Variables:
%%% mode: Matlab
%%% fill-column: 76
%%% comment-column: 0
%%% End:
