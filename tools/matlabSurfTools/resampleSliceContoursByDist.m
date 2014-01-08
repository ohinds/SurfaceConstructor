% sl = resampleSliceContours(sl,cl)
% resamples a set of contours in the x and y directions to be a
% multiple of the sampling rate in the z-direction, assuming that the
% sampling in the z sampling rate is constant across all slices
%
% Oliver Hinds <oph@bu.edu>
% 2006-05-15

function sl2 = resampleSliceContoursByDist(sl,cl,mult)
  
  if(nargin < 2)
    fprintf('usage: sl2 = resampleSliceContours(sl,cl[,mult]);');
    return
  end
  
  if(nargin < 3)
    mult = 1.0;
  end
  
  % set the sampling rate by the z direction 
  sr = mult*abs(sl{1}{1}(1,3)-sl{2}{1}(1,3));
  
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
      d = 0;
      oldd = 0;
      for(v=2:size(cont,1))
	d = d + sqrt(sum((cont(v,1:2)-cont(v-1,1:2)).^2));
	
	while(d >= sr) 
	  r = (sr-oldd)/(d-oldd);
	  newCont(end+1,:) = cont(v-1,:) + r*(cont(v-1,:)-cont(v,:));
	  d = d - sr;
	end
	
	oldd = d;
      end
      
      sl2{sind}{cind} = newCont;
    end
  end
return

%************************************************************************%
%%% $Source: /home/cvs/PROJECTS/SurfaceConstructor/tools/matlabSurfTools/resampleSliceContoursByDist.m,v $
%%% Local Variables:
%%% mode: Matlab
%%% fill-column: 76
%%% comment-column: 0
%%% End:
