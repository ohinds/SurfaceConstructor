function tristruct
% Description of the fields in the structure used by the function
% "triangle" for input and output.
% 
% Note: the only required field is the "pl" field. If you want to
% input a PSLG, you also need to specify the "sl" field. All of the
% other fields are pretty much optional as inputs (but may be
% generated on output)
%
% POINTS:
% ------
% pl: 2xV point list (i.e. 2-D coords)
% pal: MxV point attribute list, e.g. M = 3 for RGB vals of each pt
% pml: 1xV point marker list. markers must be integers > 0
%
% SEGMENTS:
% --------
% sl: 2xS segment list
% sml: 1xS segment marker list. markers must be integers > 0
%
% FACES:
% -----
% tl: (inc)xF faces list, where inc is #points per face (input 12)
% tal: MxF face attribute list, e.g. M = 3 for RGB vals of each face
% tarl: 1xF max triangle area list (used when refining)
% nl: 3xF list of triangle neighbors
% inc: number of corners per face (presumably always 3 for triangles)
% 
% EDGES
% -----
% el: 2xE list of edges
% eml: 1xE list of edge markers
%
% HOLES:
% -----
% hl: 2xH list of points (i.e. 2-D coords) inside holes
%
% REGIONS:
% -------
% rl: 4xR region attribute list: 2-D coords for a point inside the
%     region, a regional attribute (presumable a scalar, but also an
%     integer?) and fourthly, maximum area for this region.
%
% VORONOI INFO:
% ------------
% vpl: list of Voronoi center coords
% vpal: attribute list of Voronoi center points
% vel: Voronoi edge list
% vnrl: normal vectors for each Voronoi edge

eval(sprintf('help %s', mfilename));