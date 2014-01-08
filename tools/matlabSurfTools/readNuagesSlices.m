% readNuagesSlices.m takes a filename and reads a set of slices
% NOTE: compatibile with the surfSlicer
%
% SLICES = READNUAGESSLICES(FILENAME,DIM)
%   FILENAME is the name of a file containing valid input to the
%   nuages surface reconstruction application
%   DIM is the index of the dimension along which slices are taken,
%   from {1,2,3}. The slice position will be assigned to this
%   coordinate 
%   SLICES will be a 2-D cell array (arranged as slice-by-contour)
%   containing 3xN matrices representing coordinates of tack marks
% Oliver Hinds <oph@bu.edu>
% 2004-08-18

function slices = readNuagesSlices(filename,dim)
  % open the file
  fp = fopen(filename,'r');

  % validate file
  if(fp == -1)
    return
  end

  %
  slices = {};
  slices{1} = {};
  slices{1}{1} = [];
  curSlice = 1;
  curContour = 1;
  trash = fscanf(fp, 'S %d\nv %d z %f\n{');
  curZ = trash(3);
  
  % read all contours
  while(~feof(fp))
    str = fscanf(fp, '%s', 1);
    if(strcmp(str,'}'))
      str = fscanf(fp, '%s', 1);
      if(strcmp(str,'v'))
	trash = fscanf(fp, '%d z %f\n{',2);
	curZ = trash(2);
	
	curSlice = curSlice+1;
	curContour = 1;
	slices{curSlice} = {};
      else
	%str = fscanf(fp, '%s', 1);
	curContour = curContour+1;
      end
      slices{curSlice}{curContour} = [];

      continue;
    end

    % create the 
    slices{curSlice}{curContour}(end+1,:) = [0 0 0];
    
    % read the points
    str2 = fscanf(fp, '%s', 1);
    strs = {str str2};
    if(strcmp(str,'') | strcmp(str2,''))
      break;
    end
    
    for(i=1:3)
      if(i == dim)
	slices{curSlice}{curContour}(end,i) = curZ;
      else
	slices{curSlice}{curContour}(end,i) = str2num(strs{1});
	strs{1} = strs{2};
      end    
    end
  end

  slices{end} = {slices{end}{1:end-1}};

return

%************************************************************************%
%%% $Source: /home/cvs/PROJECTS/SurfaceConstructor/tools/matlabSurfTools/readNuagesSlices.m,v $
%%% Local Variables:
%%% mode: Matlab
%%% fill-column: 76
%%% comment-column: 0
%%% End:
