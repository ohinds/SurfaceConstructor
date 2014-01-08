function surfStruct = readobj(filestring)
% Read in vertices and faces in OBJ format
%
% surfStruct = readobj(filestring)

fptr = fopen(filestring, 'r');

vertices = [];
faces = [];

% ************** %
% Read body %
% ************** %
V = 0; F = 0;
CONT = 1;
while CONT
  str = fscanf(fptr, '%s', 1);
  if ~isempty(str)
    str = str(1);
  end
  switch str
   case '#'
    fgetl(fptr);
   case 'v'
    V = V + 1;
    vertices(V, :) = fscanf(fptr, '%f', 3)';
   case 'f'
    F = F + 1;
    faces(F, :) = fscanf(fptr, '%d', 3)';
   otherwise
    CONT = 0;
  end
end

surfStruct.vertices = vertices;
surfStruct.faces = faces;

fclose(fptr);

% read the label file, if there is one.
[fptr msg] = fopen([filestring '.lab']);

if(fptr == -1)
  filestring = filestring(1:end-4);
  [fptr msg] = fopen([filestring '.lab']);
end

if(fptr == -1)
  return;
end

surfStruct.vertexLabels = [];
CONT = 1;
while CONT
  str = fscanf(fptr, '%s', 1);
  if isempty(str) | feof(fptr)
    CONT = 0;
    continue;
  end
  
  if(str(1) == '#')
    fgetl(fptr);
  else
    surfStruct.vertexLabels(end+1) = str2num(str);
  end
end

surfStruct.vertexLabels = surfStruct.vertexLabels';

fclose(fptr);