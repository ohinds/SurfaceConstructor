% write the label file assocciated with a surface
% 
% Oliver Hinds <oph@bu.edu>
% 2005-10-19

function writelabelfile(fname,vertexlabels)
  % validate
  if(nargin < 2)
    fprintf('usage: writelabelfile(fname,vertexlabels)\n');
    return;
  end
    
  fp = fopen(fname,'w');
  
  if(fp == -1)
    fprintf('couldn''t open file %s\n', fname);
    return;
  end    
  
  fprintf(fp,'%d\n',vertexlabels);
  
  fclose(fp);
return
