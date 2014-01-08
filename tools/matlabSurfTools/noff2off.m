% noff2off converts an off file with normals to one without normals
% based on readoff.m by mukund
%
% NOFF2OFF(NOFF_FILE, OFF_FILE)
%   NOFF_FILE is the filename of an off file with normals (an
%   'noff' file). the surface is read from this file
%   OFF_FILE is the filename of an off file without normals (an
%   'off' file). the surface is written to this file
%
% NOTE: comments are not yet supported in the surface files. the
% reader will fail if there are comments in the file
%
% Oliver Hinds <oph@bu.edu>
% 2004-11-04

function noff2off(noff_file, off_file)
% NOFF2OFF(NOFF_FILE, OFF_FILE)
%     NOFF_FILE   filename of the off file with normals
%     OFF_FILE    filename of the off file to write without normals

  noff_fptr = fopen(noff_file,'r');
  off_fptr = fopen(off_file,'w');
  
  % *********** %
  % Read header %
  % *********** %
  str = fscanf(noff_fptr, '%s', 1);
  numVertices = fscanf(noff_fptr, '%d', 1);
  numFaces = fscanf(noff_fptr, '%d', 1);
  numEdges = fscanf(noff_fptr, '%d', 1);
  
  % ************ %
  % Write header %
  % ************ %
  fprintf(off_fptr, 'OFF\n%d %d %d\n', numVertices, numFaces, numEdges);
  
  % ************************* %  
  % Read vertices and normals %
  % ************************* %
  v = fscanf(noff_fptr, '%f %f %f %f %f %f\n', [6 numVertices]);
  
  % ************** %  
  % Write vertices %
  % ************** %
  fprintf(off_fptr, '%f %f %f\n', v(1:3,:));
  
  % *********************************** %
  % Read faces: i.e. connectivity table %
  % *********************************** %
  f = fscanf(noff_fptr, '%d %d %d %d\n', [4 numFaces]);
  
  % ************************************ %
  % Write faces: i.e. connectivity table %
  % ************************************ %
  fprintf(off_fptr, '%d %d %d %d\n', f);

  
  fclose(noff_fptr);
  fclose(off_fptr);

  
return;

%************************************************************************%
%%% $Source: /home/cvs/PROJECTS/SurfaceConstructor/tools/matlabSurfTools/noff2off.m,v $
%%% Local Variables:
%%% mode: Matlab
%%% fill-column: 76
%%% comment-column: 0
%%% End:
