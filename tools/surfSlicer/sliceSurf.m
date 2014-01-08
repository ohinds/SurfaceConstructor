% sliceSurf.m generates slices of a surface
% Oliver Hinds <oph@bu.edu>
% 2004-07-28

function sliceSurf(surfName,dim,numSlices)
  
  % validate input
  if(nargin < 1)
    fprintf('usage: sliceSurf(surfName[,dimension])\nexit');
  elseif(nargin < 2)
    dim = 1;
  elseif(nargin < 3)
    numSlices = 100;
  end
  
  % load the surface
  eval(sprintf('loadStruct = load(''%s'');',surfName));
  eval(['surfStruct = loadStruct.' surfName ';']);
  
  % preprocess the surface and determine the distance between slices
  preprocessQ(surfStruct);
  dist = (max(surfStruct.vertices(:,dim)) - min(surfStruct.vertices(:,dim)))...
	 / numSlices;
  
  % get the slices
  slices = surfSlicer(surfStruct,dist,dim);

  % save the slices in a few formats
  eval(['save ' surfName '_slices.mat  slices']);
  writeNuagesSlices(slices,[surfName '_slices_nuages.txt'], dim);
  
return