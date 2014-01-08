function [ordered_bN, numBoundaries, ordered_bN_cell] = boundaryVertices(surfStruct, varargin)
% Find boundary vertices and order them.
%
% [ordered_bN, numBoundaries] = boundaryVertices(surfStruct, [n0], ['silent'])
% [ordered_bN, numBoundaries] = boundaryVertices(surfStruct,   [], ['silent'])
%
% n0 is optional; if specified, start traversal from n0.

% NOTES on algorithm:
%
% 1) we assume that every vertex is either interior w/ 
% NNoV - NTaV = 0, or a boundary vertex with NNoV - NTaV = 1.
%
% 2) consistentOrientation means Neighbors is such that a vertex is
% circumnavigated by its neighbors in a consistent way (consistent
% across vertices).
%
% For a boundary vertex n1, let the first item in its Neighbor-list be
% n2. Then for n2, n1 must be the LAST item in n2's Neighbor-list for
% the orientation to be consistent.
%
% 3) consistentFaces makes each triangle [n0 n1 n2], [n0 n2 n3],
% etc. where n0 is the source, n1-n3 are its ordered neighbors.
%
% So if, in a flattening, each triangle is ordered in an
% anti-clockwise manner, then the ordered boundary vertices will go
% around in an anti-clockwise manner.



% PREPROCESSING NOTE: surfStruct must be a MANIFOLD. If the boundary
% vertices are non-manifold, it is not possible to traverse them in
% order (robustly). This function therefore assumes a single component
% manifold, and uses the fact that the triangulation is oriented
% consistently. However, it could be generalized to work on
% non-manifold meshes for which the non-manifoldness occurs away from
% the boundary.
%
%checkSurface(surfStruct, mfilename, inputname(1), 'ccE', 'manifE');

%%% bN contains the labels of all the boundary vertices 
bN = find( surfStruct.NNoV - surfStruct.NTaV == 1 );
if isempty(bN)
  ordered_bN = []; %%% There is no boundary, we are done
  numBoundaries = 0;
  ordered_bN_cell = {};
  return
end
n = size(bN,1);

%%% n0 
n0_GIVEN = 0;
n0 = bN(1);
if nargin >= 2
  if ~isempty(varargin{1})
    n0 = varargin{1};
    n0_GIVEN = 1;
    if ~ismember(n0, bN)
      fprintf('\n');
      fprintf('%s: vertex %d is not a boundary vertex of %s\n', ...
	      mfilename, n0, inputname(1));
      error('Exiting ...');
    end
  end
end
% eliminate n0 from the original unordered list
indx = find(bN == n0);
bN(indx) = [];

%%% VERBOSITY
VERBOSE = 1;
if nargin >= 3
  if strncmpi(varargin{2}, 's', 1)
    VERBOSE = 0;
  end
end

counter = 0;
CONT = 1;
while CONT
  
  counter = counter + 1;

  %%% sort the boundary vertices into ordered_bN so that they
  %%% traverse the boundary of the surface
  ordered_bN = zeros(n,1);
  ordered_bN(1) = n0;

  i = 2; 
  QUIT = 0;
  while ~QUIT
    
    n_1 = ordered_bN(i-1);
    nextNode = surfStruct.Neighbors( surfStruct.Index2(n_1) );
    % Above, we assume a consistently oriented triangulation.
    % Otherwise, then we would have to search through the neighbors of
    % n_1 for another vertex on the boundary (but other than the one
    % prior to n_1)
    
    if nextNode == n0
      QUIT = 1;
    else
      % add nextNode to the ordered list
      ordered_bN(i) = nextNode;
      % eliminate nextNode from the original unordered list
      indx = find(bN == nextNode);
      bN(indx) = [];
      % increment i
      i = i + 1;
    end
    
  end

  %%% Resize the array to remove trailing zeros
  ordered_bN_cell{counter} = ordered_bN(1:i-1);

  if isempty(bN) 
    % i.e. there is a single boundary
    CONT = 0;
  else        
    if n0_GIVEN
      % i.e. multiple boundaries, but n0 was given, so STOP
      CONT = 0;
      if VERBOSE
	fprintf('\n');
	fprintf(' ------------------------------------------------------\n');
	fprintf(' %s: Multiple boundaries detected.\n', mfilename);
	fprintf(' --> Extracted boundary containing vertex %d.\n', n0);
	fprintf(' ------------------------------------------------------\n');
	fprintf('\n');
      end
    else
      % i.e. multiple boundaries, n0 not given, so extract another boundary
      CONT = 1;
      n0 = bN(1);
      % eliminate n0 from the original unordered list
      indx = find(bN == n0);
      bN(indx) = [];
    end
    
  end
  
end

% search for the largest boundary (trivial if counter == 1)
maxVal = 0;
for ccc = 1:counter
  if length(ordered_bN_cell{ccc}) > maxVal
    ordered_bN = ordered_bN_cell{ccc};
    maxVal = length(ordered_bN);
  end
end

% if counter > 1, report that largest boundary was chosen
if counter > 1
  if VERBOSE
    fprintf('\n');
    fprintf(' ------------------------------------------------------\n');
    fprintf(' %s: Multiple boundaries detected.\n', mfilename);
    fprintf(' --> Extracted boundary with most vertices.\n');
    fprintf(' ------------------------------------------------------\n');
    fprintf('\n');
  end
end

numBoundaries = counter;



