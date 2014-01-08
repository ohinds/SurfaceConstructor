% calculates the surface area of a face
%
% a = faceArea(verts) 
%
% Oliver Hinds <oph@bu.edu>
% 2004-11-05

function a = faceArea(verts)
  
  a = 0.5*sqrt(-2*verts(1,1)*verts(1,1)*verts(2,3)*verts(3,3)-2*verts(1,1)*verts(2,2)*verts(2,2)*verts(3,1)-2*verts(1,1)*verts(3,2)*verts(3,2)*verts(2,1)-2*verts(1,2)*verts(1,2)*verts(2,3)*verts(3,3)-2*verts(1,1)*verts(1,1)*verts(2,2)*verts(3,2)+2*verts(2,1)*verts(1,2)*verts(3,1)*verts(2,2)+2*verts(2,1)*verts(3,2)*verts(3,1)*verts(1,2)-2*verts(2,1)*verts(3,2)*verts(3,1)*verts(2,2)-2*verts(1,2)*verts(2,3)*verts(2,2)*verts(1,3)+2*verts(1,2)*verts(2,3)*verts(2,2)*verts(3,3)+2*verts(1,2)*verts(2,3)*verts(3,2)*verts(1,3)+2*verts(1,2)*verts(3,3)*verts(2,2)*verts(1,3)-2*verts(1,2)*verts(3,3)*verts(3,2)*verts(1,3)+2*verts(1,2)*verts(3,3)*verts(3,2)*verts(2,3)+2*verts(2,2)*verts(1,3)*verts(3,2)*verts(2,3)-2*verts(2,2)*verts(3,3)*verts(3,2)*verts(2,3)-2*verts(1,1)*verts(2,3)*verts(2,1)*verts(1,3)+2*verts(1,1)*verts(2,3)*verts(2,1)*verts(3,3)+2*verts(1,1)*verts(2,3)*verts(3,1)*verts(1,3)+2*verts(1,1)*verts(3,3)*verts(2,1)*verts(1,3)-2*verts(1,1)*verts(3,3)*verts(3,1)*verts(1,3)+2*verts(1,1)*verts(3,3)*verts(3,1)*verts(2,3)+2*verts(2,1)*verts(1,3)*verts(3,1)*verts(2,3)+2*verts(2,1)*verts(3,3)*verts(3,1)*verts(1,3)-2*verts(2,1)*verts(3,3)*verts(3,1)*verts(2,3)-2*verts(2,1)*verts(2,1)*verts(1,2)*verts(3,2)-2*verts(2,1)*verts(1,2)*verts(1,2)*verts(3,1)-2*verts(3,1)*verts(3,1)*verts(1,2)*verts(2,2)-2*verts(1,2)*verts(2,3)*verts(2,3)*verts(3,2)-2*verts(1,2)*verts(3,3)*verts(3,3)*verts(2,2)-2*verts(2,2)*verts(2,2)*verts(1,3)*verts(3,3)-2*verts(2,2)*verts(1,3)*verts(1,3)*verts(3,2)+2*verts(1,1)*verts(3,2)*verts(3,1)*verts(2,2)-2*verts(1,1)*verts(3,2)*verts(3,1)*verts(1,2)+2*verts(1,1)*verts(3,2)*verts(2,1)*verts(1,2)+2*verts(1,1)*verts(2,2)*verts(3,1)*verts(1,2)+2*verts(1,1)*verts(2,2)*verts(2,1)*verts(3,2)-2*verts(1,1)*verts(2,2)*verts(2,1)*verts(1,2)+verts(1,1)*verts(1,1)*verts(2,2)*verts(2,2)+verts(1,1)*verts(1,1)*verts(3,2)*verts(3,2)+verts(2,1)*verts(2,1)*verts(1,2)*verts(1,2)+verts(2,1)*verts(2,1)*verts(3,2)*verts(3,2)+verts(3,1)*verts(3,1)*verts(1,2)*verts(1,2)+verts(3,1)*verts(3,1)*verts(2,2)*verts(2,2)+verts(1,2)*verts(1,2)*verts(2,3)*verts(2,3)+verts(1,2)*verts(1,2)*verts(3,3)*verts(3,3)+verts(2,2)*verts(2,2)*verts(1,3)*verts(1,3)+verts(2,2)*verts(2,2)*verts(3,3)*verts(3,3)+verts(3,2)*verts(3,2)*verts(1,3)*verts(1,3)+verts(3,2)*verts(3,2)*verts(2,3)*verts(2,3)+verts(1,1)*verts(1,1)*verts(2,3)*verts(2,3)+verts(1,1)*verts(1,1)*verts(3,3)*verts(3,3)+verts(2,1)*verts(2,1)*verts(1,3)*verts(1,3)+verts(2,1)*verts(2,1)*verts(3,3)*verts(3,3)+verts(3,1)*verts(3,1)*verts(1,3)*verts(1,3)+verts(3,1)*verts(3,1)*verts(2,3)*verts(2,3)-2*verts(3,2)*verts(3,2)*verts(1,3)*verts(2,3)+2*verts(2,2)*verts(3,3)*verts(3,2)*verts(1,3)-2*verts(1,1)*verts(2,3)*verts(2,3)*verts(3,1)-2*verts(1,1)*verts(3,3)*verts(3,3)*verts(2,1)-2*verts(2,1)*verts(2,1)*verts(1,3)*verts(3,3)-2*verts(2,1)*verts(1,3)*verts(1,3)*verts(3,1)-2*verts(3,1)*verts(3,1)*verts(1,3)*verts(2,3));
return


%************************************************************************%
%%% $Source: /home/cvs/PROJECTS/SurfaceConstructor/tools/matlabSurfTools/faceArea.m,v $
%%% Local Variables:
%%% mode: Matlab
%%% fill-column: 76
%%% comment-column: 0
%%% End:
