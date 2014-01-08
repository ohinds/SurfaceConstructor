/******************************************************************************
* surfSlicer.h reads in an obj file, slices it up into parallel slices and 
* writes out a nuages compatible list of contours
* Oliver Hinds <oph@bu.edu> 2004-08-11
*
* This application is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by the
* Free Software Foundation; either version 2 of the License, or (at your
* option) any later version.
*
* This software is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* General Public License for more details.
*
* You should have received a copy of the GNU General Public License along
* with this application; if not, write to the Free Software Foundation,
* Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*****************************************************************************/

#include "vtkPolyData.h"

#include <stdlib.h>
#include <stdio.h>

/**
 * given a surface, slice it up
 */
vtkPolyData *sliceSurf(vtkPolyData *surf, int numSlices, int dimension);

/**
 * given a surface, slice it up
 */
//vtkPolyData *sliceSurfVTK(vtkPolyData *surf, int numSlices, int dimension);
