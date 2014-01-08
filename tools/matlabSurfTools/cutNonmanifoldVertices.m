function s = cutNonmanifoldVertices(s)
  s = preprocessQ(s);
  
  for(i=1:s.V)
    if(s.vertexManifoldness(i) ~= 0)
      s = cutVertex(s,i);
      s = preprocessQ(s);
      if(strcmp(s.vertexManifoldness,'N/A'))
	break;
      end
    end
  end
  
return

function s = cutVertex(s,v)
  [NTaV, maxNTaV, Index, LTaV, v1TaV, v2TaV] = ...
      trianglesAtVertex(s.vertices', s.faces'-1);  
  s.LTaV = LTaV;
  s.v1TaV = v1TaV;
  s.v2TaV = v2TaV;
  s = buildNTaE(s);
  
  % build star
  vstar = buildStar(s,v);

  % decompose star into n connected components
  compList = {};
  curComp = 0;
  while(~isempty(vstar.edges))
    curEnds = vstar.edges(1,:);
    curFace = vstar.faceRef(1);
    vstar.edges(1,:) = [];
    vstar.faceRef(1) = [];
    curComp = curComp+1;
    compList{curComp}(1) = curFace;
    while(~isempty(intersect(vstar.edges(:),curEnds)))
      for(i=1:2)
	[r,c] = find(vstar.edges == curEnds(i),1);
	if(~isempty(r))
	  if(c == 1)
	    c2 = 2;
	  else
	    c2 = 1;
	  end
	  
	  % make sure the connecting edge is a manifold edge
	  if(s.NTaE(s.Index2(v)+find(s.Neighbors(s.Index2(v):s.Index2(v)+s.NNoV(v)-1) == curEnds(i))-1) < 3)
	    curEnds(i) = vstar.edges(r,c2);
	    compList{curComp}(end+1) = vstar.faceRef(r);
	    vstar.edges(r,:) = [];
	    vstar.faceRef(r) = [];	  
	  else
	    curEnds(i) = -1;
	  end
	end
      end
    end
  end
  
  % duplicate vertex n-1 times
  n = length(compList);
  for(i=2:n)
    vi = size(s.vertices,1)+1;
    
    % duplicate the vertex and adjust the face references
    s.vertices(vi,:) = s.vertices(v,:);
  
    for(f=1:length(compList{i}))
      s.faces(compList{i}(f),find(s.faces(compList{i}(f),:) == v)) = vi;
    end
  end
return

function star = buildStar(s,v)
  star.vertices = s.Neighbors(s.Index2(v):s.Index2(v)+s.NNoV(v)-1);
  star.edges = [];
  star.faceRef = [];
  
  % loop over neighbors, adding edges
  for(i=0:s.NNoV(v)-1)
    n = s.Neighbors(s.Index2(v)+i);
    % search the neighbors of this neighbor for neighbors of this vertex 
    for(j=0:s.NNoV(n)-1)
      nn = s.Neighbors(s.Index2(n)+j);
      if(nn == v)
	continue;
      end
      
      if(~isempty(find(s.Neighbors(s.Index2(v):s.Index2(v)+s.NNoV(v)-1) == nn))) 
	% check that the vertices are connected by a minifold edge
%	if(s.vertexManifoldness(n) == 1 & s.vertexManifoldness(nn) == 1)
%	  continue;
%	end
	
	% add the edge if the edge doesnt exist in edges yet
	if(isempty(strmatch([n nn],star.edges,'exact')) ...
	   & isempty(strmatch([nn n],star.edges,'exact')))
	  
	  nind = intersect(find(s.v1TaV(s.Index(v):s.Index(v)+s.NTaV(v)-1) == n),find(s.v2TaV(s.Index(v):s.Index(v)+s.NTaV(v)-1) == nn));
	  if(~isempty(nind))
	    star.faceRef(end+1) = s.LTaV(s.Index(v)+nind(1)-1);
	    star.edges(end+1,:) = [n nn];
	  else
	    nind = intersect(find(s.v2TaV(s.Index(v):s.Index(v)+s.NTaV(v)-1) == n),find(s.v1TaV(s.Index(v):s.Index(v)+s.NTaV(v)-1) == nn));
	    if(~isempty(nind))
	      star.faceRef(end+1) = s.LTaV(s.Index(v)+nind(1)-1);
	      star.edges(end+1,:) = [n nn];
	    end
	  end
	end
      end
    end
  end
return

function s = buildNTaE(s)
  s.NTaE = zeros(size(s.Neighbors));
  p = combnk([1 2 3],2);
  for(f=1:s.F)
    for(i=1:size(p,1))
      v1 = s.faces(f,p(i,1));
      v2 = s.faces(f,p(i,2));
      s.NTaE(s.Index2(v1) + find(s.Neighbors(s.Index2(v1):s.Index2(v1)+s.NNoV(v1)-1) == v2)-1) = s.NTaE(s.Index2(v1) + find(s.Neighbors(s.Index2(v1):s.Index2(v1)+s.NNoV(v1)-1) == v2)-1) + 1;

      s.NTaE(s.Index2(v2) + find(s.Neighbors(s.Index2(v2):s.Index2(v2)+s.NNoV(v2)-1) == v1)-1) = s.NTaE(s.Index2(v2) + find(s.Neighbors(s.Index2(v2):s.Index2(v2)+s.NNoV(v2)-1) == v1)-1) + 1;
    end
  end
return