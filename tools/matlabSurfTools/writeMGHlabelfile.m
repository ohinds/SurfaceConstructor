function writeMGHlabelfile(filestring, surf, surflabels)
% write MGH per-vertex labels (e.g. .w files)
%
% writeMGHsurfLabels(filestring, surf, surflabels)
%

L = size(surflabels, 1);

% Open  ascii
fid = fopen(filestring,'w') ;

% test for successful open
if ( fid == -1 ),
  error('%s: error opening file [%s]', mfilename, filestring);
end;

% write header
fprintf(fid,['#!ascii label ' filestring ', from subject \n']);
fprintf(fid,'%d\n',length(find(surflabels ~= -1)));

% write body
for i = 1:L  
  if(surflabels(i) > -1)
    fprintf(fid,'%d %f %f %f %d\n',i-1,surf.vertices(i,:),surflabels(i));
  end
end

% Close file
fclose(fid);

