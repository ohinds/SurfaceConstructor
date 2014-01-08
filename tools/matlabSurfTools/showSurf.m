% showSurf displays a surface with some good default lighting and
% axes
%
% h = showSurf(surfStruct,showVertexCurvature,showFaceCurvature,showLabels)
%   SURFSTRUCT is a surface structure with M vertices and N faces
%   containing at least the members: vertices, a 3xM list of vertex
%   coordinates, and faces, a 3xN list of face vertex indices
%
% note that showFaceCurvature and showVertexCurvature are mutally
% exclusive. faces will be prefered.
%
% Oliver Hinds <oph@bu.edu>
% 2004-11-05

function h = showSurf(surfStruct,showVertexCurvature,showFaceCurvature,showLabels,vcdata)
  % check for args
  if(nargin < 1)
    fprintf('usage: handle = showSurf(surfStruct[,faceColor,faceAlpha,labelColors]');
  end
  if(nargin < 2) 
    showVertexCurvature = 0;
  end
  if(nargin < 3) 
    showFaceCurvature = 0;
  end
  if(nargin < 4) 
    showLabels = 0;
  end
  if(nargin > 4)
    vcdataOverride = 1;
    showLabels = 0;
    showFaceCurvature = 0;
    showVertexCurvature = 0;
    vertexFaceColor = vcdata;
  else
    vcdataOverride = 0;
  end
  
  % determine the face/vertex coloring
  if(~vcdataOverride)
    vertexFaceColor = [];
    if(showFaceCurvature ~= 0 & isfield(surfStruct,'faceCurvature'))
      vertexFaceColor = surfStruct.faceCurvature;
      showVertexCurvature = 0;
    end
    
    if(showVertexCurvature & isfield(surfStruct,'vertexCurvature'))
      vertexFaceColor = surfStruct.vertexCurvature;
    end
    
    if(isempty(vertexFaceColor))
      vertexFaceColor = repmat([0.6,0.6,0.6],size(surfStruct.faces,1),1);
    else
      vertexFaceColor(find(vertexFaceColor == 1)) = 0.7;
      vertexFaceColor(find(vertexFaceColor == -1)) = 0.4;
      vertexFaceColor = repmat(vertexFaceColor,1,3);    
    end
  else % if there is vertex coloring, convert it to face coloring
    fcdata = zeros(size(surfStruct.faces,1),1);
    for(f=1:size(surfStruct.faces,1))
      fcdata(f) = min(vcdata(surfStruct.faces(f,:)));
    end
    vertexFaceColor = fcdata;
  end

  % if labels should be colored defaultly, do it
  labelColorMod = 0.2;
  if(showLabels & isfield(surfStruct,'vertexLabels'))
    surfStruct.vertexLabels(find(~isfinite(surfStruct.vertexLabels))) = -1;
    
    if(size(vertexFaceColor,1) == size(surfStruct.vertices,1))
      lv = find(surfStruct.vertexLabels ~= -1);
      vertexFaceColor(lv,2:3) = vertexFaceColor(lv,2:3) - labelColorMod;
      vertexFaceColor(find(vertexFaceColor < 0)) = 0;
    else
      for(f=1:size(surfStruct.faces,1))
	l = surfStruct.vertexLabels(surfStruct.faces(f,:));
	if(isempty(find(l == -1)) & l(1) == l(2) & l(2) == l(3))
	  vertexFaceColor(f,2:3) = vertexFaceColor(f,2:3) - labelColorMod;
	end
      end
      vertexFaceColor(find(vertexFaceColor < 0)) = 0;
    end
    labels = 0;
  end
  
  % patch whatever vertices are left over
  h = patch('vertices', surfStruct.vertices, ...
	    'faces', surfStruct.faces, ...
	    'facevertexcdata', vertexFaceColor, ...
	    'facecolor', 'flat', ...
	    'edgecolor', 'none');
  axis off;
  axis vis3d;
  axis equal;
  lighting gouraud
  material dull
  camlight;
  cameratoolbar;
return

%************************************************************************%
%%% $Source: /home/cvs/PROJECTS/SurfaceConstructor/tools/matlabSurfTools/showSurf.m,v $
%%% Local Variables:
%%% mode: Matlab
%%% fill-column: 76
%%% comment-column: 0
%%% End:
