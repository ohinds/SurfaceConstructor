% performs a simple tiling based on the 3d delaunay triangulation
% of the slice contours
%
% s1 and s2 are cell arrays containing vertex lists of
% contours. There can be an arbitrary number of contours per
% slice. 
% surf is a struct containing the vertices and edges of a surface
% tiling of the input contours
% 
% Oliver Hinds <oph@bu.edu>
% 2005-08-22

function surf = delaunay_tiling(s1,s2)
  
  % validate input
  if(nargin < 2)
    fprintf('usage: surf = delaunay_tiling(c1,c2)\n');
    return;
  end
  
  % build triangle input for voronoi computation

  % slice 1
  in.pl =[];
  in.el = [];
  for(i=1:size(s1,2))
    n = size(s1{i},1);
    first = size(in.pl,1);
    in.pl(end+1:end+n,:) = s1{i}(:,1:2);
    in.el(end+1:end+n,:) = [[first:first+n-1]' [first+1:first+n-1 first]'];
  end
  in.pl = in.pl';
  in.el = in.el';
  v1 = triangle(in,'ves');

  % slice 2
  in.pl =[];
  in.el = [];
  for(i=1:size(s2,2))
    n = size(s2{i},1);
    first = size(in.pl,1);
    in.pl(end+1:end+n,:) = s2{i}(:,1:2);
    in.el(end+1:end+n,:) = [[first:first+n-1]' [first+1:first+n-1 first]'];
  end
  in.pl = in.pl';
  in.el = in.el';
  v2 = triangle(in,'ves');
  
  plotve(v1.vpl', v1.vel', v1.vnrl');
  plotve(v2.pl', v2.el', [],'r',gca);
  figure;
  plotve(v2.vpl', v2.vel', v2.vnrl','g',gca);
  plotve(v1.pl', v1.el', [], 'r',gca);
return

%************************************************************************%
%%% $Source: /home/cvs/PROJECTS/SurfaceConstructor/tools/matlabSurfTools/delaunay_tiling.m,v $
%%% Local Variables:
%%% mode: Matlab
%%% fill-column: 76
%%% comment-column: 0
%%% End:

