% writes a slice contour set to a file
% Oliver Hinds <oph@bu.edu>
% 2005-08-22

function writecontours(slices, filename)
  if(nargin < 2)
    fprintf('usage: writecontours(slices,filename)\n');
  end

  % open the file
  fp = fopen(filename,'w');
  if(fp == -1)
    fprintf('couldn''t open file %s for writing\n',filename);
  end
  
  % write the number of contours
  
end

%************************************************************************%
%%% $Source: /home/cvs/PROJECTS/SurfaceConstructor/tools/matlabSurfTools/writecontours.m,v $
%%% Local Variables:
%%% mode: Matlab
%%% fill-column: 76
%%% comment-column: 0
%%% End:
