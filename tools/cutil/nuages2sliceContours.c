/******************************************************************************
 * nuages2sliceContours.c converts the input file format for nuages to that
 * used for the surfCon slice contour format.
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
#include<correspondence.h>
#include<tile.h>

int main(int argc, char **args) {
  char nuages_fname[255] = "", slice_fname[255]  = "", *dot_ind;
  list *slices;

  /* validate input */
  if(argc < 2) {
    fprintf(stderr,"usage: nuages2sliceContours <nuagesfile> [<slicefile>]\n");
    return 0;
  }
  
  /* get the slice filename */
  strcpy(nuages_fname,args[1]);

  /* validate filename */
  if(access(nuages_fname,R_OK)) {
    fprintf(stderr,"error: can't access %s\n",nuages_fname);
    return 0;
  }

  /* get the surf filename */
  if(argc < 3) {
    strcpy(slice_fname,nuages_fname);
    dot_ind = strrchr(slice_fname,'.');
    if(dot_ind == NULL) {
      strcat(slice_fname,".slc");
    }
    else {
      strcpy(dot_ind,".slc\0");
    }
  }
  else {
    strcpy(slice_fname,args[2]);
  }

  /* read the slices from the file */
  slices = readNuagesSlices(nuages_fname);
  if(slices == NULL) {
    return 0;
  }

  /* write the slices to the file */
  writeSliceContours(slices,slice_fname);

  return 0;
}

/************************************************************************
*** $Source: /home/cvs/PROJECTS/SurfaceConstructor/tools/cutil/nuages2sliceContours.c,v $
*** Local Variables:
*** mode: c
*** fill-column: 76
*** End:
************************************************************************/
