/******************************************************************************
 * program for testing memory leaks in surfCon
 *
 * Oliver Hinds <oph@bu.edu> 2006-04-19 
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

#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<list.h>
#include<libsrTypes.h>
#include<surfIO.h>
#include<surfUtil.h>
#include<correspondence.h>
#include<tile.h>
#include<mcheck.h>

#include"/home/oph/projects/surfCon/src/surfConUtil.h"

/**
 * main
 */
int main(int argc, char **args) {
  list *slices;
  surface *surf;

  /* turn on memory tracing */
  putenv("MALLOC_TRACE=memtest_memtrace.txt");
  mtrace();

  /* read the slices from the file */
  slices = readSliceContours(args[1]);
  if(slices == NULL) {
    printf("couldn't read slices from %s!\n",args[1]);
    return 0;
  }

  preprocessSliceContours(slices);

  /* builds a guess at contour correspondence */
//  correspondenceMethod = HISTOGRAM;
//  correspondenceScope = LOCAL;  
  buildCorrespondenceGuess(slices);

  /* tile the slices */
//  minSliceInd = 0;
//  maxSliceInd = 40;
  surf = tileSlices(slices);

  deleteSliceContours(slices);

  fillBranchedHoles(surf);

  writeOFF(surf,"testmem.off");
  deleteSurface(surf);

  return 0;
}

/************************************************************************
*** $Source: /home/cvs/PROJECTS/SurfaceConstructor/tools/cutil/testmem.c,v $
*** Local Variables:
*** mode: c
*** fill-column: 76
*** End:
************************************************************************/
