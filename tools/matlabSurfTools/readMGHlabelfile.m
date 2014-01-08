function vertexLabels = readMGHlabelfile(surfStruct, filestring, quiet)
% Read MGH per-vertex labels (e.g. .w files)
%
% vertexLabels = readMGHsurfLabels(surfStruct, filestring)
%
% NOTE: this was cannibalized from the read_wfile.m in
% ~fischl/matlab on Jul 1, 2003. (So if the formats for MGH
% surface labels change, this function will become obsolete).
% 
% e.g. readMGHsurfLabels('eccen.w')

% (mukundb, Jul 1, 2003)
% (oph, 2006-02-24)

if(nargin < 3)
  quiet = 0;
end
  
V = size(surfStruct.vertices, 1);
vertexLabels = repmat(-1, V, 1);

% Open  ascii
fid = fopen(filestring,'r') ;

% test for successful open
if ( fid == -1 ),
  error('%s: error opening file [%s]', mfilename, filestring);
end;

% Read header
c = fscanf(fid,'%c',1);
while(~strcmp(c,sprintf('\n')))
  c = fscanf(fid,'%c',1);
end

numLabeledVerts = fscanf(fid,'%d',1);

if(~quiet)
  fprintf('reading %d vertex labels from %s\n',numLabeledVerts,filestring);
end
  
% Read body
for i = 1:numLabeledVerts
  
  vertex = fscanf(fid,'%d',1) + 1;
  fscanf(fid,'%f',3);
  label = fscanf(fid, '%f',1) ;  

  if(vertex > 0)
    vertexLabels(vertex) = label;
  end
  
end

% Close file
fclose(fid);

