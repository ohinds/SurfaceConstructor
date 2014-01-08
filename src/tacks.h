/*****************************************************************************
 * tacks.h is the header file for a linked list to hold image tacks for
 * surfCon
 * Oliver Hinds <oph@cns.bu.edu> 2004-03-16
 *
 *
 *
 *****************************************************************************/

#ifndef TACKS_H
#define TACKS_H

#define TACKS_VERSION_H "$Id: tacks.h,v 1.7 2007/05/22 19:18:00 oph Exp $"

#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<math.h>

#include"surfConTypes.h"
#include"surfConUtil.h"
#include"list.h"
#include"surfCon.h"

/* tack size */
#define TACK_SIZE 20 /* size for testing for containment, 
		      * measured in pixels */

/* point testing */
int tackContainsPoint(vertex *t, double x, double y);

/** 
 * tests a tack for inclusion in a rect
 */
int rectContainsTack(vertex *t, double x0, double y0, double x1, double y1);

/* list actions */

/**
 * adds a tack to the tack list
 */
vertex *addTack(list *l, double x, double y, double z);

/**
 * inserts a tack behind a specified tack
 */
vertex *insertTack(list *l, double x, double y, double z, int n);

/**
 * gets the index of a tack
 */
int getIndexOfTack(list *l, vertex *t);

/**
 * gets a tack from a list by location
 */
vertex *getTackByLoc(list *l, double x, double y);

/**
 * gets a tack from a list by inclusion in rectangle
 */
vertex *getTackInRect(list *l, double x0, double y0, double x1, double y1);

/**
 * delete a tack from the list by location
 */
void deleteTackByLoc(list *l, double x, double y);

/**
 * delete a tack from the list by pointer
 */
void deleteTackByPtr(list *l, vertex *t);

/**
 * returns the distance between two tacks
 */
double tackDistance(vertex *t1, vertex *t2);

/**
 * prints the contents of one node to an io stream
 */
void printTack(vertex *t, FILE* str);

/**
 * dumps the contents of a list to stdout
 */
void dumpTackList(list* l);

/**
 * multiplies each tack by a constant
 */
void multTackList(list* l, double multiplier);

#endif
