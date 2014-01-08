% write a slice contour label set to a file in the libsr file format
% Oliver Hinds <oph@bu.edu>
% 2006-04-21

function writeSliceContourLabels(filename,labels)
  if(nargin ~= 2)
    fprintf('usage: writeSliceContourLabels(filename,labels)\n');
    return
  end

  % open the file
  fp = fopen(filename,'w');
  if(fp == -1)
    fprintf('couldn''t open file %s for writing\n',filename);
    return;
  end

  for(i=1:length(labels))
    fprintf(fp, '%d\n', labels(i));
  end
  
  fclose(fp);
end

%************************************************************************%
%%% $Source: /home/cvs/PROJECTS/SurfaceConstructor/tools/matlabSurfTools/writeSliceContourLabels.m,v $
%%% Local Variables:
%%% mode: Matlab
%%% fill-column: 76
%%% comment-column: 0
%%% End:
