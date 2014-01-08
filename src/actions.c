/*****************************************************************************
 * actions.c is the source file for a linked list to hold image actions for
 * the manual alignment application
 * Oliver Hinds <oph@cns.bu.edu> 2004-02-04
 *
 *
 *
 *****************************************************************************/

#include"actions.h"

#define ACTIONS_VERSION_C "$Id: actions.c,v 1.4 2007/05/22 19:18:00 oph Exp $"
/* list actions */

/**
 * adds a translation action to the action list
 */
action *addTranslationAction(list *l, vector trans) {
  /* add a generic new action */
  action *a = (action*) malloc(sizeof(action));

  /* assign specific values for this translation and add it to the list */
  if(a) {
    a->type = TRANSLATION;
    a->trans = trans;

    enqueue(l,a);
  }

  return a;
}

/**
 * adds a rotation action to the action list
 */
action *addRotationAction(list *l, vector rotCenter, double angle) {
  /* add a generic new action */
  action *a = (action*) malloc(sizeof(action));

  /* assign rotation specific variables */
  if(a) {
    a->type = ROTATION;
    a->rotCenter = rotCenter;
    a->angle = angle;

    enqueue(l,a);
  }

  return a;
}

/**
 * adds a scale action to the action list
 */
action *addScaleAction(list *l, vector scale) {
  /* add a generic new action */
  action *a = (action*) malloc(sizeof(action));

  /* assign scale specific variables */
  if(a) {
    a->type = SCALE;
    a->scale = scale;

    enqueue(l,a);
  }

  return a;
}

/**
 * consoldates adjacent actions of like type
 */
void consolidateActions(list *l) {
  int i;
  action* prevAction;
  action* curAction;

  /* test for an empty list */
  if(listSize(l) == 0) return;

  prevAction = (action*) l->head->data;
  /* find adjacent actions of the same type */
  for(i = 1; i < listSize(l); i++) {
    curAction = (action*) getListNode(l,i)->data;

    /* if the action types are the same, collapse them */
    if(prevAction->type == curAction->type) {
      /* different method based on action type */
      if(prevAction->type == ROTATION) {
        /* make sure the centers of rotation are the same */
        if(fabs(prevAction->rotCenter.x - curAction->rotCenter.x) > TOL
           || fabs(prevAction->rotCenter.y - curAction->rotCenter.y) > TOL){
          prevAction = curAction;
          continue;
        }

        /* compose the two rotations */
        prevAction->angle += curAction->angle;
        prevAction->angle = fmod(prevAction->angle,360.0);

        /* delete this action */
        removeListNode(l,i);
        i--;
      }
      else if(prevAction->type == TRANSLATION) { /* translation */
        /* add our translation to the prev one */
        prevAction->trans.x += curAction->trans.x;
        prevAction->trans.y += curAction->trans.y;

        /* delete this action */
        removeListNode(l,i);
        i--;
      }
      else if(prevAction->type == SCALE) { /* scale */
        /* add this scale to the previous one */
        prevAction->scale.x += curAction->scale.x;
        prevAction->scale.y += curAction->scale.y;

        /* delete the action */
        removeListNode(l,i);
        i--;
      }
    }
    else { /* action was of different type */
      prevAction = curAction;
    }
  }
}

/**
 * prints the contents of one node to an io stream
 */
void printAction(action *a, FILE* str) {
  if(a->type == ROTATION) {
    fprintf(str, "rotation: (%g,%g) %g\n" , a->rotCenter.x, a->rotCenter.y, a->angle);
  }
  else if(a->type == TRANSLATION) {
    fprintf(str, "translation: (%g,%g)\n" , a->trans.x, a->trans.y);
  }
  else if(a->type == SCALE) {
    fprintf(str, "scale: (%g,%g)\n" , a->scale.x, a->scale.y);
  }
}

/**
 * dumps the contents of a list to stdout
 */
void dumpActionList(list* l) {
  int i;
  action *curAction;

  for(i = 0; i < listSize(l); i++) {
    curAction = (action*) getListNode(l,i)->data;
    printAction(curAction,stdout);
  }
}
