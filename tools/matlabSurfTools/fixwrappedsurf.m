% fix a wrapped surface by translating all vertices after a given z
% coord and connecting the surfaces
% Oliver Hinds <oph@bu.edu>
% 2005-10-07

function newsurf = fixwrappedsurf(surf,z)
  % vaildate
  if(nargin < 2)
    fprintf('usage: surf = fixwrappedsurf(surf,z)\n');
    return
  end
  
  newsurf = surf;

  % translate the vertices
  r = find(newsurf.vertices(:,3) > z);
  fusez = min(newsurf.vertices(:,3));
  newsurf.vertices(r,3) = newsurf.vertices(r,3) - ...
      range(newsurf.vertices(:,3));
  fprintf('fixwrappedsurf: unwrapping %d vertices\n',length(r));
  
  % eliminate duplicate vertices
  fuseVerts = find(abs(newsurf.vertices(:,3)-fusez) < eps);
  
  for(v=1:length(fuseVerts))
    v1 = fuseVerts(v);
    if(v1 == -1) 
      continue;
    end
    
    for(w=v+1:length(fuseVerts))
      v2 = fuseVerts(w);
      if(v2 == -1) 
	continue;
      end

      if(sum(abs(newsurf.vertices(v1,:)-newsurf.vertices(v2,:))) < eps)
	
	fprintf('fixwrappedsurf: found duplicate vertex, correcting.\n',length(r));

	newsurf.faces(find(newsurf.faces == v2)) = v1;
	newsurf.vertices(v2,:) = [];
	newsurf.vertexLabels(v2,:) = [];

	biggerVerts = find(fuseVerts > v2);
	fuseVerts(biggerVerts) = fuseVerts(biggerVerts)-1;
	fuseVerts(w) = -1;
	
	biggerfaces = find(newsurf.faces > v2);
	newsurf.faces(biggerfaces) = newsurf.faces(biggerfaces)-1;
      end    
    end    
  end
return

%************************************************************************%
%%% $Source: /home/cvs/PROJECTS/SurfaceConstructor/tools/matlabSurfTools/fixwrappedsurf.m,v $
%%% Local Variables:
%%% mode: Matlab
%%% fill-column: 76
%%% comment-column: 0
%%% End:
