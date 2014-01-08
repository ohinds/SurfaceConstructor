/*****************************************************************************
 * dataset.h is the header file containing function declarations for
 * utility functions for handling datasets in the surfCon application
 * Oliver Hinds <oph@cns.bu.edu> 2004-06-22
 *
 *
 *
 *****************************************************************************/

#ifndef DATASET_H
#define DATASET_H

#define DATASET_VERSION_H "$Id: dataset.h,v 1.11 2007/05/22 19:18:00 oph Exp $"

#include"surfConTypes.h"
#include"surfConUtil.h"

#include"imageUtil.h"
#include"volumeUtil.h"
#include"imageIO.h"
#include"volumeIO.h"
#include"list.h"
#include"libsrUtil.h"

#include<stdio.h>

/**
 * load slice contours from a file and view them with a black background
 */
dataset *newDatasetContourFile(char *filename, char *sliceFilename);

/**
 * creates a dataset from an MGH volume
 */
dataset *newDatasetVolume(char *confFilename, char *volumeFilename);

/**
 * creates a dataset from a set of images
 */
dataset *newDatasetImages(char *confFilename, char *directory,
                          int imageFormat, char **sliceFilenames,
                          int numSlices, double xDist, double yDist,
                          double sliceDist);

/**
 * reads a dataset from a dataset file
 */
dataset *readDataset(char *filename);

/**
 * reads the volume specific dataset params
 */
int readDatasetVolume(FILE *fp, dataset *ds);

/**
 * reads the image specific dataset params
 */
int readDatasetImage(FILE *fp, dataset *ds);

/**
 * reads the no images specific dataset params
 */
int readDatasetNoImages(FILE *fp, dataset *ds);

/**
 * prepares the dataset structure for saving, updates the
 * hasLables, hasMarkers, hasAdjacent flags are set
 */
void prepareDatasetForSave(dataset *ds);

/**
 * saves a dataset from a dataset struct to the file specified in the dataset
 */
int saveDataset(dataset *ds);

/**
 * load the images from the desired locations
 */
int loadImages(dataset *ds);

/**
 * loads the nth image from a dataset
 */
image *loadImage(dataset *ds, int n);

/**
 * image the texture with opengl
 */
GLuint imageTexture(dataset *ds, image *im);

/**
 * loads an image and makes a texture for it
 * either the image is read from a file into memory, or
 * retreived if already loaded
 */
GLuint loadTexture(dataset *ds, char *filename, int format, image *im);

/**
 * unloads a texture to free up memory
 */
void unloadTexture(int i);

/**
 * gets a slice number's filename
 */
int getSliceFilename(dataset *ds, int sliceNum, char *imgFilename);

/**
 * writess a dataset from a dataset file
 */
int writeDataset(dataset *ds, FILE *fp);

/**
 * dumps the align actions for a dataset to a file stream
 */
void dumpAlign(dataset *ds, FILE *fp);

/**
 * dumps the tacks for a dataset to a file stream
 */
void dumpContours(dataset *ds, FILE *fp);

/**
 * dumps the contour adjacency information
 */
void dumpContourAdjacency(dataset *ds, FILE *fp);

/**
 * dumps the labels for the vertices
 */
void dumpLabel(dataset *ds, FILE *fp);

/**
 * dumps the markers for a dataset to a file stream
 */
void dumpMarker(dataset *ds, FILE *fp);

/**
 * dumps the surface info  for a dataset to a file stream
 */
void dumpSurface(dataset *ds, FILE *fp);

/**
 * assign the boundary vertices
 */
void assignBoundaries(dataset *ds);

/**
 * assigns the z coordinates to the contour vertices
 */
void assignZCoords(dataset *ds);

/**
 * checks the version string for compatibility with this version
 */
int checkVersionString(char *otherVersion);

/**
 * makes an id string not an id string by replacing ID with Id
 */
char *unCVS(char * str);

#endif
