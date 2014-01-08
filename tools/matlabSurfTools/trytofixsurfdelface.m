% 
% Oliver Hinds <oph@bu.edu>
% 2006-04-27
function surf = trytofixsurfdelface(surf,mn)
  surf = preprocessQ(surf);
  
  nmv1 = find(surf.vertexManifoldness == mn);
  while(~isempty(nmv1))
    [n, nf] = showFacesAtVert(surf,nmv1(1));
    cameratoolbar
    fi = pickFace;
    fi = unique(fi);

    if(~isempty(fi))      
      [surf verts] = deleteFaceInd(surf,nf(fi));
      surf = preprocessQ(surf);
      nmv1 = find(surf.vertexManifoldness == mn);
    else
      return
    end

    close;
    

  end
return

%************************************************************************%
%%% $Source: /home/cvs/PROJECTS/SurfaceConstructor/tools/matlabSurfTools/trytofixsurfdelface.m,v $
%%% Local Variables:
%%% mode: Matlab
%%% fill-column: 76
%%% comment-column: 0
%%% End:
