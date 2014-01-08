% multDSFileCoordsBy multiplies
% Oliver Hinds <oph@bu.edu>
% 2005-07-06

function transformDSFileCoordsBy(dsinfilename, A, ...
				 dsoutfilename, verbose)
  if(nargin < 3)
    fprintf('usage: transformDSFileCoordsBy(dsinfile,A,dsoutfile,verbose)\n');
    return;
  end

  if(nargin < 4)
    verbose = 0;
  end

  % open the files
  fi = fopen(dsinfilename,'r');
  fo = fopen(dsoutfilename,'w');

  if(fi == -1 | fo == -1)
    fprintf('couldnt open one or more files, exit\n');
    return;
  end

  format long;

  lines = breakLines(fscanf(fi,'%c'));

  % iterate over each line of the file, looking for coords that
  % need multiplying
  for(lin=1:length(lines))
    oldlin = lines{lin};
    newlin = [];

    % check for rotation
    ind = strfind(oldlin,'rotation: (');
    if(length(ind) > 0)
      a = sscanf(oldlin,'rotation: (%f,%f) %f');
      b = vertcat(a,[0 1]');
      b = A*b;
      a(1:2) = b(1:2);
      newlin = sprintf('rotation: (%f,%f) %f',a);
    end

    % check for translation
    ind = strfind(oldlin,'translation: (');
    if(length(ind) > 0)
      a = sscanf(oldlin,'translation: (%f,%f)');
      b = vertcat(a,[0 1]');
      b = A*b;
      a(1:2) = b(1:2);
      newlin = sprintf('translation: (%f,%f)',a);
    end

    % check for tack
    ind = strfind(oldlin,'tack: (');
    if(length(ind) > 0)
      a = sscanf(oldlin,'tack: (%f,%f)');
      b = vertcat(a,[0 1]');
      b = A*b;
      a(1:2) = b(1:2);
      newlin = sprintf('tack: (%f,%f)',a);
    end

    % check for none
    if(length(newlin) == 0)
      newlin = oldlin;
    end

    if(verbose)
      fprintf('< %s\n',oldlin);
      fprintf('> %s\n',newlin);
    end

    % print it to the new file
    if(verbose)
      fprintf('<%s\n',oldlin);
      fprintf('>%s\n',newlin);
    end
    
    fprintf(fo,'%s\n',newlin);
  end

  fclose(fi);
  fclose(fo);

return

function lines = breakLines(s)
  lines = {};
  curLine = 1;
  ind = 1;

  while(ind < length(s))
    c = char(s(ind));
    ind = ind+1;
    lin = [];

    while(ind <= length(s) & c ~= 10)
      lin(end+1) = c;

      c = char(s(ind));
      ind = ind+1;
    end

    lines{curLine} = char(lin);
    curLine = curLine+1;
  end
return