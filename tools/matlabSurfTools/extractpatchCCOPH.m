function [surfStruct2 origVertInds] = extractpatchCC(surfStruct, varargin)
% Extract a connected component submesh from a triangular mesh.
%
% To extract the largest component (i.e. with the most vertices):
% surfStruct = extractpatchCC(surfStruct)
%
% To extract the component which contains vertex n0:
% surfStruct = extractpatchCC(surfStruct, n0)
%
% Examples of this function's utility:
% 1) obviously, if the original mesh consists of two or more
% unconnected pieces, this function can be used to extract each
% piece. 
% 2) if you delete a number of triangles from an original mesh,
% leaving some vertices "dangling", i.e. these vertices are not
% part of any triangle, then this function will delete the dangling
% vertices, and relabel the faces array correctly. 



% PREPROCESSING NOTE: surfStruct need not be a manifold. This code
% calls neighborhood() and extractpatchQ(), which preprocesses the
% output surfStruct2.
%
checkSurface(surfStruct, mfilename, inputname(1));

% BEGIN MAIN
if nargin >= 2
  nbd = neighborhood(surfStruct, varargin{1});
  verts2extract = find(nbd ~= -1);
else
  V = size(surfStruct.vertices, 1);
  vertLabels = ones(V,1);
  
  maxCC = -Inf;
  n0 = 1;
  CONT = 1;
  while CONT
    
    nbd = neighborhood(surfStruct, n0);
    index = find(nbd ~= -1);
    if length(index) > maxCC
      maxCC = length(index);
      verts2extract = index;
    end
    vertLabels(index) = 0;

    index = find(vertLabels);
    if isempty(index)
      CONT = 0;
    else
      n0 = index(1);
    end
    
  end  

end

origVertInds = verts2extract;
surfStruct2 = extractpatchQ(surfStruct, verts2extract);
