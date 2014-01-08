% addFace adds faces to a patch using the middle mouse button to select
% three vertices, then the 'a' key to add
%
% SURFSTRUCT = ADDFACE([FIGHANDLE])
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

function varargout = addFace(varargin)

VERBOSE = 1;
maxPix = Inf;

fprintf('addFace(): Hit the middle mouse button to pick three vertices\n');
fprintf('           Hit "a" to add a face with the last 3 picked vertices.\n');
fprintf('           Hit RETURN to quit adding.\n\n');

% save current state of "more"
MORE_STATE = get(0, 'More');
more off
pickedVertices = [];

if(nargin >= 1)
  surfStruct = varargin{1};
else
  fprintf('usage: [surfStruct] = addFace(surfStruct, [fig])\n');
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
      elseif(char == 65 | char == 97) %%% 'a' or 'A'
	if(size(pickedVertices,1) >= 3)
	  surfStruct.faces(end+1,:) = pickedVertices(end-2:end,:);
	  pickedVertices(end-2:end,:) = [];

	  phCands = get(gca,'Children');
	  h = [];
	  for i = 1:length(phCands)
	    childType = get(phCands(i),'Type');
	    if strncmpi(childType,'patch',5)
	      h = phCands(i);
	    end
	  end

	  % delete the old patch and create a new one
	  if ~isempty(get(gcf, 'Children'))
	    cp = get(gca,'CameraPosition');
	    ct = get(gca,'CameraTarget');
	    cu = get(gca,'CameraUpVector');
	    cv = get(gca,'CameraViewAngle');
	    m = get(h,'marker');
	    ms = get(h,'markersize');
	    ec = get(h,'edgecolor');
	    cla
	    set(gca,'CameraPosition',cp);
	    set(gca,'CameraTarget',ct);
	    set(gca,'CameraUpVector',cu);
	    set(gca,'CameraViewAngle',cv);
	  end
	  %delete(ph);
	  ph = patch('vertices', surfStruct.vertices, ...
		     'faces', surfStruct.faces, ...
		     'marker', m, ...
		     'markersize', ms, ...
		     'edgecolor', ec, ...
		     'facecolor', [0.5 0.5 0.5]);
	  axis off;
	  axis vis3d;
	  axis equal;
	  lighting gouraud
	  material dull
	  camlight;

	  fprintf(['Most recent face added: [' ...
		   num2str(surfStruct.faces(end,1)) ','...
		   num2str(surfStruct.faces(end,2)) ','...
		   num2str(surfStruct.faces(end,3)) ']\n']);

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
      if ~isempty(pickedVertex)
	pickedVertices = [pickedVertices; pickedVertex];
	if VERBOSE
	  fprintf(['Most recent vertex picked: ' ...
		   num2str(pickedVertex) '\n']);
	end
	% Display the selected vertices
	plhtmp = line(vertCoords(1), ...
		      vertCoords(2), ...
		      vertCoords(3));
	set(plhtmp, 'Marker', '.', ...
		    'MarkerSize', 6, ...
		    'Color', 'k');

	plh = [plh; plhtmp];

	if length(pickedVertices) == maxPix
	  STOP = 1;
	end

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
%%% $Source: /home/cvs/PROJECTS/SurfaceConstructor/tools/matlabSurfTools/addFace.m,v $
%%% Local Variables:
%%% mode: Matlab
%%% fill-column: 76
%%% comment-column: 0
%%% End:

