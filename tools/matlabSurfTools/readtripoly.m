% Oliver Hinds <oph@bu.edu>
% 2005-08-22

function [n,e] = readtripoly(filename)
  if(nargin < 1) 
    fprintf('usage: readtripoly(filename)\n');
    return;
  end

  % open the poly file
  fp = fopen(filename,'r');
  
  if(fp == -1)     
    fprintf('couldnt open file %s\n', filename);
    return;
  end    
  
  % read the nodes
  nn = fscanf(fp,'%d',1); % read num nodes
  fscanf(fp,'%d',3); % skip trash
  
  % read each node
  n = [];
  for(i=1:nn)
    fscanf(fp,'%f',1);
    n(i,:) = fscanf(fp,'%f',2);
  end
  
  % read the edges
  ne = fscanf(fp,'%d',1); % read num edges
  fscanf(fp,'%d',1); % skip trash
  
  % read the connectivity
  e = [];
  for(i=1:ne)
    fscanf(fp,'%f',1);
    e(end+1,:) = fscanf(fp,'%f',2);
    
    if(e(end,2) == -1)
      fscanf(fp,'%f',2);
      e(end,:) = [];
    end
  end

  fclose(fp);
return
