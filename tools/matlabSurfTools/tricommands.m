function tricommands
% To form the command string for the function "triangle" -- take the
% necessary characters below to form a string e.g. 'Qqpcva0.001'
%
%   Q  Quiet:  No terminal output except errors.
%   V  Verbose:  Detailed information on what I'm doing.
%   s  Force segments into mesh by splitting (instead of using CDT).
%   p  Triangulates a Planar Straight Line Graph (.poly file).
%   c  Encloses the convex hull with segments, i.e. output has a
%      list of segments on the convex hull.
%   r  Refines a previously generated mesh.
%   q  Quality mesh generation.  A minimum angle may be specified.
%   qXX: quality mesh generation by Ruppert's Delaunay refinement (XX
%	minimum angle; 20.7 if XX not specified)	
%   a  Applies a maximum triangle area constraint.
%   aXX: impose max tri area (either per triangle with itarl or for all
%	triangles with XX)
%   z  Numbers all items starting from zero (rather than one).
%   v  Generates a Voronoi diagram.
%   i  Uses incremental method, rather than divide-and-conquer.
%   F  Uses Fortune's sweepline algorithm, rather than d-and-c.
%   A  Applies attributes to identify elements in certain regions.
%   e  Generates an edge list.
%   n  Generates a list of triangle neighbors.
%   g  Generates an .off file for Geomview.
%   B  Suppresses output of boundary information.
%   P  Suppresses output of .poly file.
%   N  Suppresses output of .node file.
%   E  Suppresses output of .ele file.
%   I  Suppresses mesh iteration numbers.
%   O  Ignores holes in .poly file.
%   X  Suppresses use of exact arithmetic.
%   o2 Generates second-order subparametric elements.
%   Y  Suppresses boundary segment splitting.
%   S  Specifies maximum number of added Steiner points.
%   l  Uses vertical cuts only, rather than alternating cuts.
%   C  Check consistency of final mesh.
%   h  Help:  Detailed instructions for Triangle.

eval(sprintf('help %s', mfilename));