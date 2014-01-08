/*****************************************************************************
 * actions.h is the header file for a linked list to hold image actions for 
 * the manual alignment application
 * Oliver Hinds <oph@cns.bu.edu> 2004-02-04
 *
 * 
 *
 *****************************************************************************/

#ifndef ACTIONS_H
#define ACTIONS_H

#define ACTIONS_VERSION_H "$Id: actions.h,v 1.5 2007/05/22 19:18:00 oph Exp $"

#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<math.h>

#include"surfConTypes.h"
#include"surfConUtil.h"

#include"list.h"
#include"surfCon.h"

/* list actions */

/**
 * adds a translation action to the action list
 */
action *addTranslationAction(list *list, vector trans); 

/**
 * adds a rotation action to the action list
 */
action *addRotationAction(list *list, vector rotCenter, double angle); 

/**
 * adds a scale action to the action list
 */
action *addScaleAction(list *list, vector scale); 

/** 
 * consolidates adjacent actions of like type 
 */
void consolidateActions(list *list);

/**
 * prints the contents of one node to an io stream 
 */
void printAction(action *a, FILE* str);

/**
 * dumps the contents of a list to stdout
 */
void dumpActionList(list* l);

#endif
