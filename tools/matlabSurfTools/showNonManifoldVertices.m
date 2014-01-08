% showNonManifoldVertices displays a only the faces in a surface that
% contain a non manifold vertex
%
% SHOWNONMAINFOLDVERTICES(SURFSTRUCT) 
%   SURFSTRUCT is a surface structure with M vertices and N faces
%   containing at least the members: vertices, a 3xM list of vertex
%   coordinates, and faces, a 3xN list of face vertex indices
%
% Oliver Hinds <oph@bu.edu>
% 2004-11-05

function h = showNonManifoldVertices(surfStruct,chopsurf)
  % check for args
  if(nargin < 1)
    fprintf('usage: handle = showSurf(surfStruct[,faceColor]');
  end
  
  if(nargin < 2) 
    chopsurf = 0;
  end
  
  % preprocess if necessary
  if(~isfield(surfStruct,'PREPROCESSED'))
    surf = preprocessQ(surfStruct);
  else
    surf = surfStruct;
  end
  
  % check for manifoldness
  if(~strcmp(surf.PREPROCESSED,'NON-MANIFOLD!!!'))
    fprintf('the surface is manifold, exiting\n');
    return;
  end
  
  % get the non manifold vertices
  nmv = find(surf.vertexManifoldness ~= 0);
  
  % get faces with non manifold vertices
  nmf = [];
  for(ind=1:length(nmv))
    nmf = [nmf;
	   unique([find(surf.faces(:,1) == nmv(ind)); 
		   find(surf.faces(:,2) == nmv(ind)); 
		   find(surf.faces(:,3) == nmv(ind))])];
  end
  
  % set the new face array
  if(chopsurf)
    surf.faces = surf.faces(nmf,:);
  end
  
  % show
  showVertex(surf,nmv);
return

%************************************************************************%
%%% $Source: /home/cvs/PROJECTS/SurfaceConstructor/tools/matlabSurfTools/showNonManifoldVertices.m,v $
%%% Local Variables:
%%% mode: Matlab
%%% fill-column: 76
%%% comment-column: 0
%%% End:
