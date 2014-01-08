% Oliver Hinds <oph@bu.edu>
% 2005-08-22

function writetripoly(filename,n,e)
  if(nargin < 2) 
    fprintf('usage: writetripoly(filename,n,[e])\n');
    return;
  end
  if(nargin < 3) 
    e = [];
  elseif(e == -1)
    e = [[1:size(n,1)]' [2:size(n,1) 1]']; 
  end
  
  % open the poly file
  fp = fopen(filename,'w');
  
  if(fp == -1)     
    fprintf('couldnt open file %s\n', filename);
    return;
  end    
  
  % write the nodes
  nn = size(n,1);
  fprintf(fp,'%d 2 0 0\n',nn); % num nodes
  
  % write each node
  for(i=1:nn)
    fprintf(fp,'%i %f %f\n',i,n(i,:));
  end
  
  % write the edges
  ne = size(e,1);
  fprintf(fp,'%d 0\n',ne); % num edges
  
  % write the connectivity
  for(i=1:ne)
    fprintf(fp,'%d %d %d\n',i,e(i,:));
  end

  fprintf(fp,'0');

  fclose(fp);
return
