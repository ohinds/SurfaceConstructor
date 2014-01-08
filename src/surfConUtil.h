/*****************************************************************************
 * surfConUtil.h is the header file for utility functions for surface recon
 * Oliver Hinds <oph@cns.bu.edu> 2004-02-04
 *
 * 
 *
 *****************************************************************************/

#ifndef SURF_RECON_UTIL_H
#define SURF_RECON_UTIL_H

#include"list.h"

#include"surfConTypes.h"
#include"actions.h"
#include"tacks.h"

#include<stdlib.h>
#include<stdio.h>
#include<jpeglib.h>
#include<ctype.h>

/**
 * save the current display to an image file
 */
void screenCapture();

/**
 * sets the slice direction of a dataset
 */
void setSliceDirection(dataset *ds, int sliceDir);

/* /\** */
/*  * get the window coords of a vector */
/*  *\/ */
/* vector getWindowCoordsV(vector v); */

/* /\** */
/*  * get the window coords of a quad */
/*  *\/ */
/* quadri getWindowCoordsQ(quadri q); */

/**
 * get the image coords of a window space vertex
 */
vertex getImageCoordsVert(vertex v);

/**
 * get the image coords of a window space vector
 */
vector getImageCoordsV(vector v);

/**
 * get the image coords of a window space quad
 */
quadri getImageCoordsQ(quadri q);

/**
 * get the window coords of an image space vertex
 */
vertex getWindowCoordsVert(vertex v);

/**
 * get the window coords of an image space vector
 */
vector getWindowCoordsV(vector v);

/**
 * get the window coords of an image space quad
 */
quadri getWindowCoordsQ(quadri q);

/**
 * get the world coords of an image space  vector
 */
vector getWorldCoordsV(vector v);

/**
 * get the world coords of an image space quad
 */
quadri getWorldCoordsQ(quadri q);

/**
 * build a to world transformation matrix 
 */
void buildToWorldTransform(float A[4][4], enum SLICEDIRECTION dir, 
			   float B[4][4]);

/**
 * copies a list of contours
 */
list *cloneSliceContours(dataset *ds);

/**
 * applies a desired scaling of the display
 */
void doScale(double s);

/**
 * applies a desired offset of the display
 */
void doOffset(double x, double y);

/**
 * write a list of markers to a file stream
 */
int writeMarkers(dataset *ds, FILE* fp);

/**
 * create a list of slice contours to be used as input to the fuchs
 * algorithm from a dataset
 */
list *buildSlices(dataset *ds);

/**
 * transform surface into world coordinates
 */
int surfaceVerts2WorldCoords(surface *surf, dataset *ds);

/**
 * gets the texture method used based on availability of extensions
 */
int getTextureMethod();

#endif




