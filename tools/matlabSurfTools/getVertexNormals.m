% vertexNormals = getVertexNormals(surfStruct)
% Oliver Hinds <oph@bu.edu>
% 2005-03-24

function [vertexNormals, faceNormals] = getVertexNormals(surfStruct)
  
  % calculate face normals 
  faceNormals = [];  
  for(face=1:size(surfStruct.faces,1))
    faceNormals(face,:) ...
	= getNormal(surfStruct.vertices(surfStruct.faces(face,:),:));
  end
  
  % average face normals of all faces incident on a vertex
  vertexNormals = [];
  for(vertex=1:size(surfStruct.vertices,1))
    vertexNormals(vertex,1:3) = ...
	mean(faceNormals(unique([find(surfStruct.faces(:,1) == vertex); ...
		    find(surfStruct.faces(:,2) == vertex); ...
		    find(surfStruct.faces(:,3) == vertex) ]),:));
    vertexNormals(vertex,:) ...
	= vertexNormals(vertex,:)/sqrt(sum(vertexNormals(vertex,:).^2));
  end
return

function normal = getNormal(vertices)
  normal = cross(vertices(1,:)-vertices(2,:),vertices(1,:)-vertices(3,:));
  normal = normal/sqrt(sum(normal.^2));
return
  
%************************************************************************%
%%% $Source: /home/cvs/PROJECTS/SurfaceConstructor/tools/matlabSurfTools/getVertexNormals.m,v $
%%% Local Variables:
%%% mode: Matlab
%%% fill-column: 76
%%% comment-column: 0
%%% End:
