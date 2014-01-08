% generate figures for hbm 2005 conference stria abstract
% 
% Oliver Hinds <oph@bu.edu>
% 2005-01-11

function getview(h)

  % h is the axes, get the camera properties
  fprintf('  set(gca,''cameraposition'',[%f, %f, %f]);\n',...
	  get(h,'cameraposition'));
  fprintf('  set(gca,''cameratarget'',[%f, %f, %f]);\n',...
	  get(h,'cameratarget'));
  fprintf('  set(gca,''cameraupvector'',[%f, %f, %f]);\n',...
	  get(h,'cameraupvector'));
  fprintf('  set(gca,''cameraviewangle'',%f);\n',...
	  get(h,'cameraviewangle'));

  % lights
  lts = findall(h,'type','light');
  for(i=1:length(lts))
    [az,el] = lightangle(lts(i));
    fprintf('  lightangle(%f,%f);\n',az,el);    
  end
return

%************************************************************************%
%%% $Source: /home/cvs/PROJECTS/SurfaceConstructor/tools/matlabSurfTools/getview.m,v $
%%% Local Variables:
%%% mode: Matlab
%%% fill-column: 76
%%% comment-column: 0
%%% End:
