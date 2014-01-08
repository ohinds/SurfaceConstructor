function surfStruct = readMGHsurf(filestring)
% surfStruct = readMGHsurf(filestring)
%
% NOTE: this was cannibalized from the read_surf.m in
% ~fischl/matlab on Oct 4, 2001. (So if the formats for MGH
% surfaces change, this function will become obsolete).
% 
% e.g. readMGHsurf('lh.white')

% (mukundb, Oct 4, 2001)

  TRIANGLE_FILE_MAGIC_NUMBER =  16777214 ;
  QUAD_FILE_MAGIC_NUMBER =  16777215 ;
  NEW_QUAD_FILE_MAGIC_NUMBER =  16777213 ;

  fid = fopen(filestring, 'rb', 'b') ;
  if ( fid < 0 ),
    str = sprintf('could not open surface file %s.', filestring) ;
    error(str) ;
  end;

  [filepath, filename] = fileparts(filestring);
  if ( isempty(filepath) ),
    surfstr = fullfile(pwd, filename);
  else,
    surfstr = filestring;
  end;
  
  surfStruct.surffile = surfstr;

  try, 
    magic = fread3(fid) ;

    if (magic == QUAD_FILE_MAGIC_NUMBER) % old, quad format
      
      vnum = fread3(fid) ;
      fnum = fread3(fid) ;
      vertex_coords = fread(fid, vnum*3, 'int16') ./ 100 ; 

      % Note: the quadfaces are stored as 3 byte ints
      
      quadfaces = fread(fid, 3*4*fnum, 'uchar');
      quadfaces = reshape(quadfaces, [3 4 fnum]);
      quadfaces = permute(quadfaces, [3 2 1]);
      quadfaces = ( bitshift(quadfaces(:,:,1), 16) + ...
		    bitshift(quadfaces(:,:,2),  8) + ...
		    quadfaces(:,:,3) );

      faces = zeros(fnum*2, 3);
      faces(1:2:fnum*2, :) = quadfaces(:,[1 2 3]);
      faces(2:2:fnum*2, :) = quadfaces(:,[1 3 4]);
    elseif (magic == NEW_QUAD_FILE_MAGIC_NUMBER) % new quad format
      
      vnum = fread3(fid) ;
      fnum = fread3(fid) ;
      vertex_coords = fread(fid, vnum*3, 'float32') ;

      % Note: the quadfaces are stored as 3 byte ints
      
      quadfaces = fread(fid, 3*4*fnum, 'uchar');
      quadfaces = reshape(quadfaces, [3 4 fnum]);
      quadfaces = permute(quadfaces, [3 2 1]);
      quadfaces = ( bitshift(quadfaces(:,:,1), 16) + ...
		    bitshift(quadfaces(:,:,2),  8) + ...
		    quadfaces(:,:,3) );

      faces = zeros(fnum*2, 3);
      faces(1:2:fnum*2, :) = quadfaces(:,[1 2 3]);
      faces(2:2:fnum*2, :) = quadfaces(:,[1 3 4]);
  
    elseif (magic == TRIANGLE_FILE_MAGIC_NUMBER) % new, tri format

      fgets(fid) ;
      fgets(fid) ;
      vnum = fread(fid, 1, 'int32') ;
      fnum = fread(fid, 1, 'int32') ;
      vertex_coords = fread(fid, vnum*3, 'float32') ; 
      faces = fread(fid, fnum*3, 'int32') ;
      faces = reshape(faces, 3, fnum)' ;

    end

    vertex_coords = reshape(vertex_coords, 3, vnum)' ;

  catch,
    error('file "%s" is not a valid surface file!', filename);
  end;

  fclose(fid) ;

  surfStruct.vertices = vertex_coords;
  surfStruct.faces = faces + 1; % 0-based to 1-based

  return;

  
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function [retval] = fread3(fid)
% read a 3 byte integer out of a file

  b1 = fread(fid, 1, 'uchar') ;
  b2 = fread(fid, 1, 'uchar') ;
  b3 = fread(fid, 1, 'uchar') ;
  retval = bitshift(b1, 16) + bitshift(b2,8) + b3 ;

