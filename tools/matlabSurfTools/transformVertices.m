% transforms vertex coordinates by a transformation matrix
% Oliver Hinds <oph@bu.edu>
% 2006-02-06

function coords = transformVertices(coords,A)
  if(size(coords,2) == 2)
    coords(:,3) = 0;
    chop2 = 1;
  else
    chop2 = 0;
  end
  
  coords = A*[coords'; ones(1,size(coords,1))];
  coords = coords(1:3,:)';
  
  if(chop2)
    coords = coords(:,1:2);
  end
return

%************************************************************************%
%%% $Source: /home/cvs/PROJECTS/SurfaceConstructor/tools/matlabSurfTools/transformVertices.m,v $
%%% Local Variables:
%%% mode: Matlab
%%% fill-column: 76
%%% comment-column: 0
%%% End:
