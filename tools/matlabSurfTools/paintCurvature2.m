% paints curvature cdata onto a surface
%
% paintCurvature2(surfStruct,curveNeighbors,drawSurf,inflIter,smooth2DIter,smooth2DDim,invert)
%
% Oliver Hinds <oph@bu.edu>
% 2005-04-29

function [vcdata, curveSurf] = paintCurvature2(surfStruct,curveNeighbors,...
					       drawSurf,inflIter,...
					       smooth2DIter,smooth2DDim, ...
					       invert)
  
  % validate the input
  if(nargin < 1) 
    fprintf('error: surfStruct is a required argument. exit\n');
    return;
  end
  if(nargin < 2) 
    curveNeighbors = 1;
  end
  if(nargin < 3)
    drawOtherSurf = 0;
  else
    if(drawSurf == 0) 
      drawOtherSurf = 0;
    elseif(drawSurf == -1)
      drawOtherSurf = -1;
    else 
      drawOtherSurf = 1;
    end
  end
  if(nargin < 4) 
    inflIter = 0;
  end
  if(nargin < 5) 
    smooth2DIter = 0;
  end
  if(nargin < 6) 
    smooth2DDim = 3;
  end
  if(nargin < 6) 
    invert = 0;
  end

  %darkgray = 0.7;
  %lightgray = 0.4;
  
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
  
  if(inflIter > 0 | smooth2DIter > 0)
    curveSurf = surfStruct;
  
    if(inflIter > 0)
      curveSurf = inflateQ(curveSurf, inflIter, 0.1, 0, 0, 0);
    end
    
    if(smooth2DIter > 0)
      curveSurf = smoothSurf2D(curveSurf, smooth2DDim, smooth2DIter);
    end

    [curvM, curvG] = curvatureQ(curveSurf,curveNeighbors);
  else
    [curvM, curvG] = curvatureQ(surfStruct,curveNeighbors);
  end

  curvM(find(isnan(curvM))) = 0;
  if(invert) 
    curvM = -curvM;
  end
  
  vcdata = ones(size(curvM,1),1);
  vcdata(find(curvM > 0)) = -1;
  %vcdata = repmat(vcdata,1,3);
  
  if(drawOtherSurf ~= 0)
    if(drawOtherSurf == -1)
      showSurf(curveSurf,vcdata);
    else
      showSurf(drawSurf,vcdata);
    end
  else
    showSurf(surfStruct,vcdata);
  end
return

%************************************************************************%
%%% $Source: /home/cvs/PROJECTS/SurfaceConstructor/tools/matlabSurfTools/paintCurvature2.m,v $
%%% Local Variables:
%%% mode: Matlab
%%% fill-column: 76
%%% comment-column: 0
%%% End:
