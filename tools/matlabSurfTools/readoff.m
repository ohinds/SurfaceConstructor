function surfStruct = readoff(filestring)
% Read in the vertices and faces of a file in Geomview OFF format
% surfStruct = readoff(filestring)
%
% Note 1: this function assumes that the *.off file is a very basic
% file, i.e. it just has the vertex and face info, and not any of the
% texture, color, etc. information that the OFF format supports.
%
% Note 2: assumes that all the faces are triangles. 

fptr = fopen(filestring,'r');

% *********** %
% Read header %
% *********** %
str = fscanf(fptr, '%s', 1);
numVertices = fscanf(fptr, '%d', 1);
numFaces = fscanf(fptr, '%d', 1);
numEdges = fscanf(fptr, '%d', 1);

% ************* %  
% Read vertices %
% ************* %
v = fscanf(fptr, '%f %f %f\n', [3 numVertices]);
v = v';
  
% *********************************** %
% Read faces: i.e. connectivity table %
% *********************************** %
f = fscanf(fptr, '%d %d %d %d\n', [4 numFaces]);
f = f';
f = f(:,[2 3 4]);
f = f + 1;

fclose(fptr);

surfStruct.vertices = v;
surfStruct.faces = f;



