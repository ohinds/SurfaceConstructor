% paints curvature cdata onto a surface
%
% paintCurvature(surfStruct,smooth2DIter,smooth2DDim,invert)
%
% Oliver Hinds <oph@bu.edu>
% 2005-04-29

function [vcdata, curveSurf] = paintCurvature(surfStruct,...
					      smooth2DIter,smooth2DDim, ...
					      invert)
  
  % validate the input
  if(nargin < 1) 
    fprintf('error: surfStruct is a required argument. exit\n');
    return;
  end
  if(nargin < 2) 
    smooth2DIter = 0;
  end
  if(nargin < 3) 
    smooth2DDim = 3;
  end
  if(nargin < 4) 
    invert = 0;
  end

  if(~isfield(surfStruct,'vertices') | ~isfield(surfStruct,'faces')) 
    fprintf('error: surfStruct should have at least "vertices" and "faces" fields. exit\n');
    return;
  end
  
  if(~isfield(surfStruct,'PREPROCESSED'))
    surfStruct = preprocessQ(surfStruct);
  end
  
  if(~strcmp('True',surfStruct.PREPROCESSED)) 
    fprintf('error: surface has non-manifold vertices. exit\n');
    return;    
  end
  
  if(smooth2DIter > 0)
    curveSurf = smoothSurf2D(surfStruct, smooth2DDim, smooth2DIter);
  else
    curveSurf = surfStruct;
  end

  %sulcalDepth = getSulcalDepth(curveSurf);
  
  vcdata = ones(size(sulcalDepth,1),1);
  vcdata(find(sulcalDepth > 0)) = -1;

return

%************************************************************************%
%%% $Source: /home/cvs/PROJECTS/SurfaceConstructor/tools/matlabSurfTools/paintCurvature.m,v $
%%% Local Variables:
%%% mode: Matlab
%%% fill-column: 76
%%% comment-column: 0
%%% End:
