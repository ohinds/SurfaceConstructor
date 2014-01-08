% read a slice contour set from a file
% Oliver Hinds <oph@bu.edu>
% 2005-08-22

function slices = readslices(filename)
  if(nargin < 1)
    fprintf('usage: slices = readslices(filename)\n');
  end

  % open the file
  fp = fopen(filename,'r');
  if(fp == -1)
    fprintf('couldn''t open file %s for reading\n',filename);
  end
  

  % read the number of slices
  ns = fscanf(fp,'%d',1);
  slices = cell(1,ns);

  % read each slice
  for(slice_ind=1:ns)
    fscanf(fp,'%s',1);
    nc = fscanf(fp,'%d',1);
    slices{slice_ind} = cell(1,nc);
    z = fscanf(fp,'%f',1);
    for(contour_ind=1:nc)
      slices{slice_ind}{contour_ind} = [];
      fscanf(fp,'%s',1); 
      nv = fscanf(fp,'%d',1);
      for(vertex_ind=1:nv)
	v = fscanf(fp,'%f %f',2);
	v(3) = z;
	slices{slice_ind}{contour_ind}(vertex_ind,:) = v';
      end
    end
  end
  
  fclose(fp);
end

%************************************************************************%
%%% $Source: /home/cvs/PROJECTS/SurfaceConstructor/tools/matlabSurfTools/readslices.m,v $
%%% Local Variables:
%%% mode: Matlab
%%% fill-column: 76
%%% comment-column: 0
%%% End:
