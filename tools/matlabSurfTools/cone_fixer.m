% fixes non mainfoldness caused by a cone error
% Oliver Hinds <oph@bu.edu>
% 2006-08-21

function s = cone_fixer(s)
  s = preprocessQ(s);
  
  for(i=1:s.V)
    if(s.vertexManifoldness(i) == 2)
      s = fix_cone(s,i);
    end
  end
  
  s = preprocessQ(s);
  
return

function s = fix_cone(s,v)
  n = 0;
  done = 0;
  count = 0;
  while(~done)
    neighList = [];
    triList = [];

    t1 = s.LT1aNE(s.Index2(v)+n);
    neighList(end+1) = s.Neighbors(s.Index2(v)+n);
    triList(end+1) = s.LT2aNE(s.Index2(v)+n);      
    while(t1 ~= s.LT2aNE(s.Index2(v)+n))
      n = n+1;
      neighList(end+1) = s.Neighbors(s.Index2(v)+n);
      triList(end+1) = s.LT2aNE(s.Index2(v)+n);      
    end

    vi = v;
    if(count > 0)
      vi = size(s.vertices,1)+1;
      
      % duplicate the vertex and adjust the face references
      s.vertices(vi,:) = s.vertices(v,:);
      
      for(f=1:length(triList))
	s.faces(triList(f),find(s.faces(triList(f),:) == v)) = vi;
      end
    end
    
    % move the vertex epsilon toward these neighbors
    m = mean(s.vertices(neighList,:));
    s.vertices(vi,:) = s.vertices(vi,:) + 0.1*(m-s.vertices(vi,:));
    
    n = n+1;
    if(n == s.NNoV(v))
      done = 1;
    end
      
    count = count+1;    
  end
  
return

%************************************************************************%
%%% $Source: /home/cvs/PROJECTS/SurfaceConstructor/tools/matlabSurfTools/cone_fixer.m,v $
%%% Local Variables:
%%% mode: Matlab
%%% fill-column: 76
%%% comment-column: 0
%%% End:
