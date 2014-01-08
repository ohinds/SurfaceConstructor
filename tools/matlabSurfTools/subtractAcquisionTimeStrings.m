% subtracts two dicom info acquisition time strings
%

function tim = subtractAcquisionTimeStrings(di1, di2)
  % check for args
  if(nargin < 2)
    fprintf('usage: tim = subtractAcquisionTimeStrings(di1, di2)');
  end
  
  ac1 = di1.AcquisitionTime;
  ac2 = di2.AcquisitionTime;
  
  frac1 = str2num(ac1(find(ac1 == '.'):end));
  if(length(frac1) == 0)
    frac1 = 0;
  end
  
  frac2 = str2num(ac2(find(ac2 == '.'):end));
  if(length(frac2) == 0)
    frac2 = 0;
  end

  % chop
  ac1(find(ac1 == '.'):end) = [];
  ac2(find(ac2 == '.'):end) = [];
  
  % get whole seconds
  s1 = str2num(ac1(end-1:end));
  s2 = str2num(ac2(end-1:end));
  
  % chop
  ac1(end-1:end) = [];
  ac2(end-1:end) = [];
  
  % get minutes 
  m1 = str2num(ac1(end-1:end));
  m2 = str2num(ac2(end-1:end));

  % chop
  ac1(end-1:end) = [];
  ac2(end-1:end) = [];
  
  % get hours
  h1 = str2num(ac1(end-1:end));
  h2 = str2num(ac2(end-1:end));

  % calculate
  
  if(frac2 > frac1)
    frac1 = frac1+1;
    s1 = s1-1;
  end
  
  if(s2 > s1)
    s1 = s1+60;
    m1 = m1-1;
  end
  
  if(m2 > m1)
    m1 = m1+60;
    h1 = h1-1;
  end
  
  if(h1 < h2)
    fprintf('cant subtract these two numbers, the hours of the first number are smaller\n');
    return
  end
  
  tim = [h1-h2, m1-m2, s1-s2 + (frac1-frac2)]; 
  
return

%************************************************************************%
%%% $Source: /home/cvs/PROJECTS/SurfaceConstructor/tools/matlabSurfTools/subtractAcquisionTimeStrings.m,v $
%%% Local Variables:
%%% mode: Matlab
%%% fill-column: 76
%%% comment-column: 0
%%% End:
