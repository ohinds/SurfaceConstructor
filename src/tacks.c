/*****************************************************************************
 * tacks.c is the source file for a linked list to hold coutours for
 * the surface reconstruction
 * Oliver Hinds <oph@cns.bu.edu> 2004-03-16
 *
 *
 *
 *****************************************************************************/

#include"tacks.h"
#include"contour.extern"

#define TACKS_VERSION_C "$Id: tacks.c,v 1.9 2007/05/22 19:18:00 oph Exp $"

/* point testing */

/**
 * tests a point for inclusion in a tack
 */
int tackContainsPoint(vertex *t, double x, double y) {
  return sqrt(pow(t->x - x,2) + pow(t->y - y,2)) < tackWidth/scale;
}

/**
 * tests a tack for inclusion in a rect
 */
int rectContainsTack(vertex *t, double x0, double y0, double x1, double y1) {
  if(sqrt(pow(x1 - x0,2) + pow(y1 - y0,2)) < tackWidth/scale) {
    return tackContainsPoint(t,x0+(x1-x0)/2.0,y0+(y1-y0)/2.0);
  }
  else {
    return t->x >= x0 && t->x <= x1 && t->y >= y0 && t->y <= y1;
  }
}

/* list actions */

/**
 * add a tack to the list
 */
vertex *addTack(list *l, double x, double y, double z) {
  /* allocate space for the tack */
  vertex *t = createVertex();

  if(t) { /* successful allocation */
    t->selected = FALSE;
    t->x = x;
    t->y = y;
    t->z = z;

    t->number = -1;
    t->boundary = FALSE;

    enqueue(l,t);
  }

  return t;
}

/**
 * inserts a tack behind a specified tack
 */
vertex *insertTack(list *l, double x, double y, double z, int n) {
  /* allocate space for the new tack */
  vertex *t = createVertex();

  if(t) { /* successful allocation */
    t->selected = FALSE;
    t->x = x;
    t->y = y;
    t->z = z;

    t->number = -1;
    t->boundary = FALSE;

    insertListNode(l,n,t);
  }

  return t;
}

/**
 * gets the index of a tack
 */
int getIndexOfTack(list * l, vertex *t) {
  listNode *i;
  int ind;

  /* search list for tack */
  for(i = getListNode(l,0),ind=0; i; i = (listNode*) i->next,ind++) {
    if(i->data == t) {
      return ind;
    }
  }

  return -1;
}

/**
 * gets a tack from a list by location
 */
vertex *getTackByLoc(list *l, double x, double y) {
  listNode* i;
  vertex *t;

  /* make sure the list isnt empty */
  if(listSize(l) < 1) {
    return NULL;
  }

  /* find a tack that contains the point */
  for(i=getListNode(l,0); i; i = (listNode*) i->next) {
    t = (vertex*) i->data;
    if(tackContainsPoint(t, x, y)) {
      return t;
    }
  }

  return NULL;
}

/**
 * gets a tack from a list by inclusion in rectangle
 */
vertex *getTackInRect(list *l, double x0, double y0, double x1, double y1) {
  listNode* i;
  vertex *t;
  double swap;

  /* make sure the list isnt empty */
  if(listSize(l) < 1) {
    return NULL;
  }

  /* make the appropriate coordinates minimum */
  if(x1 < x0) {
    swap = x0;
    x0 = x1;
    x1 = swap;
  }
  if(y1 < y0) {
    swap = y0;
    y0 = y1;
    y1 = swap;
  }

  /* find a tack in the rectangle */
  for(i=getListNode(l,0); i; i = (listNode*) i->next) {
    t = (vertex*) i->data;
    if(rectContainsTack(t, x0, y0, x1, y1)) {
      return t;
    }
  }

  return NULL;
}

/**
 * delete a tack from the list by location
 */
void deleteTackByLoc(list *l, double x, double y) {
  deleteTackByPtr(l, getTackByLoc(l,x,y));
}

/**
 * delete a tack from the list by pointer to its list node
 */
void deleteTackByPtr(list *l, vertex *t) {
  listNode *prev = l->head;

  /* if empty, return */
  if(listSize(l) < 1) {
    return;
  }

  /* special case - element to be deleted is first */
  if(t == l->head->data) {
    removeListNode(l,0);
  }
  else {
    /* find the prev element and remove the current one */
    while(prev && ((listNode*) prev->next)->data != t) {
      prev = (listNode *) prev->next;
    }
    if(prev == NULL) return;
    prev->next = ((listNode*) prev->next)->next;
  }

  /* special case - element to be deleted is last */
  if(t == l->tail->data) {
    l->tail = prev;
  }

  free(t);
  l->len--;
}

/**
 * returns the distance between two tacks
 */
double tackDistance(vertex *t1, vertex *t2) {
  return sqrt( pow(t2->x - t1->x, 2) +
               pow(t2->y - t1->y, 2) );
}

/**
 * prints the contents of one node to an io stream
 */
void printTack(vertex *t, FILE* str) {
  fprintf(str, "tack: (%0.30lf %0.30lf)\n" , t->x, t->y);
}

/**
 * dumps the contents of a list to stdout
 */
void dumpTackList(list* l) {
  listNode* i;
  vertex *curTack;

  for(i = getListNode(l,0); i; i = (listNode*) i->next) {
    curTack = (vertex*) i->data;
    printTack(curTack,stdout);
  }
}

/**
 * multiplies each tack by a constant
 */
void multTackList(list* l, double multiplier) {
  listNode* i;
  vertex *curTack;

  for(i = getListNode(l,0); i; i = (listNode*) i->next) {
    curTack = (vertex*) i->data;
    curTack->x *= multiplier;
    curTack->y *= multiplier;
  }
}
