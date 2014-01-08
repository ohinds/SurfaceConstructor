% smooths a surface along two-dimensions, without modifying the third
%
% [smoothedSurf] = smoothSurf2D(surfStruct[,dim,iter,alpha])
%
% Oliver Hinds <oph@bu.edu>
% 2005-04-29

function smoothedSurf = smoothSurf2D(surfStruct,dim,iter,alpha)

  % validate the input
  if(nargin < 1) 
    fprintf('error: surfStruct is a required argument. exit\n');
    return;
  end
  if(nargin < 2) 
    dim = 3;
  end
  if(nargin < 3) 
    iter = 10;
  end
  if(nargin < 4) 
    alpha = 0.1;
  end

  display = 0;
  showIt = 10;
  
  
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
  
  if(dim > 3 | dim < 1)
    fprintf('error: dim %d is out of bounds [1-3]. exit\n',dim);
    return;        
  end
  
  smoothedSurf = surfStruct;

  for(it=1:iter)
    % iterate over vertices, finding adjacent vertices and fitting the
    % plane 
    for(vert=1:size(smoothedSurf.vertices,1))
      % get the adjacent vertices
      adjVert = smoothedSurf.vertices(smoothedSurf.Neighbors(smoothedSurf.Index2(vert):...
						  smoothedSurf.Index2(vert)...
						  +smoothedSurf.NNoV(vert)-1),:);
      
      % eliminate vertices in same dim
      adjVert(find(smoothedSurf.vertices(vert,dim) == adjVert(:,dim)),:) = [];
      
      % see if there are enough vertices to smooth
      if(size(adjVert,1) < 3)
	continue;
      end
      
      % see if all adjacent vertices are on one side of this vertex in the
      % dim dimension
      if(isempty(find(adjVert(:,dim) < smoothedSurf.vertices(vert,dim))) ...
	 | isempty(find(adjVert(:,dim) > smoothedSurf.vertices(vert,dim))))
	continue;
      end
      
      %keyboard;
      
      % fit the plane
      X = [ones(size(adjVert,1),1) adjVert(:,dim)];
      y = adjVert;
      y(:,dim) = [];
      B = inv(X'*X)*X'*y;
      
      % get the plane's prediction of the var dims of this vertex
      yhat = [1 smoothedSurf.vertices(vert,dim)]*B;
      
      % assign the new vertex
      for(d=1:3)
	if(d~=dim)
	  smoothedSurf.vertices(vert,d) = smoothedSurf.vertices(vert,d) + ...
	      alpha * (yhat(1) - smoothedSurf.vertices(vert,d));
	  yhat(1) = [];
	end
      end      
    end

    if(display & mod(it+1,showIt) == 0)
      smoothedSurf = preprocessQ(smoothedSurf);
      figure,showSurf(smoothedSurf);      
      pause;
    end

  end
  
return;

%************************************************************************%
%%% $Source: /home/cvs/PROJECTS/SurfaceConstructor/tools/matlabSurfTools/smoothSurf2D.m,v $
%%% Local Variables:
%%% mode: Matlab
%%% fill-column: 76
%%% comment-column: 0
%%% End:

