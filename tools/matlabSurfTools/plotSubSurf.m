% plotSubSurf plots a subset of the faces in a surfStruct
%
% PLOTSUBSURF(SURF,FACES)
%   SURF is a surface structure with M vertices and N faces
%   containing at least the members: vertices, a 3xM list of vertex
%   coordinates, and faces, a 3xN list of face vertex indices
%   FACES is a index vector of the faces to display
%
% Oliver Hinds <oph@bu.edu>
% 2004-11-05

function plotSubSurf(surf,faces)
  figure;
  subsurf.vertices = surf.vertices;
  subsurf.faces = surf.faces(faces,:);
  showSurf(subsurf);
return

%************************************************************************%
%%% $Source: /home/cvs/PROJECTS/SurfaceConstructor/tools/matlabSurfTools/plotSubSurf.m,v $
%%% Local Variables:
%%% mode: Matlab
%%% fill-column: 76
%%% comment-column: 0
%%% End:
