% prunes the input set of vertices from the given surface
%
% Oliver Hinds <oph@bu.edu>
% 2005-10-12

function [prunedSurf origVertInds] = pruneVertices(surf,verts)
  % validate
  if(nargin < 2)
    fprintf('usage: pruneVertices(surf,verts)\n');
  end
  
  prunedSurf = surf;
  
  verts = sort(verts,1,'descend');

  vertices = prunedSurf.vertices;
  delVertInds = [];
  V = size(prunedSurf.vertices,1);
  faces = prunedSurf.faces;
  
  for(i=1:length(verts))
    v = verts(i);

    [f c] = find(prunedSurf.faces == v);
    
    prunedSurf.faces(f,:) = [];
    prunedSurf.vertices(v,:) = [];
    delVertInds(end+1) = v;

    if(isfield(prunedSurf,'vertexLabels'))
      prunedSurf.vertexLabels(v,:) = [];
    end
    if(isfield(prunedSurf,'vertexCurvature'))
      prunedSurf.vertexCurvature(v,:) = [];
    end
    if(isfield(prunedSurf,'faceCurvature'))
      prunedSurf.faceCurvature(f,:) = [];
    end

    biggerfaces = find(prunedSurf.faces > v);
    prunedSurf.faces(biggerfaces) = prunedSurf.faces(biggerfaces)-1;  
  end
  
  origVertInds = 1:length(vertices);
  origVertInds(delVertInds) = [];

  prunedSurf = preprocessQ(prunedSurf);
  [prunedSurf origVertInds2] = extractpatchCCOPH(prunedSurf);
  origVertInds = origVertInds(origVertInds2);

  % fix the vertex labels and curvature data based on the rpindex
%   if(isfield(prunedSurf,'vertexLabels'))
%     prunedSurf.vertexLabels = ...
% 	prunedSurf.vertexLabels(prunedSurf.rpIndex);
%   end
%   if(isfield(prunedSurf,'vertexCurvature'))
%     prunedSurf.vertexCurvature = ...
% 	prunedSurf.vertexCurvature(prunedSurf.rpIndex);
%   end  
%   if(isfield(prunedSurf,'faceCurvature'))
%     %keyboard
    
%     % the following is stolen directly from extractpatchQ.m
%     nbdarray = zeros(V, 1);
%     nbdarray(prunedSurf.rpIndex) = 1; %%% e.g. [1 0 1 1 0] 
%     tmp = nbdarray(faces); %%% e.g. [1 1 1; 1 0 1; 1 1 1; 1 1 1]
%     tmp = sum(tmp,2);     %%% e.g. [3; 2; 3; 3]
%     tris = find(tmp == 3); %%% e.g. = [1 3 4]    
%     subf = faces(tris,:);
%     % end stealing
    
%     prunedSurf.faceCurvature = ...
% 	prunedSurf.faceCurvature(subf);
    
%   end
  
  prunedSurf = preprocessQ(prunedSurf);
  if(prunedSurf.vertexManifoldness ~= 'N/A')
    fprintf('error: surface is not manifold\n');
    prunedSurf = topoFixer(prunedSurf);
  end
  
return

%************************************************************************%
%%% $Source: /home/cvs/PROJECTS/SurfaceConstructor/tools/matlabSurfTools/pruneVertices.m,v $
%%% Local Variables:
%%% mode: Matlab
%%% fill-column: 76
%%% comment-column: 0
%%% End:
