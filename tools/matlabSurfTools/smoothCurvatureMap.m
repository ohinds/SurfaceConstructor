% smooth curvature map takes a surface and a vertex curvature map
% and outputs a face curvature map with smooth boundaries
%
% Oliver Hinds <oph@bu.edu>
% 2005-10-21

function fcdata = smoothCurvatureMap(surfStruct,minind,maxind,verbose)
  
  % validate
  if(nargin < 1) 
    fprintf('usage: fcdata = smoothCurvatureMap(surfStruct)\n');
    return;
  end
  
  if(nargin < 2)
    minind = -1;
  end
  
  if(nargin < 3)
    maxind = -1;
  end
  
  if(nargin < 4)
    verbose = 0;
  end
  
  if(~isfield(surfStruct,'vertices') | ~isfield(surfStruct,'faces') | ~isfield(surfStruct,'vertexCurvature')) 
    fprintf('error: surfStruct should have at least "vertices", "faces", and "vertexCurvature" fields. exit\n');
    return;
  end
  
  if(~isfield(surfStruct,'PREPROCESSED'))
    surfStruct = preprocessQ(surfStruct);
  end
  
  if(~strcmp('True',surfStruct.PREPROCESSED)) 
    fprintf('error: surface has non-manifold vertices. exit\n');
    return;    
  end
 
  if(minind == -1)
    minind = 1; 
  end
  
  if(maxind == -1)
    maxind = surfStruct.V;
  end
  
  % smooth the vertex curvature over a certain number of jumps of
  % neighbors
  numNeighJumps = 3;
  vcdata = surfStruct.vertexCurvature;
  smoothedVCdata = vcdata;
  %keyboard
  for(v=minind:maxind)
    if(verbose)
      fprintf('smoothing curvature at vertex %d\n',v);
    end
    
    % build a neighbor list
    neighList = [];
    neighDistList = [];
    queue = v;
    while(~isempty(queue))
      n = queue(1);
      queue(1) = [];
      
      % find the jump distance
      t = find(neighList == n,1);
      if(isempty(t))
	d = 0;
      else
	d = neighDistList(t);
      end
      
      if(d > numNeighJumps)
	break;
      end
      
      localNeigh = surfStruct.Neighbors(surfStruct.Index2(n):surfStruct.Index2(n)+surfStruct.NNoV(n)-1);
      localNeigh(find(localNeigh == v)) = [];
      for(ln=1:length(localNeigh))
	visited = find(localNeigh(ln) == neighList);
	if(~isempty(visited))
	  localNeigh(ln) == [];
	end
      end
      
      neighList = vertcat(neighList,localNeigh);
      neighDistList = vertcat(neighDistList,(d+1)*ones(size(localNeigh)));
      queue = vertcat(queue,localNeigh);
    end
  
    % take the most common value of the neighbors
    smoothedVCdata(v) = mode(vcdata(neighList));
  end
  
  % iterate over faces, assigning each a curvature color
  fcdata = -ones(surfStruct.F,1);
  for(f=1:surfStruct.F)
    vc = smoothedVCdata(surfStruct.faces(f,:));
    if(vc(1) == vc(2) & vc(2) == vc(3))
      fcdata(f) = vc(1);
    end
  end
  
return

%************************************************************************%
%%% $Source: /home/cvs/PROJECTS/SurfaceConstructor/tools/matlabSurfTools/smoothCurvatureMap.m,v $
%%% Local Variables:
%%% mode: Matlab
%%% fill-column: 76
%%% comment-column: 0
%%% End:

