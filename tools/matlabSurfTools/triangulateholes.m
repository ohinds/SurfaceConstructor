% triangulateholes fills in the holes in a surface by finding the
% boundaries, flattening, and then triangulating 
% Oliver Hinds <oph@bu.edu> 2005-05-20

function surfStructNoHoles = triangulateholes(surfStruct,display,closed)
  % validate input
  if(nargin < 1)
    fprintf('error: surfStruct is a required argument\n');
  end
  if(nargin < 2)
    display = 0;
  end
  if(nargin < 3)
    closed = 0;
  end
  
  surfStructNoHoles = surfStruct;
  
  % find the boundaries
  if(closed)
    surfStructNoHoles.PREPROCESSED = 'False';
    f = surfStructNoHoles.faces(end,:);
    surfStructNoHoles.faces(end,:) = [];
    i = 0;
    while(~strcmp(surfStructNoHoles.PREPROCESSED,'True'))
      surfStructNoHoles.faces(end+1,:) = f;
      f = surfStructNoHoles.faces(end-i,:);
      surfStructNoHoles.faces(end,:) = [];
      vert = f(3);
      surfStructNoHoles = preprocessQ(surfStructNoHoles);
      i = i+1;
    end
  else
    vert = [];
  end
  [bn, numboundaries, bn_cell] = boundaryVertices(surfStructNoHoles);
  
  if(closed) 
    for(i=1:numboundaries)
      if(~isempty(find(bn_cell{i} == f(3))))
	maxInd = i;
	break;
      end
    end
  else 
    maxVal = 0;
    maxInd = 1;
    for(ccc=1:numboundaries)
      if(length(bn_cell{ccc}) > maxVal)
	maxInd = ccc;
	maxVal = length(bn_cell{ccc});
      end
    end
  end
  
  % flatten the surface
  if(closed) 
    bfsurf = BARYflattenOPH(surfStructNoHoles,vert);
  else
    bfsurf = BARYflatten(surfStructNoHoles);
  end
  
  % triangulate each hole 
  triflags = 'Q';
  for(i=1:numboundaries)
    if(i == maxInd)
      continue;
    end
    
    if(display)
      showVertex(surfStructNoHoles,bn_cell{i});
      pause;
    end
  
    % build the input struct
    in.pl = bfsurf.vertices(bn_cell{i},1:2)';
    
    % run it
    out = triangle(in,triflags);
    
    % append the output faces to the surfStruct
    fc = bn_cell{i}(out.tl');
    
    % transpose if wrong
    if(size(fc) == [3 1])
      fc = fc';
    end
    
    surfStructNoHoles.faces(end+1:end+size(fc,1),:) = fc;

    surfStructNoHoles = preprocessQ(surfStructNoHoles);
  
    if(~strcmp(surfStructNoHoles.PREPROCESSED,'True'))    
      surfStructNoHoles.faces(end-size(fc):end,:) = [];
      surfStructNoHoles = preprocessQ(surfStructNoHoles);
    end
  end
  
  if(closed)
    surfStructNoHoles.faces(end+1,:) = f;
  end
  
  surfStructNoHoles = preprocessQ(surfStructNoHoles);
  
return

