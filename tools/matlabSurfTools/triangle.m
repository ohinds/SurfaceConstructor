function output = triangle(input, command)
% Triangle
% A Two-Dimensional Quality Mesh Generator and Delaunay Triangulator.
% Version 1.4
%
% Copyright 1993, 1995, 1997, 1998, 2002 Jonathan Richard Shewchuk
%
% MATLAB EXecutable (MEX) version 1.0
% Computer Vision and Computational Neuroscience Laboratory
% Boston University
%
%
% usage:   output = triangle(input, command)
%
%   "input" is a structure containing the input data normally sent to
%   triangle via input files, and "command" is a string consisting of
%   command line flags intented for triangle.
%
%   See the help provided with "tricommands.m" and "tristruct.m" for more
%   information on the command and input struct syntax.
%
%   The minimum input data requirement for triangle is a input struct field
%   "pl" consisting of a 2-by-N matrix containing the x-y coordinates of the
%   vertices, where the first row corresponds to the `x' coordinate and the
%   second row the `y' coordinate.
%
%
% input fields:
%    pl, pal, pml, tl, tal, tarl, sl, sml, hl, rl, nc
%
% output fields:
%    pl, pal, pml, tl, tal, nl, sl, sml, el, eml, vpl, vpal, vel, vnrl, nc
%
%
% examples:
%
%   output = triangle(input, '')         % vanilla Delaunay
%   output = triangle(input, 'pc')       % constrained Delaunay
%   output = triangle(input, 'q')        % quality mesh generation

% Gen-Nan Chen, 01/2001:           Initial C MEXification (Windows and Linux)
% Mukund Balasubramanian 02/2001:  MATLAB interface and organization
% Jonathan Polimeni, 06/2003:      Additional error checking and clean-up

% $Id: triangle.m,v 1.1 2005/08/23 14:46:13 oph Exp $
%**************************************************************************%

  if ( nargin ~= 2 ),
	errstr = 'two input arguments required:   output = triangle(input, command)';
	error('triangle:syntax', errstr);
  elseif ( nargout > 1 ),
	errstr = 'one output argument provided:   output = triangle(input, command)';
	error('triangle:syntax', errstr);
  end;


  % if input struct is missing fields, insert missing fields and
  % initialize their contents to the empty set "[]"
  [input, errstr] = pad_input(input);
  if ( ~isempty(errstr) ),
	error('triangle:invalid_fields', errstr);
  end;

  % parse input struct fields for invalid data
  errstr = validate_input(input);
  if ( ~isempty(errstr) ),
	error('triangle:invalid_fields', errstr);
  end;

  % parse input command string and check for consistency with input data
  [errstr, warnstr] = validate_command(command, input);
  if ( ~isempty(errstr) ),
	error('triangle:invalid_command', errstr);
  end;
  if ( ~isempty(warnstr) ),
	for wi = 1:length(warnstr),
	  warning('triangle:invalid_command', warnstr{wi});
	end;
  end;

  
  % *** MEX function call ***

  [output.pl, output.pal, output.pml, ...
   output.tl, output.tal, output.nl, ...
   output.sl, output.sml, ...
   output.el, output.eml, ...
   output.vpl, output.vpal, output.vel, output.vnrl, ...
   output.nc] = ...
	  trimex(command, ...
			 input.pl, input.pal, input.pml, ...
			 input.tl, input.tal, input.tarl, ...
			 input.sl, input.sml, ...
			 input.hl, input.rl, ...
			 input.nc);

  
  % raise error messages if triangle fails and output fields are empty
  errstr = validate_output(output);
  if ( ~isempty(errstr) ),
	error('triangle:invalid_fields', errstr);
  end;

  % trimex terminated successfully!  
  return;


%**************************************************************************%
function [input, errstr] = pad_input(input)
% pad_input:  insert missing fields and initialize their contents to the
% empty set "[]"

  % initialize errstr---if no errors are found, return empty string
  errstr = [];

  %% point list
  if ( ~isfield(input,'pl') ),
	errstr = 'input needs to have a field "pl", a point list';
	return;
  end;

  %% point attribute list
  if ( ~isfield(input,'pal') ),
	% NOTE: for some reason, you need inputs pl and pal for the MEX
	% function to NOT crash when you give the "v" Voronoi flag!
	input.pal = [];
	% 	input.pal = 1:N;
  end;

  %% point marker list
  if ( ~isfield(input,'pml') ),
	input.pml = [];
  end;

  %% triangle list
  if ( ~isfield(input,'tl') ),
	input.tl = [];
  end;

  %% triangle attribute list
  if ( ~isfield(input,'tal') ),
	input.tal = [];
  end;

  %% triangle area list
  if ( ~isfield(input,'tarl') ),
	input.tarl = [];
  end;

  %% segment list
  if ( ~isfield(input,'sl') ),
	input.sl = [];
  end;

  %% segment marker list
  if ( ~isfield(input,'sml') ),
	input.sml = [];
  end;

  %% hole list
  if ( ~isfield(input,'hl') ),
	input.hl = [];
  end;

  %% region list
  if ( ~isfield(input,'rl') ),
	input.rl = [];
  end;

  %% number of corners scalar
  if ( ~isfield(input,'nc') ),
	input.nc = [];
  end;

  % no errors found!
  return;


%**************************************************************************%
function errstr = validate_input(input)
% validate_input:  parse input struct fields for invalid data

  % initialize errstr---if no errors are found, return empty string
  errstr = [];

  % template for error string common to fields that are constrained to be
  % finite, real, and numeric (i.e., all of the fields)
  invalid_str = 'input %s must be a finite, real, numeric array';


  %     pl:        Input point list (2 X iN)
  if ( ~isempty(input.pl) ),
	if ( ~isvalidnum(input.pl) ),
	  errstr = sprintf(invalid_str, 'pl');
	  return;
	end;

	[rows, cols] = size(input.pl);

	if ( rows ~= 2 ),
	  errstr = 'vertex list must consist of 2 rows';
	  return;
	end;

	if ( cols < 3 ),
	  errstr = 'triangles must have at least 3 vertices';
	  return;
	end;

  	% save the number of vertices to check whether it is consistent
    % across fields
	N = cols;

  end;


  %     pal:       Input point attribute list (point attribute numbers X iN)
  if ( ~isempty(input.pal) ),
	if ( ~isvalidnum(input.pal) ),
	  errstr = sprintf(invalid_str, 'pal');
	  return;
	end;

	[rows, cols] = size(input.pal);

	if ( cols ~= N ),
	  errstr = 'number of vertex attributes does not equal number of vertices';
	  return;
	end;

  end;


  %     pml:       Input point marker list (1 X iN)
  if ( ~isempty(input.pml) ),
	if ( ~isvalidnum(input.pml) ),
	  errstr = sprintf(invalid_str, 'pml');
	  return;
	end;

	if (  any( rem(input.pml, 1) | (input.pml < 0) | (input.pml > 1) )  ),
	  errstr = 'boundary markers must be either 0 or 1';
	  return;
	end;

	[rows, cols] = size(input.pml);

	if ( rows ~= 1 ),
	  errstr = 'vertex marker list must consist of 1 row';
	  return;
	end;

	if ( cols ~= N ),
	  errstr = 'number of vertex point markers does not equal number of vertices';
	  return;
	end;

  end;


  %     tl:        Input triangle list (3 X iM )
  if ( ~isempty(input.tl) ),
	if ( ~isvalidnum(input.tl) ),
	  errstr = sprintf(invalid_str, 'tl');
	  return;
	end;

	if (  any( rem(input.tl, 1) | (input.tl < 0) )  ),
	  errstr = 'vertex indices in triangle list must be non-negative integers';
	  return;
	end;

	if ( max(input.tl(:)) > N )
	  errstr = 'index in triangle list exceeds number of vertices';
	  return;
	end;

	[rows, cols] = size(input.tl);

	if ( rows ~= 3 ),
	  errstr = 'triangle list must contain 3 rows of vertex indices';
	  return;
	end;

	% save the number of triangles to check whether it is consistent
    % across fields
	M = cols;

  end;


  %     tal:       Input triangle attribute list (triangle attribute numbers X iM)
  if ( ~isempty(input.tal) ),
	if ( ~isvalidnum(input.tal) ),
	  errstr = sprintf(invalid_str, 'tal');
	  return;
	end;

	[rows, cols] = size(input.tal);

	if ( cols ~= M ),
	  errstr = 'number of triangle attributes does not equal number of triangles';
	  return;
	end;

  end;


  %     tarl:      Input triangle area constraint (1 X iM)
  if ( ~isempty(input.tarl) ),
	if ( ~isvalidnum(input.tarl) ),
	  errstr = sprintf(invalid_str, 'tarl');
	  return;
	end;

	[rows, cols] = size(input.tarl);

	if ( rows ~= 1 ),
	  errstr = 'triangle area constraints must consist of 1 row';
	  return;
	end;

	if ( cols ~= M ),
	  errstr = 'number of triangle area constraints does not equal number of triangles';
	  return;
	end;

  end;


  %     sl:        Input segment list (2 X iL)
  if ( ~isempty(input.sl) ),
	if ( ~isvalidnum(input.sl) ),
	  errstr = sprintf(invalid_str, 'sl');
	  return;
	end;

	if (  any( rem(input.sl, 1) | (input.sl < 0) )  ),
	  errstr = 'vertex indices in segment list must be positive integers';
	  return;
	end;

	if ( max(input.sl(:)) > N )
	  errstr = 'index in segment list exceeds number of vertices';
	  return;
	end;

	[rows, cols] = size(input.sl);

	if ( rows ~= 2 ),
	  errstr = 'segment list must contain 2 rows of vertex indices';
	  return;
	end;

	% save the number of segments to check whether it is consistent across
    % fields
	L = cols;

  end;


  %     sml:       Input segment marker list (1 X iL)
  if ( ~isempty(input.sml) ),
	if ( ~isvalidnum(input.sml) ),
	  errstr = sprintf(invalid_str, 'sml');
	  return;
	end;

	if (  any( rem(input.sml, 1) | (input.sml < 0) | (input.sml > 1) )  ),
	  errstr = 'segment markers must be either 0 or 1';
	  return;
	end;

	[rows, cols] = size(input.sml);

	if ( rows ~= 1 ),
	  errstr = 'segment marker list must consist of 1 row';
	  return;
	end;

	if ( cols ~= L ),
	  errstr = 'number of segment markers does not equal number of vertices';
	  return;
	end;

  end;


  %     hl:        Input hole list (2 X iH)
  if ( ~isempty(input.hl) ),
	if ( ~isvalidnum(input.hl) ),
	  errstr = sprintf(invalid_str, 'hl');
	  return;
	end;

	[rows, cols] = size(input.hl);

	if ( rows ~= 2 ),
	  errstr = 'input hole list must consist of 2 rows';
	  return;
	end;

  end;


  %     rl:        Input region list (2 X iR)
  if ( ~isempty(input.rl) ),
	if ( ~isvalidnum(input.rl) ),
	  errstr = sprintf(invalid_str, 'rl');
	  return;
	end;

	[rows, cols] = size(input.rl);

	if ( rows ~= 2 ),
	  errstr = 'region list must consist of 2 rows';
	  return;
	end;

  end;


  %     nc:        Input number of corners
  if ( ~isempty(input.nc) ),
	if ( ~isvalidnum(input.nc) ),
	  errstr = sprintf(invalid_str, 'nc');
	  return;
	end;

	[rows, cols] = size(input.nc);

	if ( (rows ~= 1) | (cols ~= 1) ),
	  errstr = 'number of corners must be a scalar';
	  return;
	end;

	if ( rem(input.nc,1) | (input.nc < 3) ),
	  errstr = 'number of corners must be an integer >= 3';
	  return;
	end;

  end;

  % no errors found!
  return;


%**************************************************************************%
function [errstr, warnstr] = validate_command(command, input)
% validate_command:  parse input command string and check for consistency
% with input data

  % initialize errstr---if no errors are found, return empty string
  errstr = [];

  % initialize warnstr---if no warnings are found, return empty cell
  warnstr = cell(0);

  % constrained Delaunay triangulations are "constrained" by supplying a
  % PSLG in the form of a "segment list"
  if ( ~isempty(findstr(command, 'p')) & isempty(input.sl) ),
	errstr = 'segment field "sl" required for constrained Delaunay';
	return;
  end;

  % if the point set is non-convex and specifying the PSLG is required for
  % proper triangulation, the user can get away with 'p' and no 'c' in the
  % command flags provided that a *valid* boundary is passed as edge
  % segments in the "sl" field of the input struct.
  if ( ~isempty(findstr(command, 'p')) & isempty(findstr(command, 'c')) )
	warnstr{end+1} = ['Constrained Delaunay triangulation requires specification' ...
					  '\n         of a valid boundary either in the form of a segment' ...
					  '\n         list (input field "sl") or the convex hull (command ' ...
					  'flag "c").'];
  end;

  % warn the user that by supplying "z" in the command string requests
  % 0-based indexing as in the C convention
  if ( ~isempty(findstr(command, 'z')) ),

	warnstr{end+1} = ['supplying "z" in the command string requests 0-based' ...
					  ' indexing\n         as in the C convention,' ...
					  ' whereas MATLAB uses 1-based indexing'];
	
	% check all input fields that are indices for 0-based indexing
	if ( ~isempty(input.tl) ),
	  if ( min(input.tl) ~= 1 ),
		warnstr{end+1} = ['0-based indexing chosen, but triangles index' ...
						  ' into vertex list\n           starting at 1'];
	  end;
	end;

	if ( ~isempty(input.sl) ),
	  if ( min(input.sl) ~= 1 ),
		warnstr{end+1} = ['0-based indexing chosen, but segments index' ...
						  ' into vertex list\n         starting at 1'];
	  end;
	end;

  else,

	% check all input fields that are indices for 1-based indexing
	if ( ~isempty(input.tl) ),
	  if ( min(input.tl) < 1 ),
		warnstr{end+1} = ['1-based indexing chosen, but triangles index' ...
						  ' into vertex list', '\n         starting at 0'];
	  end;
	end;

	if ( ~isempty(input.sl) ),
	  if ( min(input.sl) < 1 ),
		warnstr{end+1} = ['1-based indexing chosen, but segments index' ...
						  ' into vertex list\n         starting at 0'];
	  end;
	end;

  end;

  % no errors found!
  return;


%**************************************************************************%
function errstr = validate_output(output)
% validate_output:  parse output struct for empty fields

  % initialize errstr---if no errors are found, return empty string
  errstr = [];

  
  % if triangulation is empty its usually an error, but if other fields
  % are empty is it a good idea to raise an error?
  if ( prod(size(output.tl)) == 0 ),
	errstr = 'trimex produced an empty triangulation';
	return;
  end;
  
  % no errors found!
  return;

  
%**************************************************************************%
function bool = isvalidnum(array)
% isvalidnum:  common set of checks for valid input arrays

  bool = isnumeric(array) & isreal(array) & isfinite(array);


  %************************************************************************%
  %%% $Source: /home/cvs/PROJECTS/SurfaceConstructor/tools/matlabSurfTools/triangle.m,v $
  %%% Local Variables:
  %%% mode: Matlab
  %%% fill-column: 76
  %%% comment-column: 0
  %%% End:
