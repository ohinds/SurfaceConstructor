/******************************************************************************
 * reconFromSlices.c performs a surface reconstruction from a given
 * slice contour file.
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
#include<correspondence.h>
#include<tile.h>

/** filename vars **/
static char 
  slice_fname[255] = "",
  slicelab_fname[255] = "",
  surf_fname[255]  = "",
  executeName[255] = "";

/** save option for corresponded slices **/
static int saveSlices = 0;
static int onlyCorrespond = 0;
static int dontCorrespond = 0;
static int manualThreshold = 0;
static double corrThresh = -1.0;
static int doFillHoles = TRUE;
//static int numHolesLeft = 1;

/******************************************************
 * getopt declarations
 ******************************************************/

/* argument parsing stuff */

#include <unistd.h>

int getopt(int argc, char * const argv[],
		   const char *optstring);

extern char *optarg;
extern int optind, opterr, optopt;

#define _GNU_SOURCE
#include <getopt.h>

int getopt_long(int argc, char * const argv[],
				const char *optstring,
				const struct option *longopts, int *longindex);


/**
 * lowerize a string
 */
void lower(char *str) {
  int i;
  for(i = 0; str[i] != '\0'; i++) {
    tolower(str[i]);
  }
}

/**
 * prints usage and command line options / flags
 */
void printUsage(void) {
  fprintf(stdout, "\n");

  /* print usage */
  fprintf(stdout, "Usage: %s [OPTIONS] <slicefile> [<surffile>]\n", 
	  executeName);

  fprintf(stdout, "\nOPTIONS:\n");  
  fprintf(stdout, "  -r, --resamplecontours    whether to resample the input contours (default is 1).\n");
  fprintf(stdout, "  -R, --resamplecontoursequal resample the input contours, creating vertex spacing equal to the inter-slice distance (default is 0).\n");
  fprintf(stdout, "  -a, --resampledistance    set the resampling distance to this multiplier of the interslice distance (default 2.0).\n");
  fprintf(stdout, "  -A, --resampleabs         indicates that resample distance should be absolute (default 0).\n");
  fprintf(stdout, "  -c, --correspondmethod    method to use for correspondence. \n\t\t\t\tcan be d,h,k (density, histogram, kmeans, none)\n");
  fprintf(stdout, "  -S, --correspondscope     scope to use for correspondence. \n\t\t\t\tcan be l,g (local,global)\n");
  fprintf(stdout, "  -p, --minpercentile       minimum percentile of contour distances to consider\n");
  fprintf(stdout, "  -P, --maxpercentile       maximum percentile of contour distances to consider\n");
  fprintf(stdout, "  -t, --corrthresh          manual correspondence threshold\n");
  fprintf(stdout, "  -m, --nummeans            number of means for correspondence clustering\n");
  fprintf(stdout, "  -s, --saveslices          save the corresponded slices\n");
  fprintf(stdout, "  -d, --saveDistances       save the distances between contours in a text file\n");
  fprintf(stdout, "  -f, --distancefile        name of file to save the distances between contours\n");
  fprintf(stdout, "  -i, --penalizeintersection whether to penalize surface self-intersection (default 1)\n");
  fprintf(stdout, "  -I, --intersectionmult    how much to penalize self-intersection (default 0.1)\n");
  fprintf(stdout, "  -e, --penalizerepeatededges whether to penalize pinching (default 0)\n");
  fprintf(stdout, "  -E, --repeatededgemult    how much to penalize pinching (default 0.2)\n");
  fprintf(stdout, "  -k, --cappingmethod       method to use for cocontour capping. \n\t\t\t\tcan be s,p,n (skeleton, point, none)\n");
  fprintf(stdout, "  -C, --correspond          don't reconstruct, only correspond (with -s)\n");
  fprintf(stdout, "  -#, --skipslices          skip every #th slice in recon\n");
  fprintf(stdout, "  -M, --minind              minimum slice index for reconstruction\n");
  fprintf(stdout, "  -X, --maxind              maximum slice index for reconstruction\n");
  fprintf(stdout, "  -b, --almostclosedmult    multiplier controlling contour closure during branching\n");
  fprintf(stdout, "  -q, --cliqueradius        number of neighbors on either side of vertex to consider in liklihood estimation during branching\n");
  fprintf(stdout, "  -F, --nofillholes         dont fill holes after tiling\n");
  //  fprintf(stdout, "  -H, --numholes            number of holes to leave when filing\n");
  fprintf(stdout, "  -l, --slicelabfile        slice label filename\n");
  fprintf(stdout, "  -V, --verbose             turn on verbose messages\n");
  fprintf(stdout, "  -h, --help                display this help and exit\n");
  fprintf(stdout, "  -?                        display this help and exit\n");

  fprintf(stdout, "\nReport bugs to <oph@bu.edu>.\n");


  return;
}

/**
 * parse the command line arguments
 */
void parseArgs(int argc, char **argv) {
  int opt, option_index = 0;
  char  *dot_ind;

  static struct option long_options[] = {
    {"resamplecontours",1, 0, 'r'},
    {"resampleequal",   1, 0, 'R'},
    {"resampledistance",1, 0, 'a'},
    {"resampleabs",     1, 0, 'A'},
    {"correspondmethod",1, 0, 'c'},
    {"minpercentile",   1, 0, 'p'},
    {"maxpercentile",   1, 0, 'P'},
    {"correspondscope", 1, 0, 'S'},
    {"corrthresh",      1, 0, 't'},
    {"nummeans",        1, 0, 'm'},
    {"saveslices",      0, 0, 's'},
    {"savedistances",   0, 0, 'd'},
    {"distancefile",    0, 0, 'f'},
    {"cappingmethod",   1, 0, 'k'},
    {"correspond",      0, 0, 'C'},
    {"circularitythresh",1, 0, 'T'},
    {"minind",          1, 0, 'M'},
    {"maxind",          1, 0, 'X'},
    {"almostclosedmult",1, 0, 'b'},
    {"cliqueradius",    1, 0, 'q'},
    {"penalizeintersection",1, 0, 'i'},
    {"intersectionmult",1, 0, 'I'},
    {"penalizerepeatededges", 1, 0, 'e'},
    {"nofillholes",       1, 0, 'F'},
    //    {"numholes",        1, 0, 'H'},
    {"repeatededgemult",1, 0, 'E'},
    {"slicelabfile",1, 0, 'l'},
    {"verbose",         0, 0, 'V'},
    {"help",            0, 0, 'h'},
    {0, 0, 0, 0}
  };

  /* loop through input arguments */
  while(1) {
    opt = getopt_long (argc, argv, "r:R:a:A:c:p:P:S:t:m:#:M:X:b:q:si:I:e:E:df:k:CTFH:l:hV?",
		       long_options, &option_index);

    if(opt == -1)
      break;

    if(opt == ':') {
      printUsage();
      exit(EXIT_FAILURE);
    }

    if(opt == '?') {
      printUsage();
      exit(EXIT_SUCCESS);
    }

    switch(opt) {
    case 'r':
      lower(optarg);
      resample = (atoi(optarg) != 0 && strcmp(optarg,"false"));
      break;
    case 'R':
      resampleEqual = (atoi(optarg) != 0 && strcmp(optarg,"false"));
      resample = TRUE;
      break;
    case 'a':
      resampleDistance = atof(optarg);
      resampleEqual = FALSE;
      break;
    case 'A':
      resampleAbs = (atoi(optarg) != 0 && strcmp(optarg,"false"));
      resampleEqual = FALSE;
      break;
    case 'c':
      if(!strcmp(optarg,"h") || !strcmp(optarg,"histogram")) {
	correspondenceMethod = HISTOGRAM;
      }
      else if(!strcmp(optarg,"k") || !strcmp(optarg,"kmeans")) {
	correspondenceMethod = KMEANS;
      }
      else {
	dontCorrespond = TRUE;
      }
      break;
    case 'p':
      minPercentile = atof(optarg);
      break;
    case 'P':
      maxPercentile = atof(optarg);
      break;
    case 't':
      corrThresh = atof(optarg);
      dontCorrespond = TRUE;
      manualThreshold = TRUE;
      break;
    case 'm':
      correspondenceNumMeans = atoi(optarg);
      break;
    case '#':
      skipSlices = atoi(optarg);
      break;
    case 'M':
      minSliceInd = atoi(optarg);
      break;
    case 'X':
      maxSliceInd = atoi(optarg);
      break;
    case 'b':
      almostClosedMultiplier = atof(optarg);
      break;
    case 'q':
      cliqueRadius = atoi(optarg);
      break;
    case 'i':
      lower(optarg);
      testEdgeCrossing = (atoi(optarg) != 0 && strcmp(optarg,"false"));
      break;
    case 'I':
      edgeCrossingPenalty = atof(optarg);
      testEdgeCrossing = TRUE;
      break;
    case 'e':
      lower(optarg);
      testRepeat = (atoi(optarg) != 0 && strcmp(optarg,"false"));
      break;
    case 'E':
      repeatedEdgePenalty = atof(optarg);
      testRepeat = TRUE;
      break;
    case 's':
      saveSlices = TRUE;
      break;
    case 'd':
      saveContourDistances = TRUE;
      break;
    case 'f':
      strcpy(contourDistancesFilename,optarg);
      break;
    case 'k':
      if(!strcmp(optarg,"s") || !strcmp(optarg,"skeleton")) {
	cappingMethod = TILE_SKELETON;
      }
      else if(!strcmp(optarg,"p") || !strcmp(optarg,"point")) {
	cappingMethod = TILE_POINT;
      }
      else {
	cappingMethod = NO_CAPPING;
      }
      break;
    case 'S':
      if(!strcmp(optarg,"l") || !strcmp(optarg,"local")) {
	correspondenceScope = LOCAL;
      }
      else if(!strcmp(optarg,"g") || !strcmp(optarg,"global")) {
	correspondenceScope = GLOBAL;
      }
      break;
    case 'C':
      onlyCorrespond = TRUE;
      break;
    case 'T':
      thresholdForCircularity = atof(optarg);
      break;
    case 'F': 
      doFillHoles = FALSE;
      break;
//    case 'H': 
//      numHolesLeft = atoi(optarg);
//      break;
    case 'l':
      strcpy(slicelab_fname,optarg);
      break;
    case 'V':
      SR_VERBOSE = TRUE;
      break;
    case 'h': case '?':
      printUsage();
      exit(EXIT_SUCCESS);
      break;
    }
  }

  /* validate args */

  /* check for slice filename */
  if(argc - optind <= 0) {
    fprintf(stderr,"error: missing slice filename.\n");
    printUsage();
    exit(0);
  }

  /* get the slice filename */
  strcpy(slice_fname,argv[optind]);

  /* validate filename */
  if(access(slice_fname,R_OK)) {
    fprintf(stderr,"error: can't access %s\n",slice_fname);
    printUsage();
    exit(0);
  }

  /* get the surf filename */
  if(argc - (optind+1) <= 0) {
    strcpy(surf_fname,slice_fname);
    dot_ind = strrchr(surf_fname,'.');
    if(dot_ind == NULL) {
      strcat(surf_fname,".off");
    }
    else {
      strcpy(dot_ind,".off\0");
    }
  }
  else {
    strcpy(surf_fname,argv[optind+1]);
  }
  
}


/**
 * main
 */
int main(int argc, char **args) {
  list *slices;
  surface *surf;

  /* turn on memory tracing, if defined */
#ifdef MEMLEAK
#include<mcheck.h>
  putenv("MALLOC_TRACE=reconFromSlices_memtrace.txt");
  mtrace();
#endif

  strcpy(executeName,args[0]);
  parseArgs(argc,args);

  /* read the slices from the file */
  slices = readSliceContours(slice_fname);
  if(slices == NULL) {
    return 0;
  }

  if(strcmp(slicelab_fname,"")) {
    readSliceContourLabels(slices,slicelab_fname);
  }

  preprocessSliceContours(slices);

  /* builds a guess at contour correspondence */
  if(!dontCorrespond) {
    buildCorrespondenceGuess(slices);
  }
  else if(manualThreshold) {
    setAdjacentContoursManual(slices,corrThresh);
  }

  if(saveSlices) {
    writeSliceContours(slices,"reconSliceSave.slc");
  }

  if(onlyCorrespond) {
    return 0;
  }

  /* tile the slices */
  surf = tileSlices(slices);
  deleteSliceContours(slices);

  /* fill holes, if desired */
  if(doFillHoles) {
    fillBranchedHoles(surf);
  }

  if(surf->manifoldness == SURF_MANIFOLD) {
    fprintf(stdout,"surface is manifold with %d connected components\n",
	    listSize(surf->CC));
  } 
  else {
    fprintf(stdout,"surface is not manifold\n");
  }

  /* write the surface file */
  writeOFF(surf,surf_fname);

  if(strcmp(slicelab_fname,"")) {
    strcpy(strrchr(surf_fname,'.'),".lab");
    writeLabelFile(surf,surf_fname);
  }

  deleteSurface(surf);

  return 0;
}

/************************************************************************
*** $Source: /home/cvs/PROJECTS/SurfaceConstructor/tools/cutil/reconFromSlices.c,v $
*** Local Variables:
*** mode: c
*** fill-column: 76
*** End:
************************************************************************/
