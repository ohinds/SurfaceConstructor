% isSurf checks a surf struct to see if it is a propoerly
% preprocessed surface structure.
%
% bool = isSurf(surfStruct)
%
% Oliver Hinds <oph@bu.edu>
% 2005-10-24

function bool = isSurf(surfStruct)
  bool = 0;
  
  if(nargin < 1)
    fprintf('usage: bool = isSurf(surfStruct)\n');
    return;
  end

  if(~isfield(surfStruct,'vertices') | ~isfield(surfStruct,'faces'))
    fprintf('error: surfStruct should have at least "vertices" and "faces" fields. exit\n');
    return;
  end

  if(~isfield(surfStruct,'PREPROCESSED'))
    surfStruct = preprocessQ(surfStruct);
  end

  if(~strcmp('True',surfStruct.PREPROCESSED))
    fprintf('error: surface has non-manifold vertices. exit\n');
    return;
  end
  
  bool = 1;
return

%************************************************************************%
%%% $Source: /home/cvs/PROJECTS/SurfaceConstructor/tools/matlabSurfTools/isSurf.m,v $
%%% Local Variables:
%%% mode: Matlab
%%% fill-column: 76
%%% comment-column: 0
%%% End:
