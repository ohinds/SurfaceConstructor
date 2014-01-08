% deleteFace deletes faces from a patch using the middle mouse button
% and the 'x' key
%
% DELETEDFACES = DELETEFACE([FIGHANDLE])
%
% Hit RETURN in the figure window to quit deleting. 
%
% Uses the figure handle given by gcf, if figHandle is not given. 
%
% NOTE: we use the middle mouse button and RETURN instead of the
% first mouse button so that we can leave the first mouse button
% for use by the camera tools.
%
% Oliver Hinds <oph@bu.edu>
% 2004-11-04
% taken from mukund's pickFace.m

function varargout = deleteFace(varargin)

fprintf('deleteFace(): Hit the middle mouse button to pick a face\n');
fprintf('            Hit "x" to delete the last picked face.\n');
fprintf('            Hit RETURN to quit deleting.\n\n');

% save current state of "more"
MORE_STATE = get(0, 'More');
more off
deletedFaces = [];
pickedFaces = [];

if(nargin >= 1)
  surfStruct = varargin{1};
else
  fprintf('usage: [surfStruct, deletedFaces] = deleteFace(surfStruct, [fig])\n');
  return;
end

if nargin >= 2
  fig = varargin{2};
else
  fig = gcf; 
end

figure(fig);
ah = gca;

set(ah, 'Projection', 'orthographic');

phCands = get(ah,'Children');
ph = [];
for i = 1:length(phCands)
  childType = get(phCands(i),'Type');
  if strncmpi(childType,'patch',5)
    ph = phCands(i);
  end
end
if isempty(ph)
  fprintf('Current figure does not have a "patch" object\n');
  error('Exiting ...');
end

% Start Loop; end with hitting return
plh = [];
button = 0;
triCoords = [];
char = 0;
STOP = 0;
while ~STOP
  % Use no-side effect WAITFORBUTTONPRESS
  waserr = 0;
  try
    keydown = wfbp;
  catch
    waserr = 1;
  end
  if(waserr == 1)
    if(ishandle(fig))
      error('Interrupted');
    else
      error('Interrupted by figure deletion');
    end
  end
  
  ptr_fig = get(0,'CurrentFigure');
  if(ptr_fig == fig)
    if keydown
      %%% if a key was pressed, check for the return key
      char = get(fig, 'CurrentCharacter');
      if (char == 13) | (char == 113) %%% Return key or 'q'
	STOP = 1; %%% Game over
	button = 0;
      elseif(char == 88 | char == 120) %%% 'x' or 'X'
	if(~isempty(pickedFace))
	  surfStruct = deleteFaceNumber(surfStruct,pickedFace);
	  deletedFaces(end+1) = pickedFace(end);
	  pickedFace(end) = [];
	  
	  % delete the old patch and create a new one
	  if ~isempty(get(gcf, 'Children'))
	    cp = get(gca,'CameraPosition');
	    ct = get(gca,'CameraTarget');
	    cu = get(gca,'CameraUpVector');
	    cv = get(gca,'CameraViewAngle');
	    cla  
	    set(gca,'CameraPosition',cp);
	    set(gca,'CameraTarget',ct);
	    set(gca,'CameraUpVector',cu);
	    set(gca,'CameraViewAngle',cv);
	  end
	  %delete(ph);
	  ph = patch('vertices', surfStruct.vertices, ...
		     'faces', surfStruct.faces, ...
		     'facecolor', [0.5 0.5 0.5], ...
		     'edgecolor', 'none');
	  axis off;
	  axis vis3d;
	  axis equal;
	  lighting gouraud
	  material dull
	  camlight;
	  toggleEdges;

	  fprintf(['Most recent face deleted: ' ...
		   num2str(deletedFaces(end)) '\n']);
	  
	end
	button = 0;
      end
    else
      %%% a button was pressed
      button = get(fig, 'SelectionType');
      if strcmp(button,'open')
	button = b(length(b));
      elseif strcmp(button,'normal')
	button = 1;
      elseif strcmp(button,'extend')
	button = 2;
      elseif strcmp(button,'alt')
	button = 3;
      else
	error('Invalid mouse selection.')
      end
    end
    
    if button == 2
      
      %%% "Meat" of this program %%%
      [ptCoords, vertCoords, pickedVertex, triCoords, pickedFace]...
	  = select3d(ph);
      if ~isempty(pickedFace)
	pickedFaces = [pickedFaces; pickedFace];
	fprintf(['Most recent face picked: ' ...
		 num2str(pickedFace) '\n']);

	% Display the selected vertices
	plhtmp = patch('Vertices', triCoords', ...
		       'Faces', [1 2 3], ...
		       'FaceColor', 'k'); 
	plh = [plh; plhtmp];
	
      end
    
    end   

  end
  
end

if nargout >= 1
  varargout{1} = surfStruct;
end

if nargout >= 2
  varargout{2} = deletedFaces;
end


set(0, 'More', MORE_STATE);

%% delete a face from a patch
function ph = deleteFaceNumber(ph,faceNum)
  % get the vertices in the face
  verts = ph.faces(faceNum,:);

  % remove the face from the list
  ph.faces(faceNum,:) = [];
  
  % remove any vertices that aren't in any more faces
  verts = fliplr(sort(verts));
  for(i=1:length(verts))
    if(length(find(ph.faces == verts(i))) == 0) % no more references to vertex
      ph.vertices(verts(i),:) = [];
      
      % decreement all vertex reference indices greater than deleted
      greaterIndices = find(ph.faces > verts(i));
      ph.faces(greaterIndices) = ph.faces(greaterIndices) - 1;
    end
  end
  
  % re preprocess
  %ph = preprocessQ(ph);
return;

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function key = wfbp
%WFBP   Replacement for WAITFORBUTTONPRESS that has no side effects.

fig = gcf;
current_char = [];

% Now wait for that buttonpress, and check for error conditions
waserr = 0;
try
  % Disabling ^C for edit menu so the only ^C is for interrupting
  % the function.
  h=findall(fig,'type','uimenu','accel','C');   
  set(h,'accel','');                            
  keydown = waitforbuttonpress;
  current_char = double(get(fig,'CurrentCharacter')); 
  % Capturing the character.
  if~isempty(current_char) & (keydown == 1)           
    % If the character was generated by the current keypress AND is
    % ^C, set 'waserr'to 1 so that it errors out.
    if(current_char == 3)                       
      waserr = 1;                             
    end
  end
  
  set(h,'accel','C');                                 
  % Set back the accelerator for edit menu.
catch
  waserr = 1;
end
drawnow;
if(waserr == 1)
  set(h,'accel','C');
  % Set back the accelerator if it errored out.
  error('Interrupted');
end

if nargout>0, key = keydown; end
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%************************************************************************%
%%% $Source: /home/cvs/PROJECTS/SurfaceConstructor/tools/matlabSurfTools/deleteFace.m,v $
%%% Local Variables:
%%% mode: Matlab
%%% fill-column: 76
%%% comment-column: 0
%%% End:

