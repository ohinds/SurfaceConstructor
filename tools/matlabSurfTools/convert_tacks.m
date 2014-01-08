% THIS IS OCTAVE, NOT MATLAB!!!!!!
% Oliver Hinds <oph@bu.edu>
% 2005-03-22

function convert_tacks(infilename,multiplier,outfilename)
  
  ifp = fopen(infilename,'r');
  ofp = fopen(outfilename,'w');
  
  if(~ifp)
    printf('error: couldnt open file %s for reading',infilename);
    return
  end
  
  [c, count] = fscanf(ifp,'%c','C');

  while(count > 0)
    fprintf(ofp,'%s',c);
    
    if(c == '(')
      x = fscanf(ifp,'%f','C');
      fscanf(ifp,'%c','C');
      y = fscanf(ifp,'%f','C');
      fprintf(ofp,'%0.30f,%0.30f',x*multiplier,y*multiplier);
    end
    
    [c, count] = fscanf(ifp,'%c',1);
  end

  fclose(ifp);
  fclose(ofp);
  
endfunction
