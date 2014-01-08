% compute a contour distance matrix for two slice contours
%
% dm = computeContourDistanceMatrix(s1,s2[,method])
%
% S1 is the slice one contours, in a length M cell array 
% S2 is the slice two contours, in a length N cell array 
% METHOD is either 'mean' [default] or 'median', which determines
% the statistic to use in computing a single contour distance
% DM is the MxN distance matrix
%
% Oliver Hinds <oph@bu.edu>
% 2005-12-12

function dm = computeContourDistanceMatrix(s1,s2,method)
  % arguments
  if(nargin < 2)
    fprintf('dm = computeContourDistanceMatrix(s1,s2,method)\n');
    return;
  end
  
  if(nargin < 3)
    method = 'mean';
  end
  
  % compute the distance between each contour
  m = length(s1);
  n = length(s2);
  
  dm = [];
  for(i=1:m)
    for(j=1:n)
      dm(i,j) = min(getContourDistance(s1{i},s2{j},method),...
		    getContourDistance(s2{j},s1{i},method));
    end
  end
  
end

%************************************************************************%
%%% $Source: /home/cvs/PROJECTS/SurfaceConstructor/tools/matlabSurfTools/computeContourDistanceMatrix.m,v $
%%% Local Variables:
%%% mode: Matlab
%%% fill-column: 76
%%% comment-column: 0
%%% End:
