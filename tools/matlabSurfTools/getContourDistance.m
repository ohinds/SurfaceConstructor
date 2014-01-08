% getContourDistance computes the distance between two contours in
% shape space
%
% d = computeContourDistance(c1,c2[,method])
% D is the computed distance
% C1 is the first contour, represented as a list of vertex coords, Mx2
% C2 is the second contour, represented as a list of vertex coords, Nx2
% METHOD is the statistic used to compute the aggregate distance
%        this can be either 'mean' or 'median', with the default
%        being 'mean'. the statistic is used on the distance
%        between the vertices on C1 and the nearest point on C2. 
% 
% Oliver Hinds <oph@bu.edu>
% 2005-12-12

function d = getContourDistance(c1,c2,method)
  % validate
  if(nargin < 2)
    fprintf('usage: d = computeContourDistance(c1,c2[,method])\n');
    return
  end
  
  if(nargin < 3)
    method = 'median';
  end
  
  % one to two
  d_indiv_12 = contourDisplacement(c1,c2);
  
  if(strcmp(method,'median'))
    d_12 = median(d_indiv_12);
  else
    d_12 = mean(d_indiv_12);
  end
  
  % two to one
  d_indiv_21 = contourDisplacement(c2,c1);
  
  if(strcmp(method,'median'))
    d_21 = median(d_indiv_21);
  else
    d_21 = mean(d_indiv_21);
  end
  
  d = min([d_12 d_21]);
  
  d = log(d);
  
return

%************************************************************************%
%%% $Source: /home/cvs/PROJECTS/SurfaceConstructor/tools/matlabSurfTools/getContourDistance.m,v $
%%% Local Variables:
%%% mode: Matlab
%%% fill-column: 76
%%% comment-column: 0
%%% End:
