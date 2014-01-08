% lab = readsurflabels(surffname);
% Oliver Hinds <ohinds@mit.edu> 2007-07-21
function lab = readsurflabels(filestring);

  % read the label file, if there is one.
  [fptr msg] = fopen([filestring '.lab']);

  if(fptr == -1)
    newfilestring = filestring(1:end-4);
    [fptr msg] = fopen([newfilestring '.lab']);
  end

  if(fptr == -1)
    [fptr msg] = fopen(filestring);
  end

  if(fptr == -1)
    return;
  end

  lab = [];
  CONT = 1;
  while CONT
    str = fscanf(fptr, '%s', 1);
    if isempty(str) | feof(fptr)
      CONT = 0;
      continue;
    end
    
    if(str(1) == '#')
      fgetl(fptr);
    else
      lab(end+1) = str2num(str);
    end
  end

  lab = lab';

  fclose(fptr);
  
return