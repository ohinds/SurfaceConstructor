function s3 = deleteUnequalVertices(s1,s2,s3)
  done = 0;
  while(~done)
    for(v=1:min([size(s1.vertices,1) size(s2.vertices,1)]))
      if(~verticesEqual(s1.vertices(v,:), s2.vertices(v,:)))
	fprintf('removing %d\n',v);
	s1.vertices(v,:) = [];
	s3.vertices(v,:) = [];
%	inds = find(s3.faces > v);
%	s1.faces(inds) = s1.faces(inds)-1;
%	s3.faces(inds) = s3.faces(inds)-1;
%	s1 = preprocessQ(s1);
%	if(~strcmp(s1.vertexManifoldness,'N/A'))
%	  return
%	end
	break;
      end
    end
    if(v == min([size(s1.vertices,1) size(s2.vertices,1)]))
      done = 1;
    end
  end

  s3.faces = s2.faces;
return

function b = verticesEqual(v1,v2)
  b = sum(abs(v1-v2)) < 0.000000001;
return