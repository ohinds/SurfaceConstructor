% Oliver Hinds <oph@bu.edu>
% 2005-08-31

function f = make_faces_from_edges(e)
  e(:,2) = e(:,2) + max(e(:,1));
  
  f = [];
  curF = 1;
  for(i=1:size(e,1)-1)
    f(curF,1) = e(i,1);
    
    if(e(i,1) == e(i+1,1))
      f(curF,2) = e(i,2);     
    else
      f(curF,2) = e(i+1,1);     
    end
    f(curF,3) = e(i+1,2);     
    
    curF = curF+1;
  end

  f(curF,1) = e(end,1);
  
  if(e(end,1) == e(1,1))
    f(curF,2) = e(end,2);     
  else
    f(curF,2) = e(1,1);     
  end
  f(curF,3) = e(1,2);     
  
  
return