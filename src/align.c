/*****************************************************************************
 * align.c is the source file for the slice alignment portion of an
 * application to construct surfaces from serial secions
 * Oliver Hinds <oph@cns.bu.edu> 2004-02-04
 *
 *
 *
 *****************************************************************************/

#include"align.h"
#include"contour.extern"

/******************************************************
 * globals
 ******************************************************/

/* pointers to the two current images */
static image *ref;
static image *mov;

/* pointers to the two current image textures */
static GLuint *refTex;
static GLuint *movTex;

/* the slice image quads for positions */
static quadri refImgBounds;
static quadri movImgBounds;

/* pointer to the current action list */
static list *actions;

/* flag to tell which image to display, 0 or 1 */
static int displayImg = 0;

/* flags for rotation and motion */
static int moving = 0;
static int rotating = 0;
static int settingRotCenter = FALSE;

/* current image rotation and translation */
static double curRotAngle = 0;
static vector curRotCenter;
static vector curTransAction;

/* sensitivity modifiers */
static double coarseSensitivity = 1.0;
static double fineSensitivity = 0.1;
static double curSensitivity = 1.0;

/* increments for actions */
static double transIncrX = 0.001;
static double transIncrY = 0.001;
static double scaleIncrX = 0.001;
static double scaleIncrY = 0.001;

/* flag for animation or not */
static int animate = TRUE;
static int framePeriod = 10;

/* flags for display of tacks */
static int showPrevTacks = FALSE;
static int showCurTacks = FALSE;

/* msecs between image switches */
int switchPeriod = 10;
static int switchPerIncrement = 10;
static const int MAX_SWITCH_PERIOD = 1000;

/* size of stuff */
static const int rotCenterSize = 4;

/******************************************************
 * functions
 ******************************************************/

/** initialization and display ops **/

/**
 * prepare the images and perform init on local vars
 */
void alignImgInit() {
  static int firstRun = TRUE;
  image *img;

  /* validate the curSlice */
  curSlice %= curDataset->numSlices;
  if(curSlice < 0) curSlice += curDataset->numSlices;

  /* assign the reference and moving images and textures */
  if(curSlice > 0) {
    /* load the textures if dynamic textures is on */
    if(dynamicTextures) {
      img = loadImage(curDataset, curSlice-1);
      curDataset->slices[curSlice-1] = *img;
      free(img);
    }

    ref = &curDataset->slices[curSlice-1];
    refTex = (GLuint*) &curDataset->sliceTextures[curSlice-1];
  }

  /* zero the translation and rotation */
  curTransAction.x = 0.0;
  curTransAction.y = 0.0;

  /* initialize things that only need it the first run */
  if(firstRun == TRUE) {
    /* get a quadric object */
    if(quad == NULL) {
      quad = gluNewQuadric();
      gluQuadricDrawStyle(quad, GLU_FILL);
    }

    curRotCenter.x = (movImgBounds.v1.x+movImgBounds.v3.x)/2.0;
    curRotCenter.y = (movImgBounds.v1.y+movImgBounds.v3.y)/2.0;

    firstRun = FALSE;
  }

  /* load the textures if dynamic textures is on */
  if(dynamicTextures) {
    img = loadImage(curDataset, curSlice);
    curDataset->slices[curSlice] = *img;
    free(img);
  }

  mov = &curDataset->slices[curSlice];
  movTex = (GLuint*)&curDataset->sliceTextures[curSlice];
}

/**
 * initialize general properties
 */
void alignInit() {
  //vector mid;
  listNode *ln;

  /* apply the moving actions if this isn't the first image */
  if(curSlice > 0) {
    /* find the initial placement of the ref image */
    refImgBounds.v1.x = 0;
    refImgBounds.v1.y = 0;
    refImgBounds.v2.x = ref->width;
    refImgBounds.v2.y = 0;
    refImgBounds.v3.x = ref->width;
    refImgBounds.v3.y = ref->height;
    refImgBounds.v4.x = 0;
    refImgBounds.v4.y = ref->height;

    /* get the action list to apply/modify, create if it doesn't exist */
    while(NULL == (ln=getListNode(curDataset->sliceActionLists,curSlice-1))) {
      actions = newList(LIST);
      enqueue(curDataset->sliceActionLists, actions);
    }
    actions = (list*) ln->data;

    /* apply the actions */
    refImgBounds = applyActions(actions,refImgBounds);
  }

  /* find the initial placement of the moving image */
  movImgBounds.v1.x = 0;
  movImgBounds.v1.y = 0;
  movImgBounds.v2.x = mov->width;
  movImgBounds.v2.y = 0;
  movImgBounds.v3.x = mov->width;
  movImgBounds.v3.y = mov->height;
  movImgBounds.v4.x = 0;
  movImgBounds.v4.y = mov->height;

  curDataset->width = mov->width;
  curDataset->height = mov->height;

  curRotCenter.x = (movImgBounds.v1.x+movImgBounds.v3.x)/2.0;
  curRotCenter.y = (movImgBounds.v1.y+movImgBounds.v3.y)/2.0;

  /* get the moving action list to apply/modify, create if it doesn't exist */
  while(NULL == (ln = getListNode(curDataset->sliceActionLists,curSlice))) {
    actions = newList(LIST);
    enqueue(curDataset->sliceActionLists, actions);
  }
  actions = (list*) ln->data;

  /* apply the actions */
  movImgBounds = applyActions(actions,movImgBounds);

  /* turn on animation */
  animate = TRUE;

  strcpy(alertString,"");

  alignTimerEvent(0);
}

/**
 * uninitializes the align drawing and imaging stuff
 */
void alignUninit() {
  /* load the texture if dynamic textures is on */
  if(dynamicTextures) {
    unloadTexture(curSlice-1);

    if(refTex) {
      unloadTexture(curSlice);
    }
  }

  /* turn off animation */
  animate = FALSE;
}

/**
 * cleans up
 */
void alignDestroy() {
  gluDeleteQuadric(quad);
  quad = NULL;
}

/**
 * check to see if a repaint is needed
 */
int alignRepaintNeeded(long now) {
  static long nextSwitch = 0;

  if(animate && now >= nextSwitch) {
    nextSwitch = now + switchPeriod;
    displayImg = !displayImg;
    return TRUE;
  }

  return FALSE;
}

/**
 * do align specific drawing
 */
void alignDraw() {
  list *l, *cList = NULL;
  listNode *i,*j;
  vertex *t, *prev;
  quadri curPos;    /* to hold current pos image to be displayed */
  vector rc; /* world coords of rotation center */
  //char slicefile1[MAX_STR_LEN];
  float maxTexX, maxTexY;
  GLenum textureMethod = getTextureMethod();

  /* draw the current referrence image */
  if(curSlice != 0 && animate
     && displayImg == 0) { /* just display the reference image */

    maxTexX = (float) ref->width;
    maxTexY = (float) ref->height;

    if(curDataset->imageSource != VP_NOIMAGES) {
      /* turn on texture mapping */
      glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
      glEnable(textureMethod);
      
      /* bind the texture */
      glBindTexture(textureMethod, *refTex);
      
      /* get the window coordinates of the reference image */
      curPos = getWindowCoordsQ(refImgBounds);
      
      if(textureMethod == GL_TEXTURE_2D) {
	maxTexX = ref->width/(float)(ref->width+ref->padX);
	maxTexY = ref->height/(float)(ref->height+ref->padY);
      }
      
      /* make a quadrilateral and provide texture coords */
      glBegin(GL_QUADS); {
	glTexCoord2d(0.0,0.0);
	glVertex3d(curPos.v1.x, curPos.v1.y, 0.0);
	glTexCoord2d(maxTexX,0.0);
	glVertex3d(curPos.v2.x, curPos.v2.y, 0.0);
	glTexCoord2d(maxTexX,maxTexY);
	glVertex3d(curPos.v3.x, curPos.v3.y, 0.0);
	glTexCoord2d(0.0,maxTexY);
	glVertex3d(curPos.v4.x, curPos.v4.y, 0.0);
      } glEnd();
      glDisable(textureMethod);
    }
  }
  else if(curSlice == 0 || !animate
	  || displayImg == 1) { /* apply the current rot & trans */
    /* calculate transformed coordinates based on current rot and trans */
    if(moving) {
      curPos = getQuadTranslation(movImgBounds, curTransAction);
    }
    else if(rotating) { /* rotate the movImg coords bu the current angle */
      curPos = getQuadRotation(movImgBounds, curRotCenter, curRotAngle);
    }
    else { /* no current transformation, draw at expected location */
      curPos = movImgBounds;
    }

    curPos = getWindowCoordsQ(curPos);

    maxTexX = (float) mov->width;
    maxTexY = (float) mov->height;

    if(curDataset->imageSource != VP_NOIMAGES) {
      if(textureMethod == GL_TEXTURE_2D) {
	maxTexX = mov->width/(float)(mov->width+mov->padX);
	maxTexY = mov->height/(float)(mov->height+mov->padY);
      }

      /* turn on texture mapping */
      glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
      glEnable(textureMethod);

      /* bind the texture */
      glBindTexture(textureMethod, *movTex);

      /* get the window coordinates of the moving image */
      //curPos = getWindowCoordsQ(curPos);

      /* make a quadrilateral and provide texture coords */
      glBegin(GL_QUADS); {
	glTexCoord2f(0,0);
	glVertex3d(curPos.v1.x, curPos.v1.y, 0);
	glTexCoord2f(maxTexX,0);
	glVertex3d(curPos.v2.x, curPos.v2.y, 0);
	glTexCoord2f(maxTexX,maxTexY);
	glVertex3d(curPos.v3.x, curPos.v3.y, 0);
	glTexCoord2f(0,maxTexY);
	glVertex3d(curPos.v4.x, curPos.v4.y, 0);
      } glEnd();
      glDisable(textureMethod);
    }
  }

  /* cur slice contours and markers */
  if(showCurTacks && listSize(curDataset->sliceContourLists) > curSlice) {
    glColor3fv(COLORS[DARK_GRAY]);

    /* iterate over contours, drawing each */
    l = (list*)getListNode(curDataset->sliceContourLists,curSlice)->data;
    for(i = getListNode(l,0); i; i = (listNode*) i->next) {
      /* get the contour at index */
      cList = ((contour*)i->data)->vertices;

      /* draw the contour lines, if we need to */
      glPointSize(pixelSize*tackWidth);
      glBegin(GL_LINES); {
	/* iterate over tacks, drawing lines between each */
	prev = listSize(cList) > 1 ? (vertex*) getListNode(cList,0)->data : NULL;
	for(j = getListNode(cList,1); prev && j; j = (listNode*) j->next) {
	  t = (vertex*) j->data;
	  drawLine(prev,t);
	  prev = t;
	}
      } glEnd();

      /* draw the tacks in the contour */
      for(j = getListNode(cList,0); j; j = (listNode*) j->next) {
	t = (vertex*) j->data;
	/* draw the tack */
	drawTack(t);
      }
    }

    /* cur slice markers */
    glColor3fv(COLORS[GRAYED_LIGHT_ORANGE]);
    l = (list*)getListNode(curDataset->sliceMarkerLists,curSlice-1)->data;
    for(i = getListNode(l,0); i; i = (listNode*) i->next) {
      t = (vertex*) i->data;
      /* draw the tack */
      drawTack(t);
    }
  }

  /* prev slice contours and markers */
  if(showPrevTacks && curSlice > 0
     && listSize(curDataset->sliceContourLists) > curSlice-1) {
    glColor3fv(COLORS[GRAY]);

    /* iterate over contours, drawing each */
    l = ((contour*)getListNode(curDataset->sliceContourLists,curSlice-1)->data)->vertices;
    for(i = getListNode(l,0); i; i = (listNode*) i->next) {
      /* get the contour at index */
      cList = (list*)i->data;

      /* draw the contour lines, if we need to */
      glPointSize(pixelSize*tackWidth);
      glBegin(GL_LINES); {
	/* iterate over tacks, drawing lines between each */
	prev = listSize(cList) > 1 ? (vertex*) getListNode(cList,0)->data : NULL;
	for(j = getListNode(cList,1); prev && j; j = (listNode*) j->next) {
	  t = (vertex*) j->data;
	  drawLine(prev,t);
	  prev = t;
	}
      } glEnd();

      /* draw the tacks in the contour */
      for(j = getListNode(cList,0); j; j = (listNode*) j->next) {
	t = (vertex*) j->data;
	/* draw the tack */
	drawTack(t);
      }
    }

    /* cur slice markers */
    glColor3fv(COLORS[GRAYED_LIGHT_ORANGE]);
    l = (list*)getListNode(curDataset->sliceMarkerLists,curSlice-1)->data;
    for(i = getListNode(l,0); i; i = (listNode*) i->next) {
      t = (vertex*)i->data;
      /* draw the tack */
      drawTack(t);
    }
  }

  /* draw the center of rotation */
  glPointSize(rotCenterSize);
  glBegin(GL_POINTS); {
    glColor3fv(COLORS[RED]);
    rc = getWindowCoordsV(curRotCenter);
    glVertex3d(rc.x, rc.y, 0);
  } glEnd();

  /* build info and alert strings */
  //getSliceFilename(curDataset, curSlice, slicefile1);
  //getSliceFilename(curDataset, curSlice+1, slicefile2);
  strcpy(modeString,"align: ");
  sprintf(modeString,"slice %d ",curSlice);
  //strcat(modeString,slicefile1);
  //strcat(modeString," to ");
  //strcat(modeString,slicefile1);
}

/**
 * handles timer events
 */
void alignTimerEvent(int value) {
  /* repaint if necessary */
  if(alignRepaintNeeded(glutGet(GLUT_ELAPSED_TIME))) {
    redisplay();
  }

  /* draw again in the future */
  if(animate) {
    glutTimerFunc(framePeriod, alignTimerEvent, 0);
  }
}

/** geometric calcs **/

/**
 * gets the midpoint of a bounding box  of a quad
 */
vector getMidPoint(quadri q) {
  vector mid,
    minXY = getMinXY(q),
    maxXY = getMaxXY(q);

  mid.x = 0.5*(minXY.x+maxXY.x);
  mid.y = 0.5*(minXY.y+maxXY.y);
  mid.z = 0.0;

  return mid;
}

/**
 * gets the size of a bounding box  of a quad
 */
vector getSize(quadri q) {
  vector siz,
    minXY = getMinXY(q),
    maxXY = getMaxXY(q);

  siz.x = maxXY.x-minXY.x;
  siz.y = maxXY.y-minXY.y;
  siz.z = 0.0;

  return siz;
}

/**
 * get the minimum xy coordinates of a bounding box  of a quad
 */
vector getMinXY(quadri q) {
  vector minXY;
  minXY.z = 0.0;

  minXY.x = q.v1.x;
  minXY.y = q.v1.y;

  minXY.x = q.v2.x < minXY.x ? q.v2.x : minXY.x;
  minXY.y = q.v2.y < minXY.y ? q.v2.y : minXY.y;

  minXY.x = q.v3.x < minXY.x ? q.v3.x : minXY.x;
  minXY.y = q.v3.y < minXY.y ? q.v3.y : minXY.y;

  minXY.x = q.v4.x < minXY.x ? q.v4.x : minXY.x;
  minXY.y = q.v4.y < minXY.y ? q.v4.y : minXY.y;

  return minXY;
}

/**
 * get the maximum xy coordinates of a bounding box of a quad
 */
vector getMaxXY(quadri q) {
  vector maxXY;
  maxXY.z = 0.0;

  maxXY.x = q.v1.x;
  maxXY.y = q.v1.y;

  maxXY.x = q.v2.x > maxXY.x ? q.v2.x : maxXY.x;
  maxXY.y = q.v2.y > maxXY.y ? q.v2.y : maxXY.y;

  maxXY.x = q.v3.x > maxXY.x ? q.v3.x : maxXY.x;
  maxXY.y = q.v3.y > maxXY.y ? q.v3.y : maxXY.y;

  maxXY.x = q.v4.x > maxXY.x ? q.v4.x : maxXY.x;
  maxXY.y = q.v4.y > maxXY.y ? q.v4.y : maxXY.y;

  return maxXY;
}

/**
 * translates a point
 */
void doTranslateAction(double *x, double *y, vector transAction) {
  *x += transAction.x;
  *y += transAction.y;
}

/**
 * rotates of a point around a specified center by a specified angle
 */
void doRotateAction(double *x, double *y, vector rotCenter, double angle) {
  double theta = angle*PI/180.0; /* rotation angle in radians */
  double diffX, diffY;

  vector rc = rotCenter;

  /* multiply the coordinates by a rotation matrix */
  diffX = *x-rc.x;
  diffY = *y-rc.y;
  *x = rc.x + cos(theta)*diffX - sin(theta)*diffY;
  *y = rc.y + sin(theta)*diffX + cos(theta)*diffY;
}

/**
 * scales a point
 */
void doScaleAction(double *x, double *y, vector scaleAction) {
  *x = scaleAction.x*(*x);
  *y = scaleAction.y*(*y);
}

/**
 * gets a scaling of a quad
 */
quadri getQuadScale(quadri base, vector scaleAction) {
  vector c; /* center of quad */
  quadri s; /* new quad */
  double imW = length(base.v4,base.v3);
  double imH = length(base.v4,base.v1);
  double sx = 1 + scaleAction.x/imW;
  double sy = 1 + scaleAction.y/imH;

  /* get the params */
  c = getMidPoint(base);

  /* new vertices are scaled from the center of the current loaction */
  s.v1.x = c.x + sx*(base.v1.x-c.x);
  s.v1.y = c.y + sy*(base.v1.y-c.y);
  s.v2.x = c.x + sx*(base.v2.x-c.x);
  s.v2.y = c.y + sy*(base.v2.y-c.y);
  s.v3.x = c.x + sx*(base.v3.x-c.x);
  s.v3.y = c.y + sy*(base.v3.y-c.y);
  s.v4.x = c.x + sx*(base.v4.x-c.x);
  s.v4.y = c.y + sy*(base.v4.y-c.y);

  s.v1.z = s.v2.z = s.v3.z = s.v4.z = 0.0;

  return s;
}

/**
 * gets a translation of a quad
 */
quadri getQuadTranslation(quadri base, vector transAction) {
  quadri t;

  /* just add an offset to each vertex */
  t.v1.x = base.v1.x+transAction.x;
  t.v1.y = base.v1.y+transAction.y;
  t.v2.x = base.v2.x+transAction.x;
  t.v2.y = base.v2.y+transAction.y;
  t.v3.x = base.v3.x+transAction.x;
  t.v3.y = base.v3.y+transAction.y;
  t.v4.x = base.v4.x+transAction.x;
  t.v4.y = base.v4.y+transAction.y;

  t.v1.z = t.v2.z = t.v3.z = t.v4.z = 0.0;
  return t;
}

/**
 * gets a rotation of a quad around a specified center by a specified angle
 */
quadri getQuadRotation(quadri base, vector rotCenter, double angle) {
  quadri r;
  double theta = angle*PI/180.0; /* rotation angle in radians */
  double diffX, diffY;
  vector rc = rotCenter;

  /* multiply the coordinates by a rotation matrix */
  diffX = base.v1.x-rc.x;
  diffY = base.v1.y-rc.y;
  r.v1.x = rc.x + cos(theta)*diffX - sin(theta)*diffY;
  r.v1.y = rc.y + sin(theta)*diffX + cos(theta)*diffY;

  diffX = base.v2.x-rc.x;
  diffY = base.v2.y-rc.y;
  r.v2.x = rc.x + cos(theta)*diffX - sin(theta)*diffY;
  r.v2.y = rc.y + sin(theta)*diffX + cos(theta)*diffY;

  diffX = base.v3.x-rc.x;
  diffY = base.v3.y-rc.y;
  r.v3.x = rc.x + cos(theta)*diffX - sin(theta)*diffY;
  r.v3.y = rc.y + sin(theta)*diffX + cos(theta)*diffY;

  diffX = base.v4.x-rc.x;
  diffY = base.v4.y-rc.y;
  r.v4.x = rc.x + cos(theta)*diffX - sin(theta)*diffY;
  r.v4.y = rc.y + sin(theta)*diffX + cos(theta)*diffY;

  r.v1.z = r.v2.z = r.v3.z = r.v4.z = 0.0;

  return r;
}

/**
 * sets the center of rotation
 */
void setRotationCenterV(vector rc) {
  curRotCenter = rc;
}

/**
 * sets the center of rotation
 */
void setRotationCenter2D(double x, double y) {
  curRotCenter.x = x;
  curRotCenter.y = y;
}

/** action application functions */

/**
 * applies a translation to the moving image and adds one to the list.
 * propagates the change through the align actions and tacks of all
 * images after this one to maintain consistency
 */
void applyTranslation(vector transAction) {
  list *postActions, *postContour, *postTackList;
  listNode *i,*j,*k;
  vertex *curTack;

  /* add the action */
  if(!addTranslationAction(actions, transAction)) {
    fprintf(stderr, "error: can't allocate memory to store a translation action\n");
  }
  else { /* successful allocation */
    /* consolodate the action list */
    consolidateActions(actions);

    /* apply the action to the movImg */
    movImgBounds = getQuadTranslation(movImgBounds, transAction);

    /* apply the translation to all future action lists */
    for(i = getListNode(curDataset->sliceActionLists,curSlice+1); i; i = (listNode*) i->next) {
      postActions = (list*) i->data;
      if(!addTranslationAction(postActions, transAction)) {
	fprintf(stderr, "error: can't allocate memory to store a translation action\n");
      }
      else { /* successful allocation, consolodate  */
	consolidateActions(postActions);
      }
    }

    /* apply the translation to all tacks on & after this slice */
    for(i = getListNode(curDataset->sliceContourLists,curSlice); i; i = (listNode*) i->next) {
      postContour = (list*) i->data;
      for(j = getListNode(postContour,0); j; j = (listNode*) j->next) {
	postTackList = ((contour*) j->data)->vertices;
	for(k = getListNode(postTackList,0); k; k = (listNode*) k->next) {
	  curTack = (vertex*) k->data;
	  doTranslateAction(&curTack->x, &curTack->y, transAction);
	}
      }
    }

    /* apply the translation to all markers on & after this slice */
    for(i = getListNode(curDataset->sliceMarkerLists,curSlice); i; i = (listNode*) i->next) {
      postContour = (list*) i->data;
      for(j = getListNode(postContour,0); j; j = (listNode*) j->next) {
	curTack = (vertex*) j->data;
	doTranslateAction(&curTack->x, &curTack->y, transAction);
      }
    }
  }
}

/**
 * applies a rotation to the image and adds one to the list
 * propagates the change through the align actions of all images after
 * this one to maintain consistency
 */
void applyRotation(vector rotCenter, double angle) {
  listNode *i,*j,*k;
  list *postActions, *postContour, *postTackList;
  vertex *curTack;

  /* add the action */
  if(!addRotationAction(actions,rotCenter,angle)) {
    fprintf(stderr, "error: can't allocate memory to store a rotation action\n");
  }
  else {
    /* consolodate the action list */
    consolidateActions(actions);

    /* apply the rotation to the moving image */
    movImgBounds = getQuadRotation(movImgBounds, rotCenter, angle);

    /* apply the rotation to all future action lists */
    for(i = getListNode(curDataset->sliceActionLists,curSlice+1); i; i = (listNode*) i->next) {
      postActions = (list*) i->data;
      if(!addRotationAction(postActions, rotCenter, angle)) {
	fprintf(stderr, "error: can't allocate memory to store a rotation action\n");
      }
      else { /* successful allocation, consolodate  */
	consolidateActions(postActions);
      }
    }

    /* apply the rotation to all tacks on & after this slice */
    for(i = getListNode(curDataset->sliceContourLists,curSlice); i; i = (listNode*) i->next) {
      postContour = (list*) i->data;
      for(j = getListNode(postContour,0); j; j = (listNode*) j->next) {
	postTackList = ((contour*) j->data)->vertices;
	for(k = getListNode(postTackList,0); k; k = (listNode*) k->next) {
	  curTack = (vertex*) k->data;
	  doRotateAction(&curTack->x, &curTack->y, rotCenter, angle);
	}
      }
    }

    /* apply the rotation to all markers on & after this slice */
    for(i = getListNode(curDataset->sliceMarkerLists,curSlice); i; i = (listNode*) i->next) {
      postContour = (list*) i->data;
      for(j = getListNode(postContour,0); j; j = (listNode*) j->next) {
	curTack = (vertex*) j->data;
	doRotateAction(&curTack->x, &curTack->y, rotCenter, angle);
      }
    }
  }
}

/**
 * adds a specified number of pixels to the image in each direction,
 * adds action to the list
 * propagates the change through the align actions of all images after
 * this one to maintain consistency
 */
void applyScale(vector scaleAction) {
  listNode *i,*j,*k;
  list *postActions, *postContour, *postTackList;
  vertex *curTack;

  /* try to add the action to the list */
  if(!addScaleAction(actions,scaleAction)) {
    fprintf(stderr, "error: can't allocate memory to store a scale action\n");
  }
  else {
    /* consolodate the action list */
    consolidateActions(actions);

    /* apply the rotation to the moving image */
    movImgBounds = getQuadScale(movImgBounds, scaleAction);

    /* apply the scale to all future action lists */
    for(i = getListNode(curDataset->sliceActionLists,curSlice+1); i; i = (listNode*) i->next) {
      postActions = (list*) i->data;
      if(!addScaleAction(postActions, scaleAction)) {
	fprintf(stderr, "error: can't allocate memory to store a scale action\n");
      }
      else { /* successful allocation, consolodate  */
	consolidateActions(postActions);
      }
    }

    /* apply the scale to all tacks on & after this slice */
    for(i = getListNode(curDataset->sliceContourLists,curSlice); i; i = (listNode*) i->next) {
      postContour = (list*) i->data;
      for(j = getListNode(postContour,0); j; j = (listNode*) j->next) {
	postTackList = ((contour*) j->data)->vertices;
	for(k = getListNode(postTackList,0); k; k = (listNode*) k->next) {
	  curTack = (vertex*) k->data;
	  doScaleAction(&curTack->x, &curTack->y, scaleAction);
	}
      }
    }

    /* apply the scale to all markers on & after this slice */
    for(i = getListNode(curDataset->sliceMarkerLists,curSlice); i; i = (listNode*) i->next) {
      postContour = (list*) i->data;
      for(j = getListNode(postContour,0); j; j = (listNode*) j->next) {
	curTack = (vertex*) j->data;
	doScaleAction(&curTack->x, &curTack->y, scaleAction);
      }
    }
  }
}

/**
 * apply a set of actions to an image coordinate
 */
quadri applyActions(list *l, quadri base) {
  listNode *i;
  action *a;
  quadri res = base;

  /* apply each action in turn, add them to the current list */
  for(i = getListNode(l,0); i; i = (listNode*) i->next) {
    /* get the action */
    a = (action*) i->data;

    /* apply action and add it to the list */
    switch(a->type) {
    case TRANSLATION:
      res = getQuadTranslation(res, a->trans);
      break;
    case ROTATION:
      res = getQuadRotation(res, a->rotCenter, a->angle);
      break;
    case SCALE:
      res = getQuadScale(res, a->scale);
      break;
    default:
      fprintf(stderr, "error: unrecognized action type %d\n", a->type);
    }
  }

  return res;
}

/** event handlers **/

/**
 * align actions
 */
void alignAction(int action) {
  vector actionVector;

  switch(action) {
  case 'a': /* turn animation on or off */
    if(TRUE == (animate = !animate)) {
      glutTimerFunc(framePeriod, alignTimerEvent, 0);
    }
    else {
      displayImg = 1;
    }
    redisplay();
    break;
  case 'c': /* set the rotation center */
    settingRotCenter = TRUE;
    break;
  case 'y': /* speed up the flickering */
    switchPeriod -= switchPerIncrement;
    if(switchPeriod < 0) switchPeriod = 0;
    break;
  case 't': /* slow down the flickering */
    switchPeriod += switchPerIncrement;
    if(switchPeriod > MAX_SWITCH_PERIOD) switchPeriod = MAX_SWITCH_PERIOD;
    break;
  case 'e': /* toggle previous slice tack display */
    showPrevTacks = !showPrevTacks;
    redisplay();
    break;
  case 'w': /* toggle curent slice tack display */
    showCurTacks = !showCurTacks;
    redisplay();
    break;
  case 'Q':
    displayImg = !displayImg;
    redisplay();
    break;
  case 'p': /* print the action list */
  case 'P':
    fprintf(stdout,"---------------------------------------\n");
    dumpActionList(actions);
    fprintf(stdout,"---------------------------------------\n");
    break;
/*   case 'r': /\* reset the position of the reference image to unmoved *\/ */
/*     resetImage(); */
/*     break; */
/*   case 'u': /\* undo the last action *\/ */
/*   case 'U': */
/*     undoLastAction(); */
/*     break; */
    /* fine direction stuff using numeric keypad */
  case '1': /* down and left */
    actionVector.x = -transIncrX;
    actionVector.y = -transIncrY;
    applyTranslation(actionVector);
    break;
  case '2': /* down */
    actionVector.x = 0;
    actionVector.y = -transIncrY;
    applyTranslation(actionVector);
    break;
  case '3': /* down and right */
    actionVector.x = transIncrX;
    actionVector.y = -transIncrY;
    applyTranslation(actionVector);
    break;
  case '4': /* left */
    actionVector.x = -transIncrX;
    actionVector.y = 0;
    applyTranslation(actionVector);
    break;
  case '6': /* right */
    actionVector.x = transIncrX;
    actionVector.y = 0;
    applyTranslation(actionVector);
    break;
  case '7': /* up and left  */
    actionVector.x = -transIncrX;
    actionVector.y = transIncrY;
    applyTranslation(actionVector);
    break;
  case '8': /* up */
    actionVector.x = 0;
    actionVector.y = transIncrY;
    applyTranslation(actionVector);
    break;
  case '9': /* up and right */
    actionVector.x = transIncrX;
    actionVector.y = transIncrY;
    applyTranslation(actionVector);
    break;
  case '=': /* grow the slice */
  case '+':
    actionVector.x = scaleIncrX;
    actionVector.y = scaleIncrY;
    applyScale(actionVector);
    break;
  case '-': /* shrink the slice */
  case '_':
    actionVector.x = -scaleIncrX;
    actionVector.y = -scaleIncrY;
    applyScale(actionVector);
    break;
  default:
    break;
  }
}

/**
 * menu creation
 */
void createAlignMenu() {
  /* add the menu items */
  glutAddMenuEntry("-- Align Specific Actions ---",0);
  glutAddMenuEntry("'c' set rotation center",'c');
  glutAddMenuEntry("'a' toggle animation",'a');
  glutAddMenuEntry("'w' toggle current contour visibility",'w');
  glutAddMenuEntry("'e' toggle previous contour visibility",'e');
  glutAddMenuEntry("'y' speed up flicker",'[');
  glutAddMenuEntry("'t' slow down flicker",']');
  glutAddMenuEntry("'=' scale the slice up",'=');
  glutAddMenuEntry("'-' scale the slice down",'-');
}

/**
 * keyboard handler
 */
void alignKeyboard(unsigned char key, int x, int y) {
  alignAction(key);
}

/**
 * mouse handler determines what kind of actions are being performed
 */
void alignMouse(int button, int state, vector mousePos) {
  /* set the sensitivity */
  curSensitivity = GLUT_ACTIVE_CTRL == glutGetModifiers()
    ? fineSensitivity : coarseSensitivity;

  /* choose action based on button */
  switch(button) {
  case GLUT_LEFT_BUTTON:
    /* if we are setting the rotation center, set it and break */
    if(settingRotCenter == TRUE) {
      setRotationCenterV(mousePos);
      if(DEBUG) fprintf(stdout,"current rotation center is (%g,%g)\n",
			curRotCenter.x, curRotCenter.y);
      settingRotCenter = FALSE;
      break;
    }

    /* left button: translation */
    switch(state) {
    case GLUT_DOWN:
      /* translation start */
      moving = TRUE;
      break;
    case GLUT_UP:
      /* translation end, apply it */
      moving = FALSE;
      applyTranslation(curTransAction);
      curTransAction.x = curTransAction.y = 0;
      break;
    }
    break;
  case GLUT_MIDDLE_BUTTON: /* rotation */
    switch(state) {
    case GLUT_DOWN:
      /* rotation start */
      rotating = TRUE;
      break;
    case GLUT_UP:
      /* rotation end, apply rotation  */
      rotating = FALSE;

      applyRotation(curRotCenter, curRotAngle);

      /* reset the rot angle */
      curRotAngle = 0;
      break;
    }
    break;
  }

  /* store the last positions for the up */
  if(state == GLUT_DOWN) {
    lastPos = mousePos;
  }

  redisplay();
}

/**
 * mouse motion handler determines the parameters of the action
 */
void alignMouseMotion(vector mousePos) {
  double lastAngle, curAngle;
  vector rc; /* world coordinates of rotation center */

  if(moving) { /* translate by the amount the mouse was moved */
    /* update the current translation */
    curTransAction.x += curSensitivity*(mousePos.x-lastPos.x);
    curTransAction.y += curSensitivity*(mousePos.y-lastPos.y);
  }
  if(rotating) { /* rotate based on mouse motion wrt center of rotation */
    /* get world coords */
    rc = curRotCenter;

    /* get the angles in question */
    lastAngle = atan2(lastPos.y-rc.y, lastPos.x-rc.x);
    curAngle = atan2(mousePos.y-rc.y, mousePos.x-rc.x);

    /* store the current rotation */
    if(fabs(lastAngle-curAngle) < PI) {
      curRotAngle -= 180/PI * curSensitivity*(lastAngle-curAngle);
    }
    else {
      curRotAngle -= 180/PI * curSensitivity*(lastAngle+curAngle);
    }

    curRotAngle = fmod(curRotAngle,360.0);
  }
  lastPos = mousePos;

  redisplay();
}

