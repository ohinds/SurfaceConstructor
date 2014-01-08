% replaces a string in fieldnames with another string
% Oliver Hinds <oph@bu.edu>
% 2005-12-15

function replaceStrInMATVars(matfilename,oldstr,newstr,newmatfilename)

  s = load(matfilename);
  
  a = fieldnames(s);
  b = strrep(a,oldstr,newstr);
  
  t = struct();
  for(i=1:length(a))
    t = setfield(t,b{i},getfield(s,a{i}));
  end
  
  % save it again
  save(newmatfilename, '-STRUCT', 't');
  
return

%************************************************************************%
%%% $Source: /home/cvs/PROJECTS/SurfaceConstructor/tools/matlabSurfTools/replaceStrInMATVars.m,v $
%%% Local Variables:
%%% mode: Matlab
%%% fill-column: 76
%%% comment-column: 0
%%% End:
