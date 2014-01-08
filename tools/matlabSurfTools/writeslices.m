% writes a slice contour set to a file
% Oliver Hinds <oph@bu.edu>
% 2005-08-22

function writeslices(slices, filename)
  if(nargin < 2)
    fprintf('usage: writecontours(slices,filename)\n');
  end

  % open the file
  fp = fopen(filename,'w');
  if(fp == -1)
    fprintf('couldn''t open file %s for writing\n',filename);
  end
  
  % write the number of slices
  fprintf(fp,'%d\n', size(slices,2));

  % write each slice
  for(slice_ind=1:size(slices,2))
    slice = slices{slice_ind};

    fprintf(fp,'s %d %0.10f\n', size(slice,2), slice{1}(1,3));
    for(contour_ind=1:size(slice,2))
      contour = slice{contour_ind};
      fprintf(fp,'c %d\n', size(contour,1));
      for(vertex_ind=1:size(contour,1))
	fprintf(fp,'%0.10f %0.10f\n',contour(vertex_ind,1:2));
      end
    end
  end
  
  fclose(fp);
end

%************************************************************************%
%%% $Source: /home/cvs/PROJECTS/SurfaceConstructor/tools/matlabSurfTools/writeslices.m,v $
%%% Local Variables:
%%% mode: Matlab
%%% fill-column: 76
%%% comment-column: 0
%%% End:
