% sl = resampleSliceContours(sl,cl)
% resamples a set of contours in the x and y directions by a
% minimum angle constraint.
%
% Oliver Hinds <oph@bu.edu>
% 2006-05-15

function sl2 = resampleSliceContoursByAngle(sl,cl,angle)
  
  if(nargin < 2)
    fprintf('usage: sl2 = resampleSliceContours(sl,cl[,angle]);');
    return
  end
  
  if(nargin < 3)
    angle = 0.2;
  end
  
  % resample each contour
  for(sind=1:length(sl))
    for(cind=1:length(sl{sind}))
      cont = sl{sind}{cind};
      
      % handle closed case
      if(cl{sind}(cind))
	cont(end+1,:) = cont(1,:);
      end      

      newCont = [];
      newCont(1,:) = cont(1,:);
      a = 0;
      for(v=3:size(cont,1))
	a = a + abs(pi-angleBetween(cont(v-2,:),cont(v-1,:),cont(v,:)));
	
	if(a >= angle) 
	  newCont(end+1,:) = cont(v,:);
	  a = 0;
	end
      end
      
      sl2{sind}{cind} = newCont;
    end
  end
return

% from
% http://www.csit.fsu.edu/~burkardt/m_src/geometry/angle_deg_2d.m
% by John Burkardt
function theta = angleBetween(a,b,c)
  p(1) = (c(1) - b(1)) * (a(1) - b(1)) + (c(2) - b(2)) * (a(2) - b(2));  
  p(2) = (c(1) - b(1)) * (a(2) - b(2)) - (c(2) - b(2)) * (a(1) - b(1));

  if(p(1) == 0.0 & p(2) == 0.0)
    theta = 0.0;
    return
  end

  theta = atan2(p(2), p(1));

  if(theta < 0.0)
    theta = theta + 2.0 * pi;
  end

  if(theta > pi)
    theta = 2.0 * pi - theta;
  end
return


%************************************************************************%
%%% $Source: /home/cvs/PROJECTS/SurfaceConstructor/tools/matlabSurfTools/resampleSliceContoursByAngle.m,v $
%%% Local Variables:
%%% mode: Matlab
%%% fill-column: 76
%%% comment-column: 0
%%% End:
