% write a slice contour set to a file in the libsr file format
% Oliver Hinds <oph@bu.edu>
% 2006-04-21

function writeSliceContours(filename,slices,contourClosure,adjIndices)
  if(nargin < 3)
    fprintf('usage: writeSliceContours(filename,slices,contour_closure[,adjIndices])\n');
    return
  end

  % open the file
  fp = fopen(filename,'w');
  if(fp == -1)
    fprintf('couldn''t open file %s for writing\n',filename);
    return;
  end

  if(nargin < 4)
    adjIndices = [];
  end
  
  for(sl=1:length(slices))
    slice = slices{sl};
    closed = contourClosure{sl};
    
    % check for empty slice
    hasverts = 0;
    for(c=1:length(slice))
      if(~isempty(slice{c}))
	hasverts = 1;
      else
	slice(c) = [];
	closed(c) = [];
      end
    end
    if(~hasverts)
      continue;
    end
    
    % write the slice string */
    fprintf(fp, 'slice %0.6f\n', slice{1}(1,3));

    % write the contours
    for(c=1:length(slice))
      contour = slice{c};
      
      % print if its closed
      if(closed(c))
	fprintf(fp, 'contour closed\n');
      else
	fprintf(fp, 'contour open\n');
      end
      
      % write the vertices
      for(v=1:size(contour,1))
	fprintf(fp, '%0.6f %0.6f\n', contour(v,1), contour(v,2));	
      end
      
      % write adjacency
      fprintf(fp, 'adjacent\n');	
      if(~isempty(adjIndices))
	adj = adjIndices{sl}{c};
	for(a=1:length(adj))
	  fprintf(fp, '%d ', adj(a));		 
	end
      end
      fprintf(fp, '-1\n');	
    end
  end

  fclose(fp);
end

%************************************************************************%
%%% $Source: /home/cvs/PROJECTS/SurfaceConstructor/tools/matlabSurfTools/writeSliceContours.m,v $
%%% Local Variables:
%%% mode: Matlab
%%% fill-column: 76
%%% comment-column: 0
%%% End:
