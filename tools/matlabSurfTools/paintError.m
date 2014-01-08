% paints error cdata onto a surface
%
% paintError(surfStruct,inflIter,smooth2DIter,smooth2DDim)
%
% Oliver Hinds <oph@bu.edu>
% 2005-04-29

function errcdata = paintError(surfStruct,flatSurf,errorPerVertex)
  
  % validate the input
  if(nargin < 3) 
    fprintf('usage: errcdata = paintError(surfStruct,flatSurf,errorPerVertex)\nexit\n');
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
   
  errcdata = errorPerVertex;
  %errcdata = errcdata./max(errcdata);

  figure,showSurf(surfStruct,errcdata);colormap('jet');
  colorbar;
  figure,showSurf(flatSurf,errcdata);colormap('jet');
  colorbar;

return

%************************************************************************%
%%% $Source: /home/cvs/PROJECTS/SurfaceConstructor/tools/matlabSurfTools/paintError.m,v $
%%% Local Variables:
%%% mode: Matlab
%%% fill-column: 76
%%% comment-column: 0
%%% End:
