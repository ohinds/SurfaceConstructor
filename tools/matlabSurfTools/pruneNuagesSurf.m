% pruneNuagesSurf attempts to delete faces of a surface that have
% been introduced to accomodate the contraints imposed by the
% nuages reconstruction software
%
% SURFSTRUCT = PRUNENUAGESSURF(SURFSTRUCT[, TOOLONG, BOUNDS, SLICES])
%   SURFSTRUCT is a surface structure with M vertices and N faces
%   containing at least the members: vertices, a 3xM list of vertex
%   coordinates, and faces, a 3xN list of face vertex indices
%   TOOLONG is the maximum edge length
%   BOUNDS is a vector representing the corners of a rectangle outside of
%   which to delete vertices and faces. in the format 
%     [xmin, xmax, ymin, ymax, zmin, zmax]
%   SLICES  is the cell array containing the original nuages slices
%   as read in by the function READNUAGESSLICES. This argument is
%   required if added vertices are to be pruned
%
% Oliver Hinds <oph@bu.edu>
% 2004-11-04

function surfStruct = pruneNuagesSurf(surfStruct, tooLong, bounds, slices)

  TOL = 0.02;    % floating point comparison (with nuages additions)
  TOOLONG = 150; % how long is too long?
  WINBOUNDS = [-512, 512, -512, 512, -512, 512];
  
  % check param numbers
  if(nargin > 1)
    TOOLONG = tooLong;
  end
  if(nargin > 2)
    WINBOUNDS = bounds;
  end

  % which actions to perform
  pruneOutsideWindow = 1;
  pruneAddedVertices = 0;
  pruneClosingEdges = 0;
  prunePlaneEdges = 0;
  pruneLongEdges = 1;
  
  %% delete all vertices and their edges outside of a box
  if(pruneOutsideWindow)
    % iterate over vertices, delete those outside the window
    vertIndices = [];
    for(i=1:length(surfStruct.vertices))
      vert = surfStruct.vertices(i,:);
      
      % test for outside window
      if(vert(1) < WINBOUNDS(1) | vert(1) > WINBOUNDS(2) ...
	 | vert(2) < WINBOUNDS(3) | vert(2) > WINBOUNDS(4) ...
	 | vert(3) < WINBOUNDS(5) | vert(3) > WINBOUNDS(6))
	% find the faces that vertex is in and delete them
	faces = find(surfStruct.faces(:,1) == i ...
		     | surfStruct.faces(:,2) == i ...
		     | surfStruct.faces(:,3) == i);

	% print a feel-good message
	for(j=1:length(faces))
	  fprintf('delete: face %d has vertex=%d out of bounding box\n',faces(j),i);
	end
	
	surfStruct.faces(faces,:) = [];

	vertIndices(end+1) = i;	
      end
    end

    % remove the vertices
    vertIndices = fliplr(sort(vertIndices));
    for(i=1:length(vertIndices))
      if(length(find(surfStruct.faces == vertIndices(i))) ~= 0) % more references to vertex
	fprintf(['error: thought i removed all references to this' ...
		 ' vertex %d!\n'], vertIndices(i));
	return;
      end

      surfStruct.vertices(vertIndices(i),:) = [];

      % decreement all vertex reference indices greater than deleted
      greaterIndices = find(surfStruct.faces > vertIndices(i));
      surfStruct.faces(greaterIndices) = surfStruct.faces(greaterIndices) - 1;
    end
  end
  
  %% delete the vertices not in the original data and remove the faces
  
  if(pruneAddedVertices)
    % find the vertex index of each vertex in the original slices
    vertCoords = [];
    for(i=1:length(slices))
      curSlice = slices{i};
      for(j=1:length(curSlice))
	vertCoords(end+1:end+size(curSlice{j},1),:) = curSlice{j};
      end
    end

    % delete all the faces including these vertices, and then the
    % vertices themselves
    vertIndices = [];
    for(i=1:length(surfStruct.vertices))
      vert = surfStruct.vertices(i,:);
      
      % search for the vertices in the slices
      if(isempty(find(...
	  vert(1)+TOL > vertCoords(:,1) & ...
	  vert(1)-TOL < vertCoords(:,1) & ...
	  vert(2)+TOL > vertCoords(:,2) & ...
	  vert(2)-TOL < vertCoords(:,2) & ...
	  vert(3)+TOL > vertCoords(:,3) & ...
	  vert(3)-TOL < vertCoords(:,3))))

	% find the faces that vertex is in and delete them
	faces = find(surfStruct.faces(:,1) == i ...
		     | surfStruct.faces(:,2) == i ...
		     | surfStruct.faces(:,3) == i);

	% print a feel-good message
	for(j=1:length(faces))
	  fprintf('delete: face %d has an added vertex=%d\n',faces(j),i);
	end
	
	surfStruct.faces(faces,:) = [];

	vertIndices(end+1) = i;
      end
    end

    % remove the vertices
    vertIndices = fliplr(sort(vertIndices));
    for(i=1:length(vertIndices))
      if(length(find(surfStruct.faces == vertIndices(i))) ~= 0) % more references to vertex
	fprintf(['error: thought i removed all references to this' ...
		 ' vertex %d!\n'], vertIndices(i));
	return;
      end

      surfStruct.vertices(vertIndices(i),:) = [];

      % decreement all vertex reference indices greater than deleted
      greaterIndices = find(surfStruct.faces > vertIndices(i));
      surfStruct.faces(greaterIndices) = surfStruct.faces(greaterIndices) - 1;
    end
  end

  
  %% now prune edges connecting the end points of one contour
  if(pruneClosingEdges)
    % find the vertex index of each vertex in the original slices
    vertCoords = [];
    for(i=1:length(slices))
      curSlice = slices{i};
      for(j=1:length(curSlice))
	vertCoords(end+1:end+2,:) = curSlice{j}([1 end],:);
      end
    end

    % delete all the faces including more than one of these vertices
    vertIndices = [];
    for(i=1:length(surfStruct.vertices))
      vert = surfStruct.vertices(i,:);
      
      % search for the vertices in the slices
      if(~isempty(find(...
	  vert(1)+TOL > vertCoords(:,1) & ...
	  vert(1)-TOL < vertCoords(:,1) & ...
	  vert(2)+TOL > vertCoords(:,2) & ...
	  vert(2)-TOL < vertCoords(:,2) & ...
	  vert(3)+TOL > vertCoords(:,3) & ...
	  vert(3)-TOL < vertCoords(:,3))))

	vertIndices(end+1) = i;
      end
    end

    % iterate over faces, deleting appropriate ones
    for(i=length(surfStruct.faces):-1:1)
      face = surfStruct.faces(i,:);
      if(sum(face(1) == vertIndices) + sum(face(2) == vertIndices) + ...
	 sum(face(3) == vertIndices) > 1)
	surfStruct.faces(i,:) = [];
      end
    end
  end

  %% now prune faces with all edges in same slice
  if(prunePlaneEdges)
    % iterate over faces, deleting ones in the same slice or with long edges
    for(i=length(surfStruct.faces):-1:1)
      face = surfStruct.faces(i,:);
      
      sl = surfStruct.vertices(face,3);
      if(sl(1) + TOL > sl(2) & sl(1) - TOL < sl(2) & ...
	 sl(1) + TOL > sl(3) & sl(1) - TOL < sl(3))
	fprintf('delete: face %d has all edges in slice plane, z = %f\n',i,sl(1));
	surfStruct.faces(i,:) = [];
      end
    end
  end

  %% now prune long edges
  if(pruneLongEdges)
    % iterate over faces, deleting ones in the same slice or with long edges
    for(i=length(surfStruct.faces):-1:1)
      face = surfStruct.faces(i,:);
      
      d = [distance(surfStruct.vertices(face(1),:), ...
		    surfStruct.vertices(face(2),:)) ...
	   distance(surfStruct.vertices(face(2),:), ...
		    surfStruct.vertices(face(3),:))  ...
	   distance(surfStruct.vertices(face(1),:), ...
		    surfStruct.vertices(face(3),:)) ];
	
      if(max(d) > TOOLONG)
	fprintf('delete: face %d has an edge thats too long = %f\n',i,max(d));
	surfStruct.faces(i,:) = [];
      end
    end
  end
  
return

function d = distance(p1,p2)
  d = sqrt(sum((p1-p2).^2));
return

%************************************************************************%
%%% $Source: /home/cvs/PROJECTS/SurfaceConstructor/tools/matlabSurfTools/pruneNuagesSurf.m,v $
%%% Local Variables:
%%% mode: Matlab
%%% fill-column: 76
%%% comment-column: 0
%%% End:
