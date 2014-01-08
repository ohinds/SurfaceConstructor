% find a boundary from labels
%
% bv = boundaryVerticesFromLabels(surfStruct,ordered)
%
% BV is a list of boundary vertex indices
% SURFSTRUCT is a preprocessed surface structure
% ORDERED is a Boolena flag indicating whether ordering of BV is necessary
%
% Oliver Hinds <oph@bu.edu>
% 2006-03-31

function bv = boundaryVerticesFromLabels(surfStruct,ordered)

  if(nargin < 2)
    ordered = 1;
  end

  if(ordered == 0)
    bv = find(surfStruct.vertexLabels == 1);

    % prune non boundary verts
    for(i=length(bv):-1:1)
      n = neighborsQ(surfStruct,bv(i));
      if(isempty(find(surfStruct.vertexLabels(n) ~= 1)))
	bv(i) = [];
      end
    end

  else
    [prunedSurf origVertInds] = pruneVertices(surfStruct,find(surfStruct.vertexLabels == 1));
    bv = origVertInds(boundaryVertices(prunedSurf));
  end

return
