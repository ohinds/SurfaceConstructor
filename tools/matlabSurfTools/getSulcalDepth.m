% getSulcalDepth finds the sulcal depth of each vertex in a surface
% Oliver Hinds <oph@bu.edu>
% 2005-10-12

function sulcalDepth = getSulcalDepth(surf)
  % validate
  if(nargin ~= 1) 
    fprintf('usage: sulcalDepth = getSulcalDepth(surf)\n');
    return 
  end
  
  % inflate the surface
  numIts = 1000;
  stepSize = 0.1;
  normalize = 0;
  fixBoundaries = 1;
  
  inflSurf = inflateQ(surf, numIts, stepSize, normalize, fixBoundaries,0);
  
  sulcalDepth = sum(inflSurf.vertices-surf.vertices,2);
return

%************************************************************************%
%%% $Source: /home/cvs/PROJECTS/SurfaceConstructor/tools/matlabSurfTools/getSulcalDepth.m,v $
%%% Local Variables:
%%% mode: Matlab
%%% fill-column: 76
%%% comment-column: 0
%%% End:
