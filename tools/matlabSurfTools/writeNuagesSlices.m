% writeNuagesSlices.m takes a set of slices and a filename and writes a set
% of slices to the file 
%
% WRITENUAGESSLICES(SLICES,FILENAME)
%   SLICES is a 2-D cell array (arranged as slice-by-contour)
%   containing 3xN matrices representing coordinates of tack marks
%   FILENAME is a string containing the name of the file the slices will
%   be wrote to
% Oliver Hinds <oph@bu.edu>
% 2004-08-18

function writeNuagesSlices(slices,filename)
  % open the file
  fp = fopen(filename,'w');

  % validate file
  if(fp == -1)
    return
  end

  % print header
  fprintf(fp, 'S %d\n', length(slices));

  % iterate over slices
  for(curSlice=1:length(slices))
    % print number of vertices in slice and z coord
    fprintf(fp, 'v %d z %f\n', size(vertcat(slices{curSlice}{:}),1), ...
	     slices{curSlice}{1}(1,3));
    
    % print out each contour
    for(curContour=1:size(slices{curSlice},2))
      fprintf(fp,'{\n');
      for(tack=1:size(slices{curSlice}{curContour},1))
	fprintf(fp, '%f %f\n', ...
		slices{curSlice}{curContour}(tack,1), ...	
		slices{curSlice}{curContour}(tack,2));	
      end
      fprintf(fp,'}\n');
    end    
  end


return

%************************************************************************%
%%% $Source: /home/cvs/PROJECTS/SurfaceConstructor/tools/matlabSurfTools/writeNuagesSlices.m,v $
%%% Local Variables:
%%% mode: Matlab
%%% fill-column: 76
%%% comment-column: 0
%%% End:
