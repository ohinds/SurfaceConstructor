% surfSlicer.m takes a surface and produces parallel slices, each
% of which is the intersection of a plane with the surface.
% Oliver Hinds <oph@bu.edu>
% 2004-07-27

function slices = surfSlicer(surfStruct, dist, dim)
  surfStruct = preprocessQ(surfStruct);

  debug = 0;
  stoplater = 0;

  % get vertices and faces
  v = surfStruct.vertices;
  f = surfStruct.faces;

  % create a sorted face list for individual edges
  fsort = sort(f,2);
  fsort12 = fsort(:,[1 2]);
  fsort23 = fsort(:,[2 3]);

  % extract the unique edges from the faces list
  e = unique(sort([f(:,[1 2]); f(:,[2 3]); f(:,[1 3])],2),'rows');

  % find the number of slices
  minVal = min(v,[],1);
  minVal = minVal(dim);

  maxVal = max(v,[],1);
  maxVal = maxVal(dim);

  numSlices = ceil((maxVal-minVal)/dist);

  % slice along the dimension
  slices = {};
  for(s=1:numSlices)
    slicePlane = getSlicePlane((minVal-0.0001)+s*dist,dim);
    slices{s} = {};

    % build a list of intersections of the faces with the plane
    faceIntersectionGraph = zeros(size(e,1));
    edgesIntersected = zeros(size(e,1),2);
    facesIntersected = [];
    verticesIntersected = [];
    edgesPicked = [];

    for(i=1:size(f,1))
      % vector to hold the intersection ratios for each of the edges
      intRatios = [];
      edges = [];

      % test edges for intersection with the plane

      % edge 1
      edges(1) = strmatch(sort([f(i,1) f(i,2)]), e);
      if(edgesIntersected(edges(1),1))
	intRatios(1) = edgesIntersected(edges(1),2);
      else
	intRatios(1) = intersectPlaneEdge(slicePlane, ...
				 [v(e(edges(1),1),:); v(e(edges(1),2),:)]);
	edgesIntersected(edges(1),:) = [1 intRatios(1)];
      end

      % edge 2
      edges(2) = strmatch(sort([f(i,2) f(i,3)]), e);
      if(edgesIntersected(edges(2),1))
	intRatios(2) = edgesIntersected(edges(2),2);
      else
	intRatios(2) = intersectPlaneEdge(slicePlane, ...
				 [v(e(edges(2),1),:); v(e(edges(2),2),:)]);
	edgesIntersected(edges(2),:) = [1 intRatios(2)];
      end

      % edge 3
      edges(3) = strmatch(sort([f(i,1) f(i,3)]), e);
      if(edgesIntersected(edges(3),1))
	intRatios(3) = edgesIntersected(edges(3),2);
      else
	intRatios(3) = intersectPlaneEdge(slicePlane, ...
				 [v(e(edges(3),1),:); v(e(edges(3),2),:)]);
	edgesIntersected(edges(3),:) = [1 intRatios(3)];
      end

%      if(s == 39 & find(edges == 149))
%	stoplater = 1;
%	keyboard
%      end

      % find the valid intersections
      whichEdgesIntersected = edges(find(intRatios >= 0 & intRatios <= 1));

      % take different actions based on number of edges intersected
      if(length(whichEdgesIntersected) == 3)
	fprintf('found an intersection with three edges\n');

	% note that this could be trouble since it means we
        % intersected a vertex and have to watch out for which edge
        % we pick here

	equalEdges = edges(find(intRatios == 1));
	if(length(equalEdges) == 0)
	  equalEdges = edges(find(intRatios == 0));
	end

	% find the other faces these edges intersect
	% the idea here is to keep any intersections that have
        % faces adjacent to the edges. if neither edge has a face
        % adjaccent, just keep the first one.

	% first face
	tmpFace = strmatch(e(equalEdges(1),:),fsort12);
	if(length(tmpFace) == 0)
	  tmpFace = strmatch(e(equalEdges(1),:),fsort23);
	end
	if(length(tmpFace) == 0)
	  adjFaces(1) = -1;
	else
	  adjFaces(1) = tmpFace;
	end

	% second face
	tmpFace = strmatch(e(equalEdges(2),:),fsort12);
	if(length(tmpFace) == 0)
	  tmpFace = strmatch(e(equalEdges(2),:),fsort23);
	end
	if(length(tmpFace) == 0)
	  adjFaces(2) = -1;
	else
	  adjFaces(2) = tmpFace;
	end

	% resolve which edges actually intersect based on adjacent
        % face info
	if(adjFaces(2) == -1)
	  whichEdgesIntersected = setdiff(whichEdgesIntersected,adjFaces(2));
	elseif(adjFaces(1) == -1)
	  whichEdgesIntersected = setdiff(whichEdgesIntersected,adjFaces(1));
	else % all three intersected
	  faceIntersectionGraph(whichEdgesIntersected(1),...
				whichEdgesIntersected(3)) = 1;
	  faceIntersectionGraph(whichEdgesIntersected(3),...
				whichEdgesIntersected(1)) = 1;
	  faceIntersectionGraph(whichEdgesIntersected(2),...
				whichEdgesIntersected(3)) = 1;
	  faceIntersectionGraph(whichEdgesIntersected(3),...
				whichEdgesIntersected(2)) = 1;
	end
      end

      % normal case of two intersections
      if(length(whichEdgesIntersected) > 1)
        facesIntersected(end+1) = i;

	% store that this face intersected and the weight
	faceIntersectionGraph(whichEdgesIntersected(1),...
			      whichEdgesIntersected(2)) = 1;
	faceIntersectionGraph(whichEdgesIntersected(2),...
			      whichEdgesIntersected(1)) = 1;
      elseif(length(whichEdgesIntersected) == 1)
	fprintf('found an intersection with one edge\n');
	keyboard
      end
    end

    if(debug)
      showFace(surfStruct,facesIntersected),hold on;
    end
    % consider face topology and add the intersection of the edges
    % as points on the contour
    sliceEdges = find(edgesIntersected(:,2) >= 0 & edgesIntersected(:,2) <= 1);
    while(length(sliceEdges))
      slices{s}{end+1} = [];

      if(stoplater)
	keyboard
      end

      % find the edge of a contour if its not closed
      curEdge = find(sum(faceIntersectionGraph) == 1);
      if(length(curEdge) == 0) % closed contour, break it
	curEdge = sliceEdges(1);
      else
	curEdge = curEdge(1);
      end

      prevEdges = [];

      % add the first edge intersection
      while(length(curEdge))
	slices{s}{end}(end+1,1:3) = getPointOnSegment([v(e(curEdge,1),:); ...
		    v(e(curEdge,2),:)], ...
                    edgesIntersected(curEdge,2));
	[val ind] = unique(slices{s}{end},'rows');
	slices{s}{end} = slices{s}{end}(sort(ind),:);

	if(debug)
	  plot3(slices{s}{end}(:,1), slices{s}{end}(:,2),slices{s}{end}(:,3));
	  pause(.001);
	end
%	keyboard

	nextEdge = setdiff(find(faceIntersectionGraph(:,curEdge) ~= 0),...
			   prevEdges);

	% disconnect the contour if needed
	if(length(prevEdges) == 0 ...
	   & length(find(faceIntersectionGraph(:,curEdge) ~= 0)) > 1)
	  nextEdge = nextEdge(1);
	end
%	pause;

	sliceEdges(find(sliceEdges == curEdge)) = [];

	% zero this intersection
% 	if(length(prevEdges))
% 	  faceIntersectionGraph(curEdge,prevEdges(end)) = 0;
% 	  faceIntersectionGraph(prevEdges(end),curEdge) = 0;
% 	end

	prevEdges(end+1) = curEdge;
	curEdge = nextEdge;
      end
    end

    % print some progress information
    if(s > 1)
      if(s > round(100*10/numSlices))
	fprintf('\10');
      end
      fprintf('\10\10');
    end
    fprintf('%d%%',round(100*s/numSlices));
  end
  fprintf('\n');
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
  TOL = 0.0000000001;

  den = (edge(2,:) - edge(1,:)) * plane.n';

  % test for coplanar
  if(abs(den) < TOL)
    r = -1;
    return;
  end

  r = -(edge(1,:)*plane.n'-plane.p*plane.n')/den;
return

% gets the normal and a vertex on a plane given a dimention along which the
% plane is constant and its value along that dimension
function plane = getSlicePlane(val,dim)
  % normal vector
  plane.n = zeros(1,3);
  plane.n(dim) = 1;

  % point on plane
  plane.p = zeros(1,3);
  plane.p(dim) = val;
return
