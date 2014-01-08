/*****************************************************************************
 * surfConTypes.h is the header file containing type declarations for 
 * surfCon
 * Oliver Hinds <oph@cns.bu.edu> 2004-06-07
 *
 *
 *
 *****************************************************************************/

#ifndef SURF_RECON_TYPES_H
#define SURF_RECON_TYPES_H

#define SURF_RECON_TYPES_VERSION_H "$Id: surfConTypes.h,v 1.9 2007/05/22 19:18:00 oph Exp $"

#include"libvpTypes.h"
#include"libsrTypes.h"

/* redefines */

#define MAX_STR_LEN VP_MAX_STR_LEN
#define TOL         VP_TOL
#define PI          VP_PI
#define SUCCESS     VP_SUCCESS
#define FAILURE     VP_FAILURE

/** action **/

/* action types */
enum ACTIONTYPE {TRANSLATION, ROTATION, SCALE};

/* node for list type */
typedef struct {
  enum ACTIONTYPE type; /* type of action */

  /* translation */
  vector trans; 

  /* rotation */
  double angle;
  vector rotCenter; 

  /* scaling */
  vector scale;
} action;

/** label **/
typedef struct {
  int slice, cont, vert, lab;
  list *sliceLink;
  contour *contourLink;
  vertex *vertexLink;
} label;

/** dataset information */
typedef struct {
  /* general dataset info */
  char filename[MAX_STR_LEN];         /*file to read and write this struct to*/
  char directory[MAX_STR_LEN];        /*directory where slice images are*/
  int numSlices;                      /*number of slices */
  double sliceDist;                   /*distance between slices */
  double firstSliceZ;                 /*first slice z coordinate*/
  char positionFilename[MAX_STR_LEN]; /*file for individual slice positions */
  char versionString[MAX_STR_LEN];    /*dataset code version                */

  /* slice image filenames */
  char **sliceFilenames;

  /* slice images */
  enum INPUTTYPE imageSource;
  enum IMAGEFORMAT imageFormat;
  image *slices;
  volume *vol;

  /* width and height of images in dataset */
  int width, height;

  /* slice image textures */
  unsigned int *sliceTextures;

  /* align actions */
  list *sliceActionLists;

  /* contours */
  list *sliceContourLists;
  int hasAdjacency;

  /* label lists */
  //  list *sliceLabels;
  int hasLabels;

  /* point lists */
  list *sliceMarkerLists;
  int hasMarkers;

  /* surface info */
  list *vertices;
  list *faces;

  /* coordinate system */
  vector pixelsPerMM;
  vector origin;    
  double rotation;

  /* image processing */
  float contrastAdjust;
  float brightnessAdjust;
} dataset;

#endif
