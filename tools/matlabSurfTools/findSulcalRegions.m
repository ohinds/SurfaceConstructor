% finds gyral/sulcal regions of a surface
%
% [faceIsSulcus, inflatedSurf] = findSulcalRegions(surfStruct[,iter,norm,bord])
%
% THIS DOESNT WORK AT ALL!!!!!!!!!!!!!!!!!!!
%
% FINDSULCALREGIONS determines which faces are in a sulcal region by
% inflating the surface passed and testing whether the displacement of
% each face is toward or away from the centroid of the surface. if it is
% away from the centroid, it is a sulcal face
% 
% Oliver Hinds <oph@bu.edu>
% 2005-04-29

function [faceIsSulcus, inflatedSurf] = findSulcalRegions(surfStruct,iter,norm,bord)

  % validate the input
  if(nargin < 1) 
    fprintf('error: surfStruct is a required argument. exit\n');
    return;
  end
  if(nargin < 2) 
    iter = 500;
  end
  if(nargin < 3) 
    norm = 0;
  end
  if(nargin < 4) 
    bord = 1;
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
  
  % inflate the surface
  inflatedSurf = inflateQ(surfStruct,iter,0.1,norm,bord,0);
  
  % iterate over faces, deciding if the face centroid is closer or
  % farther from the surface centroid
  surfCentroid = mean(surfStruct.vertices);
  faceIsSulcus = zeros(size(surfStruct.faces,1),1);
  for(i=1:size(surfStruct.faces,1))
    faceCentroid = mean(surfStruct.vertices(surfStruct.faces(i,:),:));
    inflFaceCentroid = mean(inflatedSurf.vertices(inflatedSurf.faces(i,:),:));
    if(abs(faceCentroid-surfCentroid) > abs(inflFaceCentroid-surfCentroid))
      faceIsSulcus(i) = 1;
    end
  end
  
  
return;

%************************************************************************%
%%% $Source: /home/cvs/PROJECTS/SurfaceConstructor/tools/matlabSurfTools/findSulcalRegions.m,v $
%%% Local Variables:
%%% mode: Matlab
%%% fill-column: 76
%%% comment-column: 0
%%% End:

