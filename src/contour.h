/*****************************************************************************
 * contour.h is the header file for the contour editing portion of an
 * application to construct surfaces from serial secions
 * Oliver Hinds <oph@cns.bu.edu> 2004-03-16
 *
 * 
 *
 *****************************************************************************/

#ifndef CONTOUR_H
#define CONTOUR_H

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>

#include"surfConUtil.h"
#include"tacks.h"

#ifndef MAC
#include<GL/glut.h>
#else
#include<GLUT/glut.h>
#endif

/* externs from surfCon.h */

#include"surfCon.h"
#include"surfCon.extern"
#include"align.h"

/******************************************************
 * functions
 ******************************************************/

/**
 * load the images and perform init on them
 */
void contourImgInit();

/**
 * initialize general properties
 */
void contourInit();

/**
 * uninitializes the contour drawing and imaging stuff
 */
void contourUninit();

/**
 * cleans up
 */
void contourDestroy();

/**
 * check to see if a repaint is needed
 */
int contourRepaintNeeded(long now);

/**
 * do contour specific drawing
 */
void contourDraw();

/**
 * draw a tack
 */
void drawTack(vertex *t);

/**
 * draw a label
 */
void drawLabel(vertex *t);

/**
 * draw a line between two tacks
 */
void drawLine(vertex *t1, vertex *t2);

/** geom **/



/** event handlers **/

/**
 * contour actions
 */
void contourAction(int action);

/**
 * creates the menu for the contour
 */
void createContourMenu();

/**
 * keyboard handler
 */
void contourKeyboard(unsigned char key, int x, int y);

/**
 * mouse button handler
 */
void contourMouse(int button, int state, vector mousePos);

/**
 * mouse motion handler
 */
void contourMouseMotion(vector mousePos);

/** util **/

/** 
 * validate contours by removing empty contours and finding intersecting ones
 */
int validateContours(dataset *ds);

/**
 * copy a list of tack lists and apply a set of actions to them
 */
list *applyActionsToTacks(list *tackLists, list* actions, 
			  int sliceNum, int dir);

/**
 * unselect all tacks in a contour
 */
void unselectAllTacksContour(list *tacks);

/**
 * unselect all tacks in a slice
 */
void unselectAllTacksSlice(list *contours);

/**
 * unselect all tacks in the dataset
 */
void unselectAllTacksDataset(dataset *ds);

/**
 * select all tacks in a contour
 */
void selectAllTacksContour(list *tacks);

/**
 * select all tacks in a slice
 */
void selectAllTacksSlice(list *contours);

/**
 * select all tacks in the dataset
 */
void selectAllTacksDataset(dataset *ds);

/**
 * select the tacks at endpoints of segments that intersect 
 */
int selectIntersecting(list *contours);

/**
 * select contour intersection points 
 */
int selectContourIntersections(contour *c1, contour *c2);

/**
 * tests for two dimensional segment intersection 
 */
int segmentsIntersect2D(vertex *v11, vertex *v12, vertex *v21, vertex *v22);

/**
 * smooth all contours
 */
void smoothAllContours(double factor);

/**
 * smooth the contour across slices 
 */
void smoothContour(int sliceIndex, int contourIndex, double factor);

/**
 * smooth the contour across slices using a gaussian kernel
 */
void smoothContourGauss(int sliceIndex, int contourIndex, double factor);

/**
 * remove a contour from a dataset
 */
void removeContour(dataset *ds, list *slice, contour *cont);

/**
 * break the contour at selected neighbors
 */
void breakContour();

/**
 * join the first two contours found with selected vertices
 */
void joinTwoContours();

/**
 * toggles the closure state of a contour on the current slice
 */
void toggleClosureState(int contIndex);

/**
 * increase the current contour
 */
void increaseContour();

/**
 * decrease the current contour
 */
void decreaseContour();

/**
 * attempt to connect this contour to contours on adjacent slices
 * based on a mouse click position
 */
void connectContours(int sliceInd, int contInd, vector *mousePos);

/**
 * attempt to disconnect this contour to contours on adjacent slices
 * based on a mouse click position
 */
void disconnectContours(int sliceInd, int contInd, vector *mousePos);

/**
 * attach a label to all the selected vertices
 */
void setLabelSelected(int l);

/**
 * find the first selected vertex on the contour, or -1 if none
 */
int findFirstSelectedVertex(contour *cont);

/**
 * gets a contour point color based on a contour id
 */
color getTackColor(int contourNum);

/**
 * gets a contour point color based on a contour id and grayed
 */
color getGrayedTackColor(int contourNum);

/**
 * gets a line color based on a contour id
 */
color getLineColor(int contourNum);

/**
 * gets a line color based on a contour id and grayed
 */
color getGrayedLineColor(int contourNum);

/**
 * gets a label color
 */
color getLabelColor(int labelNum);

/**
 * evalautes a gaussian density with zero mean and unit sd 
 */
double gaussian(double x);

#endif
