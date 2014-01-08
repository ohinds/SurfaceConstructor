% fixes non mainfoldness caused by a two-hole error
% Oliver Hinds <oph@bu.edu>
% 2006-08-21

function s = two_hole_fixer(s)
  s = preprocessQ(s);
  
  for(i=1:s.V)
    if(s.vertexManifoldness(i) == 3)
      s = fix_two_hole(s,i);
    end
  end
  
  s = preprocessQ(s);
  
return

function s = fix_two_hole(s,v)
  n = find(s.LT1aNE(s.Index2(v):s.Index2(v)+s.NNoV(v)-1)==0,1)-1;
  orign = n;
  
  done = 0;
  count = 0;
  while(~done)
    neighList = s.Neighbors(s.Index2(v)+n);
    triList = [];
    
    n = n+1;
    if(n == s.NNoV(v))
      n = 0;
    end
    if(n == orign)
      done = 1;
    end
    
    while(s.LT1aNE(s.Index2(v)+n) ~= 0)
      neighList(end+1) = s.Neighbors(s.Index2(v)+n);
      triList(end+1) = s.LT1aNE(s.Index2(v)+n);
      
      n = n+1;
      if(n == s.NNoV(v))
	n = 0;
      end
      if(n == orign)
	done = 1;
      end
   
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
    
    count = count+1;    
  end
  
return

%************************************************************************%
%%% $Source: /home/cvs/PROJECTS/SurfaceConstructor/tools/matlabSurfTools/two_hole_fixer.m,v $
%%% Local Variables:
%%% mode: Matlab
%%% fill-column: 76
%%% comment-column: 0
%%% End:
