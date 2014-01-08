% writeNuagesSlices.m takes a set of slices produced by surfSlicer and
% writes out a file that can be used by nuages to perform surface
% reconstruction 
% Oliver Hinds <oph@bu.edu>
% 2004-07-28

function writeNuagesSlices(slices,filename,dim)
  % open the file
  fp = fopen(filename,'w+');
    
  % validate file
  if(fp == -1)
    return
  end
  
  % print the number of slices 
  numSlices = size(slices,2);
  fprintf(fp, 'S %d\n', numSlices);

  % write each set of contours 
  for(i=1:numSlices) 
    curSlice = slices{i};
%    keyboard
    numContours = size(curSlice,2);

    % determine total number of points in the contour 
    totalTacks = 0;
    for(j=1:numContours) 
      totalTacks = totalTacks + size(curSlice{j},1);
    end

    % print the number of vertices and the z value 
    fprintf(fp, 'v %d z %g\n', totalTacks, curSlice{1}(1,dim));

    % print each contour on the slice 
    for(j=1:numContours) 
      curContour = flipud(curSlice{j});

      curContour(:,dim) = [];
      numTacks = size(curContour,1);

      fprintf(fp, '{\n');
      % print each vertex 
      for(k=1:numTacks) 
	fprintf(fp, '%g %g\n', curContour(k,1), curContour(k,2));
      end
      fprintf(fp, '}\n');
    end
  end


return