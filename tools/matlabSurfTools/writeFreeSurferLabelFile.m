% writeFreeSurferLabelFile() writes a freesurfer label file with
% the nonnegatively labeled vertices in a surfstruct. the vertex
% index, ras coords, and label are writen for each labeled vertex.
%
% FNAME is the label file name to use when saving
% SURF is the surfStruct
% 
% Oliver Hinds <oph@bu.edu>
% 2006-01-25

function writeFreeSurferLabelFile(fname,surf)
  % validate
  if(nargin < 2)
    fprintf('usage: writeFreeSurferLabelFile(fname,surf)\n');
    return;
  end
    
  if(~isfield(surf,'vertexLabels'))
    fprintf('surf doenst have any vertex labels!\n');
    return;    
  end
  
  fp = fopen(fname,'w');
  
  if(fp == -1)
    fprintf('couldn''t open file %s\n', fname);
    return;
  end    
  
  writeVerts = find(surf.vertexLabels >= 0);
  
  % print the header
  fprintf(fp,'#!ascii label %s, from subject \n%d\n',fname,length(writeVerts));
  
  writeData = [writeVerts-1 surf.vertices(writeVerts,:) surf.vertexLabels(writeVerts)]';
  fprintf(fp,'%d %f %f %f %d\n',writeData);
  
  fclose(fp);
return
