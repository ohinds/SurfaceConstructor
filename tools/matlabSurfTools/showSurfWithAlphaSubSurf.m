% showSurfWithAlphaSubSurf displays a surface with some good default
% lighting and axes, and a sub surf transparent on top
%
% SHOWSURFWITHALPHASUBSURF(SURFSTRUCT,SUBSURFFACES,FACECOLOR,FACEALPHA)
%   SURFSTRUCT is a surface structure with M vertices and N faces
%   containing at least the members: vertices, a 3xM list of vertex
%   coordinates, and faces, a 3xN list of face vertex indices
%
% Oliver Hinds <oph@bu.edu>
% 2004-11-05

function showSurfWithAlphaSubSurf(surfStruct,subSurfFaces,...
				  faceColorSub,faceAlphaSub)
  % check for args
  if(nargin < 2)
    fprintf('usage: handle = showSurfWithAlphaSubSurf(surfStruct,subSurfFaces[,faceColor,faceAlpha])');
  end
  
  if(nargin < 3) 
    faceColorSub = [1.0,0.3,0.3];
  end
  
  if(nargin < 4)
    faceAlphaSub = 0.5;
  end
  
  faceColor = [0.6,0.6,0.6];
  faceAlpha = 1.0;

  h = patch('vertices', surfStruct.vertices, ...
	    'faces', surfStruct.faces, ...
	    'facecolor', faceColor, ...
	    'edgecolor', 'none',...
	    'facealpha',faceAlpha);

  subSurf = surfStruct;
  %normals = get(h,'vertexnormals'); 
  normals = getVertexNormals(subSurf);
  subSurf.faces = subSurfFaces;
  subSurfGrown = growSurf(subSurf,normals,0.0001);
  patch('vertices', subSurfGrown.vertices, ...
	'faces', subSurfGrown.faces, ...
	'facecolor', faceColorSub, ...
	'edgecolor', 'none',...
	'facealpha',faceAlphaSub);
  
  axis off;
  axis vis3d;
  axis equal;
  lighting gouraud
  material dull
  camlight;
return

%************************************************************************%
%%% $Source: /home/cvs/PROJECTS/SurfaceConstructor/tools/matlabSurfTools/showSurfWithAlphaSubSurf.m,v $
%%% Local Variables:
%%% mode: Matlab
%%% fill-column: 76
%%% comment-column: 0
%%% End:
