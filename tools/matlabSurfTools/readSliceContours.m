% read a slice contour set from a file writen in the surfrecon filre format
% Oliver Hinds <oph@bu.edu>
% 2005-08-22

function [slices contour_closure adjIndices labels] = readSliceContours(filename)
  if(nargin < 1)
    fprintf('usage: slices = readSliceContours(filename)\n');
  end

  % open the file
  fp = fopen(filename,'r');
  if(fp == -1)
    fprintf('couldn''t open file %s for reading\n',filename);
    return;
  end

  % try to read the label file
  haveLabels = 1;
  try
    labList = load([filename '.lab']);
  catch
    fprintf('couldn''t open label file %s.lab for reading\n',filename);
    haveLabels = 0;
  end

  slices = {};
  contour_closure = {};
  adjIndices = {};
  labels = {};
  curSlice = 1;
  
  % read the first string */
  fscanf(fp, 'slice ');

  % read the file */
  while(~feof(fp)) 
    % read the z coordinate */
    sliceZ = fscanf(fp, '%lf', 1);

    curContour = 1;

    % read the next string and test for new slice */
    str = fscanf(fp, '%s ', 1);
    while(strcmp(str,'contour'))  % while we are still reading contours */
      % read closure state */
      str = fscanf(fp, '%s', 1);

      slices{curSlice}{curContour} = [];      
      adjIndices{curSlice}{curContour} = [];      
      labels{curSlice}{curContour} = [];      
      contour_closure{curSlice}(curContour) = strcmp(str,'closed');

      % read the first string on the next line, test for 'adjacent' */
      str = fscanf(fp, '%s', 1);

      while(~strcmp(str,'adjacent'))
	% read this vertex */
	v = [];
	v(1) = str2num(str);
	v(2) = fscanf(fp, '%lf',1);
	v(3) = sliceZ;

	slices{curSlice}{curContour}(end+1,:) = v;

	str = fscanf(fp, '%s', 1);
	
	% try to read the label for this vertex
	if(haveLabels)
	  labels{curSlice}{curContour}(end+1,:) = labList(1);
	  labList(1) = [];
	end
      end

      % read the adjacent contours as ints, convert to pointers later */
      adjIndices{curSlice}{curContour}(end+1) = fscanf(fp, '%d', 1);
      while(adjIndices{curSlice}{curContour}(end) ~= -1) 
	adjIndices{curSlice}{curContour}(end+1) = fscanf(fp, '%d', 1);
      end

      str = fscanf(fp, '%s ', 1); 
      
      curContour = curContour+1;
    end
    curSlice = curSlice+1;
  end

  fclose(fp);
end

%************************************************************************%
%%% $Source: /home/cvs/PROJECTS/SurfaceConstructor/tools/matlabSurfTools/readSliceContours.m,v $
%%% Local Variables:
%%% mode: Matlab
%%% fill-column: 76
%%% comment-column: 0
%%% End:
