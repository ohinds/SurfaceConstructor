% surfslicer2 implements a more efficient surface slicer
% [slices closed] = surfSlicer2(surf,dist,dir)
% Oliver Hinds <oph@bu.edu>
% 2006-04-20

function [slices closed labels] = surfSlicer2(surf,dist,dir)

  if(nargin < 3)
    dir=3;
  end
  
  A = eye(4);
  if(dir == 1)
    A = [0 1 0 0; 0 0 1 0; 1 0 0 0; 0 0 0 1];
  elseif(dir == 2)
    A = [0 0 1 0; 1 0 0 0; 0 1 0 0; 0 0 0 1];    
  end
  
  surf.vertices = transformVertices(surf.vertices,A);
  surf = preprocessQ(surf);

  bv = boundaryVertices(surf);

  % create labels if they dont exist
  if(~isfield(surf,'vertexLabels'))
    surf.vertexLabels = -1*ones(size(surf.vertices,1),1);
  end
  
  v = surf.vertices;
  V = size(v,1);

  f = surf.faces;
  F = size(f,1);

  % create a sorted face list for individual edges
  fsort = sort(f,2);
  fsort12 = fsort(:,[1 2]);
  fsort23 = fsort(:,[2 3]);

  % extract the unique edges from the faces list
  e = unique(sort([f(:,[1 2]); f(:,[2 3]); f(:,[1 3])],2),'rows');
  E = size(e,1);

  % get the number and location of slices
  minSl = min(v(:,3));
  maxSl = max(v(:,3));
  numSl = ceil((maxSl-minSl)/dist - 1);
  sl = minSl+dist/2:dist:maxSl-dist/2;

  % create each slice
  slices = {};
  closed = {};
  labels = {};
  for(slind=1:length(sl))

    slice = sl(slind);
    fprintf('creating slice %d of %d at %f\n', slind, numSl, slice);

    slices{slind} = {};
    labels{slind} = {};
    closed{slind} = [];
    plane = getSlicePlane(slice,3);
    curContour = 0;

    % find contours
    facelist = ones(F,1);
    for(faceind=1:F)
      % skip if visited
      if(facelist(faceind) == 0)
	continue;
      end

      face = f(faceind,:);
      edges = [face([1 2]);face([2 3]);face([3 1])];
      foundedge = 0;
      for(e=1:3)
	edge = edges(e,:);
	r = intersectPlaneEdge(plane,v(edge,:));
	if(r >= 0 & r <= 1)
	  %keyboard
	  origedge = edge;
	  foundedge = 1;
	  break;
	end
      end

      facelist(faceind) = 0;

      % no intersection, go to next face
      if(~foundedge)
	continue;
      end

      % intersection, find the whole contour
      curContour = curContour+1;
      fprintf('creating contour %d of slice %d\n', curContour, slind);
      slices{slind}{curContour} = getPointOnSegment(v(edge,:),r);
      
      % label, if we need to
      if(surf.vertexLabels(edge(1)) == 1 && surf.vertexLabels(edge(2)) ==1)
	labels{slind}{curContour} = 1;
      else 
	labels{slind}{curContour} = -1;
      end
      
      % find if this edge is on the boundary
      firstonboundary = ~isempty(find(bv == edge(1))) & ~isempty(find(bv == edge(2)));

      % find adjacent faces that intersect plane
      origface = faceind;
      foundorig = 0;
      swaped = 0;
      while(~foundorig)

	face = f(faceind,:);
	lastedge = edge;
	edges = [face([1 2]);face([2 3]);face([3 1])];
	foundedge = 0;
	for(e=1:3)
	  edge = edges(e,:);

	  % check for edge we just came from
	  if((edge(1) == lastedge(1) & edge(2) == lastedge(2)) ...
	     | (edge(2) == lastedge(1) & edge(1) == lastedge(2)))
	    continue;
	  end

	  r = intersectPlaneEdge(plane,v(edge,:));
	  if(r >= 0 & r <= 1)
	    foundedge = 1;
	    break;
	  end
	end

	% no intersection, there must be an error
	if(~foundedge)
	  fprintf('error! couldnt have found no intersection here!');
	else
	  slices{slind}{curContour}(end+1,:) = getPointOnSegment(v(edge,:),r);
	  facelist(faceind) = 0;
	  
	  % label, if we need to
	  if(surf.vertexLabels(edge(1)) == 1 && surf.vertexLabels(edge(2)) ==1)
	    labels{slind}{curContour}(end+1) = 1;
	  else 
	    labels{slind}{curContour}(end+1) = -1;
	  end
	  
	end

	% swap if non manifold
	if(~strcmp(surf.vertexManifoldness,'N/A'))
	  if(surf.vertexManifoldness(edge(1)) ~= 0)
	    edgeswap = edge(1);
	    edge(1) = edge(2);
	    edge(2) = edgeswap;
	  end
	end
	
	i2 = surf.Index2(edge(1));
	o = find(surf.Neighbors(i2:i2+surf.NNoV(edge(1))-1) == edge(2));
	otherfaceind = [surf.LT1aNE(i2+o-1) surf.LT2aNE(i2+o-1)];

	faceind = otherfaceind(find(otherfaceind ~= faceind));

	% termination conditions
	if(faceind == origface)
	  foundorig = 1;
	  closed{slind}(curContour) = 1;
	  break;
	elseif(faceind == -1 | faceind == 0 | facelist(faceind) == 0)
	  if(~swaped & ~firstonboundary) % go the other direction
	    swaped = 1;
	    slices{slind}{curContour} = flipud(slices{slind}{curContour});
	    labels{slind}{curContour} = flipud(labels{slind}{curContour});

	    % reset the slicing process
	    faceind = origface;
	    edge = origedge;

	    % swap if non manifold
	    if(~strcmp(surf.vertexManifoldness,'N/A'))
	      if(surf.vertexManifoldness(edge(1)) ~= 0)
		edgeswap = edge(1);
		edge(1) = edge(2);
		edge(2) = edgeswap;
	      end
	    end

	    i2 = surf.Index2(edge(1));
	    o = find(surf.Neighbors(i2:i2+surf.NNoV(edge(1))-1) == edge(2));
	    otherfaceind = [surf.LT1aNE(i2+o-1) surf.LT2aNE(i2+o-1)];
	    faceind = otherfaceind(find(otherfaceind ~= faceind));
	    if(faceind == 0)
	      foundorig = 1;
	      closed{slind}(curContour) = 0;
	    end
	  
	  else
	    closed{slind}(curContour) = 0;
	    break;
	  end
	end
      end
    end

  end

  % flattend the labels
  lab = [];
  for(i=1:numel(labels))
    for(j=1:numel(labels{i}))
      lab = [lab labels{i}{j}];
    end  
  end  
  labels = lab';
return

% gets the point on a line segement a ratio r along the extent of
% the line
function p = getPointOnSegment(edge,r)
  p = edge(1,:) + r*(edge(2,:)-edge(1,:));
return

% tests for intersection of an edge and a plane
% returns less than 0 or more than 1 for no intersection, or ratio
% of distance along line that intersection occurs, if there is one
function r = intersectPlaneEdge(plane,edge)
  TOL = 0.0000000000001;

  den = (edge(2,:) - edge(1,:)) * plane.n';

  % test for coplanar
  if(abs(den) < TOL)
    r = -1;
    return;
  end

  r = -(edge(1,:)*plane.n'-plane.p*plane.n')/den;
return

% gets the normal and a vertex on a plane given a dimension along which the
% plane is constant and its value along that dimension
function plane = getSlicePlane(val,dim)
  % normal vector
  plane.n = zeros(1,3);
  plane.n(dim) = 1;

  % point on plane
  plane.p = zeros(1,3);
  plane.p(dim) = val;
return

%************************************************************************%
%%% $Source: /home/cvs/PROJECTS/SurfaceConstructor/tools/surfSlicer/surfSlicer2.m,v $
%%% Local Variables:
%%% mode: Matlab
%%% fill-column: 76
%%% comment-column: 0
%%% End:
