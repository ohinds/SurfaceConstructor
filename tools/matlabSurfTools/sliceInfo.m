% sliceInfo.m takes a set of slices at prints some info
%
% SLICEINFO(SLICES) 
%   SLICES is a 2-D cell array (arranged as
%   slice-by-contour) containing 3xN matrices representing coordinates of
%   tack marks
% Oliver Hinds <oph@bu.edu>
% 2004-12-17

function sliceInfo(slices)
  
  format long;
  
  mx = -Inf*ones(length(slices),3);
  mn = -Inf*ones(length(slices),3);
  
  for(slice=1:length(slices))
    mx(slice,:) = max(vertcat(slices{slice}{:}));
    mn(slice,:) = min(vertcat(slices{slice}{:}));
  end
  
  fprintf('%d slices\nmax={%f %f %f}\nmin={%f %f %f}\n',length(slices),...
	  max(mx), min(mn));
return

%************************************************************************%
%%% $Source: /home/cvs/PROJECTS/SurfaceConstructor/tools/matlabSurfTools/sliceInfo.m,v $
%%% Local Variables:
%%% mode: Matlab
%%% fill-column: 76
%%% comment-column: 0
%%% End:
