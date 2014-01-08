% find shortest path uses dikstra's algorithm to find the sortest
% distance between two vertices on a mesh along the face edges.
%
% path = findShortestPath(surfStruct,verts)
%
% Oliver Hinds <oph@bu.edu>
% 2005-10-24

function path = findShortestPath(surfStruct,verts)

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

  % go from the first vertex to the second
  s = verts(1);
  queue = Inf*ones(surfStruct.V,1);
  distance = Inf*ones(size(queue,1),1);
  predecessor = -1*ones(size(queue,1),1);
  queue(s) = 0;
  u = s;
  
  while(length(queue) ~= 0 && u ~= verts(2))
    [u_dist, u] = min(queue);
    
    queue(u) = Inf;
    distance(u) = u_dist;
    
    % expand for neighbors
    neighbors = surfStruct.Neighbors(surfStruct.Index2(u):surfStruct.Index2(u)+surfStruct.NNoV(u)-1);
    neighbors(find(distance(neighbors) < Inf)) = [];
    
    % get distance of neighbors from first vert
    %keyboard
    neighborsDist = u_dist + sqrt(sum((repmat(surfStruct.vertices(u,:),length(neighbors),1)-surfStruct.vertices(neighbors,:)).^2,2));

    % add nodes that dont exist or are shoryter than existing ones
    % to the visited list
    %keyboard
    lower = find(queue(neighbors) > neighborsDist);
    if(~isempty(lower))
      queue(neighbors(lower)) = neighborsDist(lower);
      predecessor(neighbors(lower)) = u;
    end
  end

  % build the path
  if(u ~= verts(2))
    fprintf('verts 1 and 2 are not connected\n');
    return;
  end

  path = u;
  while(path(end) ~= verts(1))
    path(end+1) = predecessor(path(end));
  end
  path = flipud(path);

return

%************************************************************************%
%%% $Source: /home/cvs/PROJECTS/SurfaceConstructor/tools/matlabSurfTools/findShortestPath.m,v $
%%% Local Variables:
%%% mode: Matlab
%%% fill-column: 76
%%% comment-column: 0
%%% End:
