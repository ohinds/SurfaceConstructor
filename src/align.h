/*****************************************************************************
 * align.h is the header file for the slice alignment portion of an
 * application to construct surfaces from serial secions
 * Oliver Hinds <oph@cns.bu.edu> 2004-02-04
 *
 * 
 *
 *****************************************************************************/

#ifndef ALIGN_H
#define ALIGN_H

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>

#include"surfConTypes.h"
#include"surfConUtil.h"
#include"libvpTypes.h"
#include"list.h"
#include"actions.h"

#ifndef MAC
#include<GL/glut.h>
#else
#include<GLUT/glut.h>
#endif

#ifdef IMG_EXPORT
#include<GL/osmesa.h>
#endif

/* externs from surfCon.h */
#include"surfCon.h"
#include"surfCon.extern"

/******************************************************
 * functions
 ******************************************************/

/**
 * load the images and perform init on them
 */
void alignImgInit();

/**
 * initialize general properties
 */
void alignInit();

/**
 * uninitializes the align drawing and imaging stuff
 */
void alignUninit();

/**
 * cleans up
 */
void alignDestroy();

/**
 * check to see if a repaint is needed
 */
int alignRepaintNeeded(long now);

/**
 * do align specific drawing
 */
void alignDraw();

/**
 * handles timer events
 */
void alignTimerEvent(int value);

/** geometric calcs **/

/* /\** */
/*  * apply an appropriate scaling to an x coordinate */
/*  *\/ */
/* double alignScaleX(double x); */

/* /\** */
/*  * apply an appropriate scaling to a y coordinate */
/*  *\/ */
/* double alignScaleY(double y); */

/** 
 * gets the midpoint of a bounding box of a quad
 */
vector getMidPoint(quadri q);

/** 
 * gets the size of a bounding box of a quad
 */
vector getSize(quadri q);

/**
 * get the minimum xy coordinates of a bounding box of a quad
 */
vector getMinXY(quadri q);

/**
 * get the maximum xy coordinates of a bounding box of a quad
 */
vector getMaxXY(quadri q);

/**
 * apply an appropriate scaling to an x coordinate
 */
double applyScaleX(double x);

/**
 * apply an appropriate scaling to a y coordinate
 */
double applyScaleY(double y);

/**
 * translates a point 
 */
void doTranslateAction(double *x, double *y, vector trans);

/**
 * rotates of a point around a specified center by a specified angle
 */
void doRotateAction(double *x, double *y, vector rotCenter, double angle);

/**
 * scales of a point
 */
void doScaleAction(double *x, double *y, vector scale);

/**
 * gets a translation of a quad 
 */
quadri getQuadTranslation(quadri base, vector trans);

/**
 * gets a rotation of a quad around a specified center by a specified angle
 */
quadri getQuadRotation(quadri base, vector rotCenter, double angle);

/**
 * gets a scaling of a quad
 */
quadri getQuadScale(quadri base, vector scale);

/** 
 * sets the center of rotation
 */
void setRotationCenter2D(double x, double y);

/** 
 * sets the center of rotation
 */
void setRotationCenterV(vector rc);

/**
  * get the world coordinates of the rotation center
 */
vector getRotCenterWorldCoords(vector rc);


/** application of actions to the image **/

/**
 * applys a translation to the image and adds one to the list
 */
void applyTranslation(vector trans);

/**
 * applys a rotation to the image and adds one to the list
 */
void applyRotation(vector rotCenter, double angle);

/**
 * applys a scale to the image and adds one to the list
 */
void applyScale(vector scale);

/**
 * apply a set of actions to an image coordinate 
 */
quadri applyActions(list *l, quadri base);

/**
 * reset to initial position
 */
void resetImage();

/**
 * undo last action 
 */
void undoLastAction();

#ifdef IMG_EXPORT
/**
 * apply actions to an input image and write it to a new file
 */
int applyActionsToImage(list *l, char *infile, char *outfile);
#endif

/** event handlers **/

/**
 * menu creation
 */
void createAlignMenu();

/**
 * align actions
 */
void alignAction(int action);

/**
 * keyboard handler
 */
void alignKeyboard(unsigned char key, int x, int y);

/**
 * mouse button handler
 */
void alignMouse(int button, int state, vector mousePos);

/**
 * mouse motion handler
 */
void alignMouseMotion(vector mousePos);

#endif
