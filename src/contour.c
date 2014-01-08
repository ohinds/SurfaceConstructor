/*****************************************************************************
 * contour.c is the source file for the contour tracing portion of an
 * application to construct surfaces from serial secions
 * Oliver Hinds <oph@cns.bu.edu> 2004-03-16
 *
 *
 *
 *****************************************************************************/

#include"contour.h"
#include"colors.extern"

/******************************************************
 * globals
 ******************************************************/

/* the image */
static image *imgBuf;
static quadri imgBounds;
static GLuint tex;

/* list of tacks and markers to make contours and marks */
static int curContour = 0;
static list *tacks;
static list *markers;

/* flags for showing tacks on neighboring slices */
static int showTacks = TRUE;
static int showLeftTacks = FALSE;
static int showRightTacks = FALSE;
static int showLabels = TRUE;

/* flags for dragging */
static vertex *dragTack = NULL;
static vertex *rightNeighbor, *leftNeighbor;
static int curNeighborNum = 0;

/* variables related to automatic tack placement while dragging */
long tackPlacePeriod = 400; /* ms */
static int tackPlaceIncrement = 1;

/* mouse modifier state */
static int controlDown = FALSE;
static int shiftDown = FALSE;
static int altDown = FALSE;

/* flags for connecting contour mode */
static int connectingContours = FALSE;

/* draw sizes for the selected and unselected tacks in pixels */
int tackInnerRadius = 4;
int tackWidth = 6;
//static int tackSelectedWidth = 24;

/* sizes for labels */
int labelInnerRadius = 6;
int labelWidth = 7;

//extern int NUMLABELCOLORS;
//extern int FIRST_LABEL_COLOR;

/* marker color, selected and unselected */
static color tackSelectedColor;
static color markerColor;
static color markerSelectedColor;

/* if we should show the line */
static int lineView = TRUE;

/* mouse anchor position for operations by rectangle */
static vector *mouseAnchorPos = NULL;
static color rectOutlineColor;
static float rectFillColor[4] = {0.0f, 0.0f, 0.0f, 0.2f};

/* echo new tack parms */
int echoNewTackCoords = FALSE;
char newTackCoordsFilename[MAX_STR_LEN] = "";
FILE* NEW_TACK_COORDS_FP = NULL;

/* quadric for disks */
GLUquadricObj *quad = NULL;

/* constant for contour smoothing amount */
static double smoothFactor = 0.001;

/******************************************************
 * functions
 ******************************************************/

/**
 * load the images and perform init on them
 */
void contourImgInit() {
  //char imgFilename[MAX_STR_LEN];
  image *img;

  /* validate the curSlice */
  curSlice %= curDataset->numSlices;
  if(curSlice < 0) curSlice += curDataset->numSlices;

  /* load the texture if dynamic textures is on */
  if(dynamicTextures) {
    img = loadImage(curDataset, curSlice);
    curDataset->slices[curSlice] = *img;
    free(img);
  }

  /* get image */
  imgBuf = &curDataset->slices[curSlice];

  tex = curDataset->sliceTextures[curSlice];
}

/**
 * initialize general properties
 */
void contourInit() {
  static int firstRun = TRUE;

  list *cList;
  listNode *ln;
  list *actions;
  contour *cont;

  /* default color stuff */
  markerColor = COLORS[ORANGE];
  markerSelectedColor = COLORS[GREEN];
  tackSelectedColor = COLORS[WHITE];
  rectOutlineColor = COLORS[BLACK];

  /* zero the contour */
  curContour = 0;

  /* initialize things that only need it the first run */
  if(firstRun == TRUE) {
    /* get a quadric object */
    if(quad == NULL) {
      quad = gluNewQuadric();
      gluQuadricDrawStyle(quad, GLU_FILL);
    }

    firstRun = FALSE;
  }

  /* find the placement of the image (square and centered) */
  imgBounds.v1.x = 0;
  imgBounds.v1.y = 0;
  imgBounds.v2.x = imgBuf->width;
  imgBounds.v2.y = 0;
  imgBounds.v3.x = imgBuf->width;
  imgBounds.v3.y = imgBuf->height;
  imgBounds.v4.x = 0;
  imgBounds.v4.y = imgBuf->height;

  curDataset->width = imgBuf->width;
  curDataset->height = imgBuf->height;

/*   img.v1.x = -(imgBuf->width *pixelSize/resizeMultiplier); */
/*   img.v1.y = -(imgBuf->height*pixelSize/resizeMultiplier); */
/*   img.v2.x =  (imgBuf->width *pixelSize/resizeMultiplier); */
/*   img.v2.y = -(imgBuf->height*pixelSize/resizeMultiplier); */
/*   img.v3.x =  (imgBuf->width *pixelSize/resizeMultiplier); */
/*   img.v3.y =  (imgBuf->height*pixelSize/resizeMultiplier); */
/*   img.v4.x = -(imgBuf->width *pixelSize/resizeMultiplier); */
/*   img.v4.y =  (imgBuf->height*pixelSize/resizeMultiplier); */


  /* apply the actions */

  /* get the action list to apply/modify, create if it doesn't exist */
  if(listSize(curDataset->sliceActionLists) > curSlice
     && NULL !=
     (actions=getListNode(curDataset->sliceActionLists,curSlice)->data)) {
    /* apply the actions */
    imgBounds = applyActions(actions,imgBounds);
  }

/*   /\* get the current tack list to apply/modify, create if it doesn't exist *\/ */
/*   while(NULL == (ln = getListNode(curDataset->sliceTackLists,curSlice))) { */
/*     cList = newList(LIST); */
/*     enqueue(curDataset->sliceTackLists, cList); */
/*   } */
/*   cList = (list*) ln->data; */

/*   if(listSize(cList) == 0) { */
/*     tacks = newList(LIST); */
/*     enqueue(cList,tacks); */
/*   } */
/*   else { */
/*     tacks = (list*) getListNode(cList, 0)->data; */
/*   } */

  /* get the current contour to modify, create if it doesn't exist */
  while(NULL == (ln = getListNode(curDataset->sliceContourLists,curSlice))) {
    cList = newList(LIST);
    enqueue(curDataset->sliceContourLists, cList);
  }
  cList = (list*) ln->data;

  /* if the slice has no contours, create one */
  if(listSize(cList) == 0) {
    cont = createContour();
    enqueue(cList, cont);
    tacks = cont->vertices;
  }
  else {
    tacks = ((contour*) getListNode(cList, 0)->data)->vertices;
  }

  /* get the current marker list to apply/modify, create if it doesn't exist */
  while(NULL == (ln = getListNode(curDataset->sliceMarkerLists,curSlice))) {
    markers = newList(LIST);
    enqueue(curDataset->sliceMarkerLists, markers);
  }
  markers = (list*) ln->data;

  strcpy(alertString,"");
}

/**
 * uninitializes the contour drawing and imaging stuff
 */
void contourUninit() {
  /* unload the texture if dynamic textures are on */
  if(dynamicTextures) {
    unloadTexture(curSlice);
  }
}

/**
 * cleans up
 */
void contourDestroy() {
  /* dispose of the quadric object */
  gluDeleteQuadric(quad);
  quad = NULL;

  if(echoNewTackCoords) {
    fclose(NEW_TACK_COORDS_FP);
  }
}

/**
 * check to see if a repaint is needed
 */
int contourRepaintNeeded(long now) {
  return FALSE;
}

/**
 * do contour specific drawing
 */
void contourDraw() {
  listNode *i,*j;
  int ind;
  list *l,*cList, *adjList = NULL;
  contour *cont;
  vertex *t, *prev;
  vector rectCorner1,rectCorner2;
  float maxTexX = (float) imgBuf->width, maxTexY = (float) imgBuf->height;
  GLenum textureMethod = getTextureMethod();
  quadri windowCoords;

  windowCoords = getWindowCoordsQ(imgBounds);  

  if(curDataset->imageSource != VP_NOIMAGES) {
    /* turn on texture mapping */
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glEnable(textureMethod);

    /* bind the texture */
    glBindTexture(textureMethod, tex);

    /* if we have no extensions, calculate the extent of the texture in (0,1) */
    if(textureMethod == GL_TEXTURE_2D) {
      maxTexX = imgBuf->width/(float)(imgBuf->width+imgBuf->padX);
      maxTexY = imgBuf->height/(float)(imgBuf->height+imgBuf->padY);
    }

    /* make a quadrilateral and provide texture coords */
    glBegin(GL_QUADS); {
      glTexCoord2d(0.0,0.0);
      glVertex3d(windowCoords.v1.x, windowCoords.v1.y, 0.0);
      glTexCoord2d(maxTexX,0.0);
      glVertex3d(windowCoords.v2.x, windowCoords.v2.y, 0.0);
      glTexCoord2d(maxTexX,maxTexY);
      glVertex3d(windowCoords.v3.x, windowCoords.v3.y, 0.0);
      glTexCoord2d(0.0,maxTexY);
      glVertex3d(windowCoords.v4.x, windowCoords.v4.y, 0.0);
    } glEnd();
    glDisable(textureMethod);
  }

  /* if we are connecting the contour, store the adjacency list */
  if(connectingContours) {
    adjList = ((contour*) getListNode((list*) getListNode(curDataset->sliceContourLists,curSlice)->data,curContour)->data)->adjacentContours;
  }

  /* draw contours and markers from adjacent slices, if neccessary */

  /* left slice contours and markers */
  if(curSlice > 0 && listSize(curDataset->sliceContourLists) > curSlice-1
     && showLeftTacks) {
    glColor3fv(COLORS[GRAY]);

    /* iterate over contours, drawing each */
    l = (list*)getListNode(curDataset->sliceContourLists,curSlice-1)->data;
    for(i = getListNode(l,0); i; i = (listNode*) i->next) {
      /* get the contour at index */
      cont = (contour*) i->data;
      cList = cont->vertices;

      /* draw the contour lines, if we need to */
      if(lineView) {
	glPointSize(pixelSize*tackWidth);
	glBegin(GL_LINES); {
	  /* iterate over tacks, drawing lines between each */
	  prev = listSize(cList) > 1 ? (vertex*) getListNode(cList,0)->data : NULL;
	  for(j = getListNode(cList,1); prev && j; j = (listNode*) j->next) {
	    t = (vertex*) j->data;
	    drawLine(prev,t);
	    prev = t;
	  }
	  /* if this is a closed contour with more than 2 tacks,
	   * draw a line from last to first */
	  if(j != getListNode(cList,1) && cont->closed == CLOSED) {
	    drawLine(prev,(vertex*) getListNode(cList,0)->data);
	  }
	} glEnd();
      }

      /* draw the tacks in the contour */
      for(j = getListNode(cList,0); j; j = (listNode*) j->next) {
	t = (vertex*) j->data;
	/* draw the tack */
	drawTack(t);
      }
    }

    /* left slice markers, unless connecting contours */
    if(!connectingContours) {
      glColor3fv(COLORS[GRAYED_LIGHT_ORANGE]);
      l = (list*)getListNode(curDataset->sliceMarkerLists,curSlice-1)->data;
      for(i = getListNode(l,0); i; i = (listNode*) i->next) {
	t = (vertex*) i->data;
	/* draw the tack */
	drawTack(t);
      }
    }
  }

  /* right slice contours and markers */
  if(curSlice < curDataset->numSlices-1 &&
     listSize(curDataset->sliceContourLists) > curSlice+1 &&
     (showRightTacks || connectingContours)) {

    /* iterate over contours, drawing each */
    l = (list*)getListNode(curDataset->sliceContourLists,curSlice+1)->data;
    for(i = getListNode(l,0), ind = 0; i; i = (listNode*) i->next, ind++) {
      glColor3fv(COLORS[DARK_GRAY]);

      /* get the contour at index */
      cont = (contour*) i->data;
      cList = cont->vertices;

      /* draw the contour lines, if we need to */
      if(lineView) {
	glPointSize(pixelSize*tackWidth);
	glBegin(GL_LINES); {
	  /* iterate over tacks, drawing lines between each */
	  prev = listSize(cList) > 1 ? (vertex*) getListNode(cList,0)->data : NULL;
	  for(j = getListNode(cList,1); prev && j; j = (listNode*) j->next) {
	    t = (vertex*) j->data;
	    drawLine(prev,t);
	    prev = t;
	  }
	  /* if this is a closed contour with more than 2 tacks,
	   * draw a line from last to first */
	  if(j != getListNode(cList,1) && cont->closed == CLOSED) {
	    drawLine(prev,(vertex*) getListNode(cList,0)->data);
	  }
	} glEnd();
      }

      /* set the color if this is an adjacent contour */
      if(listContains(adjList,cont)) {
	glColor3fv(getTackColor(ind));
      }

      /* draw the tacks in the contour */
      for(j = getListNode(cList,0); j; j = (listNode*) j->next) {
	t = (vertex*) j->data;
	/* draw the tack */
	drawTack(t);
      }
    }

    /* right slice markers */
    if(!connectingContours) {
      glColor3fv(COLORS[GRAYED_DARK_ORANGE]);
      l = (list*)getListNode(curDataset->sliceMarkerLists,curSlice+1)->data;
      for(i = getListNode(l,0); i; i = (listNode*) i->next) {
	t = (vertex*) i->data;
	/* draw the tack */
	drawTack(t);
      }
    }
  }

  /* current slice contours */
  if(showTacks) {
    /* iterate over contours, drawing each */
    l = (list*)getListNode(curDataset->sliceContourLists,curSlice)->data;
    for(i = getListNode(l,0), ind = 0; i; i = (listNode*) i->next, ind++) {
      /* get the contour at index */
      cont = (contour*)i->data;
      cList = cont->vertices;

      /* draw the contour lines, if we need to */
      if(lineView) {
	glPointSize(pixelSize*tackWidth);
	glBegin(GL_LINES); {
	  glColor3fv(curContour == ind ? getLineColor(ind)
		     : getGrayedLineColor(ind));

	  /* iterate over tacks, drawing lines between each */
	  prev = listSize(cList) > 1 ? (vertex*) getListNode(cList,0)->data : NULL;
	  for(j = getListNode(cList,1); prev && j; j = (listNode*) j->next) {
	    t = (vertex*) j->data;
	    drawLine(prev,t);
	    prev = t;
	  }
	  /* if this is a closed contour with more than 2 tacks,
	   * draw a line from last to first */
	  if(j != getListNode(cList,1) && cont->closed == CLOSED) {
	    drawLine(prev,(vertex*) getListNode(cList,0)->data);
	  }
	} glEnd();
      }

      /* draw the tacks in the contour */
      for(j = getListNode(cList,0); j; j = (listNode*) j->next) {
	t = (vertex*) j->data;

	/* draw the tack */
	if(t->selected) {
	  glColor3fv(tackSelectedColor);
	}
	else {
	  glColor3fv(curContour == ind ? getTackColor(ind)
		     : getGrayedTackColor(ind));
	}
	drawTack(t);

	if(showLabels && t->label != -1) {
	  glColor3fv(getLabelColor(t->label));
	  drawLabel(t);	  
	}
      }
    }
  }

  /* draw the labels */
/*   if(showLabels) { */
/*     l = (list*)getListNode(curDataset->sliceContourLists,curSlice)->data; */
/*     for(i = getListNode(curDataset->sliceLabels,0); i; i = (listNode*) i->next) { */
/*       lab = (label*) i->data; */
      
/*       /\* draw the label if it is on this slice *\/ */
/*       if(lab->sliceLink == l) { /\* choose color based on selection state *\/ */
/* 	glColor3fv(getLabelColor(lab->lab)); */
/*       } */
/*     } */
/*   } */

  /* draw the markers */
  for(i = getListNode(markers,0); i; i = (listNode*) i->next) {
    t = (vertex*) i->data;

    /* draw the marker */
    if(t->selected) { /* choose color based on selection state */
      glColor3fv(markerSelectedColor);
    }
    else {
      glColor3fv(markerColor);
    }
    drawTack(t);
  }

  /* draw the mouse rectangle, if we should */
  if(mouseAnchorPos != NULL) {
    rectCorner1 = getWindowCoordsV(*mouseAnchorPos);
    rectCorner2 = getWindowCoordsV(lastPos);

    glColor3fv(rectOutlineColor);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glRectf(rectCorner1.x,rectCorner1.y,rectCorner2.x,rectCorner2.y);

    glColor4fv(rectFillColor); 
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);     
    glRectf(rectCorner1.x,rectCorner1.y,rectCorner2.x,rectCorner2.y);
  }

  /* build the mode string */
  sprintf(modeString,"slice %d ",curSlice);
  strcat(modeString,"trace");
  
  if(connectingContours) {
    strcat(modeString," (connecting contours)");   
  }

  strcat(modeString,":");

}

/**
 * draw a tack
 */
void drawTack(vertex *t) {
  vertex v = getWindowCoordsVert(*t);

  /* draw an annulus */
  glTranslated(v.x, v.y,0);
  gluDisk(quad,tackInnerRadius,tackWidth,20,1);
  glTranslated(-v.x, -v.y,0);

  /* make a tiny point */
  glPointSize(1);
  glBegin(GL_POINTS); {
    glVertex3d(v.x, v.y, 0);
  } glEnd();
}

/**
 * draw a label
 */
void drawLabel(vertex *t) {
  vertex v = getWindowCoordsVert(*t);

  /* draw an annulus */
  glTranslated(v.x, v.y,0);
  gluDisk(quad,labelInnerRadius,labelWidth,20,1);
  glTranslated(-v.x, -v.y,0);
}

/**
 * draw a line between two tacks
 */
void drawLine(vertex *t1, vertex *t2) {
  vertex v1,v2,v3,v4;
  double len;

  v1 = getWindowCoordsVert(*t1);
  v2 = getWindowCoordsVert(*t2);

  /* get the coordinates of the intersection of the circles
     and the line*/
  len = tackWidth/(length(v1,v2));
  v3.x = v1.x + len*(v2.x-v1.x);
  v3.y = v1.y + len*(v2.y-v1.y);
  v4.x = v2.x + len*(v1.x-v2.x);
  v4.y = v2.y + len*(v1.y-v2.y);

  /* draw the line up to the tack circles */
  glVertex3d(v3.x, v3.y, 0);
  glVertex3d(v4.x, v4.y, 0);
}

/** geom funcs **/

/** event handlers **/

/**
 * keyboard handler
 */
void contourAction(int action) {
  contour *cont;
  int ret;

  switch(action) {
  case 'b': /* move back one contour */
    decreaseContour();
    break;
  case 'c': /* toggle the closure state of the contour */
    toggleClosureState(curContour);
    redisplay();
    break;
  case 'e': /* toggle left tack display */
    showLeftTacks = !showLeftTacks;
    redisplay();
    break;
  case 'f': /* decrease tacking speed */
    tackPlacePeriod += tackPlaceIncrement;
    break;
  case 'g': /* increase tacking speed */
    tackPlacePeriod -= tackPlaceIncrement;
    break;
  case 'k': /* break contour */
    breakContour();
    break;
  case 'J': /* join contours */
    joinTwoContours();
    break;
  case 'l': /* attach a label */
    setLabelSelected(1);
    redisplay();
    break;
  case 'L': /* detach label */
    setLabelSelected(-1);
    redisplay();
    break;
  case 'E': /* line view toggle */
    lineView = !lineView;
    redisplay();
    break;
  case 'n': /* create a new contour */
  case 'N':
    cont = createContour();
    tacks = cont->vertices;
    enqueue((list*)getListNode(curDataset->sliceContourLists,curSlice)->data,
	    cont);
    curContour = listSize((list*)getListNode(curDataset->sliceContourLists,curSlice)->data) - 1;
    redisplay();
    break;
  case 'p': /* print the tack list */
    fprintf(stdout,"---------------------------------------\n");
    dumpTackList(tacks);
    fprintf(stdout,"---------------------------------------\n");
    break;
  case 't': /* toggle contour connection mode */
    connectingContours = !connectingContours;
    redisplay();
    break;
  case 'j': /* fill selected */
    ret = selectVerticesBetween(tacks);
    if(DEBUG && ret == SR_FAILURE) {    
      fprintf(stderr,"selection fill failed\n");
    }
    redisplay();
    break;
  case 'u': /* unselect */
    unselectAllTacksContour(tacks);
    redisplay();
    break;
  case 'U':
    unselectAllTacksSlice((list*)getListNode(curDataset->sliceContourLists,curSlice)->data);
    redisplay();
    break;
  case 'a':
    selectAllTacksSlice((list*)getListNode(curDataset->sliceContourLists,curSlice)->data);
    redisplay();
    break;
  case 'v': /* move up one contour */
    increaseContour();
    break;
  case 'w': /* toggle current slice tack display */
    showTacks = !showTacks;
    redisplay();
    break;
  case 'W': /* toggle current slice label display */
    showLabels = !showLabels;
    redisplay();
    break;
  case 'y': /* smooth the current contour */
    smoothContour(curSlice,curContour,smoothFactor);
    redisplay();
    break;
  case 'Y': /* smooth all contours */
    smoothAllContours(smoothFactor);
    redisplay();
    break;
  case 'P': /* preprocess contours */
    resample = TRUE;
    preprocessSliceContours(curDataset->sliceContourLists);
    redisplay();
    break;
  case 'r': /* toggle right tack display */
    showRightTacks = !showRightTacks;
    redisplay();
    break;
  case 'x': /* reverse the direction of the current contour */
    reverseList(tacks);
    redisplay();
    break;
  default:
    break;
  }
}

/**
 * creates the menu for the contour
 */
void createContourMenu() {
  glutAddMenuEntry("-- Contour Specific Actions --",0);
  glutAddMenuEntry("'n' create new contour",'n');
  glutAddMenuEntry("'E' toggle line view",'L');
  glutAddMenuEntry("'c' toggle contour closure state",'c');
  glutAddMenuEntry("'k' break contour",'k');
  glutAddMenuEntry("'J' join two contours with selected vertices",'J');
  glutAddMenuEntry("'v' increase current contour",'v');
  glutAddMenuEntry("'b' decrease current contour",'b');
  glutAddMenuEntry("'g' increase tacking speed",'g');
  glutAddMenuEntry("'f' decrease tacking speed",'f');
  glutAddMenuEntry("'w' toggle contour visibility",'w');
  glutAddMenuEntry("'W' toggle label visibility",'W');
  glutAddMenuEntry("'e' toggle left contour visibility",'e');
  glutAddMenuEntry("'r' toggle right contour visibility",'r');
  glutAddMenuEntry("'x' reverse the direction of the current contour",'x');
  glutAddMenuEntry("'t' toggle contour connection mode",'t');
  glutAddMenuEntry("'l' label selected vertices",'l');
  glutAddMenuEntry("'L' unlabel selected vertices",'L');
  glutAddMenuEntry("'u' unselect current contour tacks",'u');
  glutAddMenuEntry("'j' fill selection of current contour tacks",'j');
  glutAddMenuEntry("'a' select current slice tacks",'a');
  glutAddMenuEntry("'U' unselect current slice tacks",'U');
}

/**
 * keyboard handler
 */
void contourKeyboard(unsigned char key, int x, int y) {
  contourAction(key);
}

/**
 * mouse handler determines what kind of actions are being performed
 */
void contourMouse(int button, int state, vector mousePos) {
  int r, l, selectedCount;
  vertex *t, *lastT = NULL;
  list *tackClone, *markerClone;
  int mod;
  
  /* assign a modifier so we can see them in other funcitons */
  if(state == GLUT_DOWN) {
    mod = glutGetModifiers();

    switch(glutGetModifiers()) {
    case GLUT_ACTIVE_CTRL:
      controlDown = TRUE;
      break;
    case GLUT_ACTIVE_SHIFT:
      shiftDown = TRUE;
      break;
    case GLUT_ACTIVE_ALT:
    case GLUT_ACTIVE_SHIFT | GLUT_ACTIVE_CTRL:
      altDown = TRUE;
      break;
    }

  }
  else {
    controlDown = shiftDown = altDown = FALSE;
  }

  /* choose action based on button */
  switch(button) {
  case GLUT_LEFT_BUTTON:
    /* left button: lay or delete a tack or select one to move */
    switch(state) {
    case GLUT_DOWN:
      /* mouse down means different things based on the selected
	 state, whether control/shift is down, whether certains modes
	 are enabled etc..., so lotsa conditionals based on these
	 things */

      /* test to see if we specifying contour connectivity across slices*/
      if(connectingContours) {
	connectContours(curSlice,curContour,&mousePos);
	break;
      }
      else if(controlDown) { /* store the location for possible rectangle select */
	mouseAnchorPos = (vector*) malloc(sizeof(vector));
	*mouseAnchorPos = mousePos;
	break;
      }

      /* test for clicking on an existing tack */
      if(NULL != (t = getTackByLoc(tacks,mousePos.x,mousePos.y)) || /* on tack, move or delete */
	 NULL != (t = getTackByLoc(markers,mousePos.x,mousePos.y))) {
	if(controlDown) { /* if ctrl down, handle neighbors */
	  /* assign the current neighbor */
	  if(curNeighborNum == 0) {
	    if(leftNeighbor) { /* unselect any left neighbor that may exist */
	      //leftNeighbor->selected = FALSE;
	      leftNeighbor = NULL;
	    }

	    /* test for in a tack */
	    if(NULL != (leftNeighbor = getTackByLoc(tacks,mousePos.x,mousePos.y))) {
	      /* check for redundant neighbors */
	      if(leftNeighbor == rightNeighbor) {
		leftNeighbor = NULL;
		rightNeighbor->selected = FALSE;
		rightNeighbor = NULL;
	      }
	      else {
		leftNeighbor->selected = TRUE;
		curNeighborNum = !curNeighborNum;
	      }
	    }
	  }
	  else {
	    /* unselect any right neighbor that may exist */
	    if(rightNeighbor) {
	      //rightNeighbor->selected = FALSE;
	      rightNeighbor = NULL;
	    }

	    /* test for in a tack */
	    if(NULL != (rightNeighbor = getTackByLoc(tacks,mousePos.x,mousePos.y))) {
	      /* check for redundant neighbors */
	      if(leftNeighbor == rightNeighbor) {
		rightNeighbor = NULL;
		leftNeighbor->selected = FALSE;
		leftNeighbor = NULL;
	      }
	      else {
		rightNeighbor->selected = TRUE;
		curNeighborNum = !curNeighborNum;
	      }
	    }
	  }
	}
	else { /* no control down, move tack */
	  dragTack = t;
	  dragTack->selected = TRUE;
	}
      }
      else if(!altDown)  { /* add a new tack */
	if(shiftDown && /* add a new marker */
	   NULL != (dragTack = addTack(markers,mousePos.x,mousePos.y,curSlice))) {
	  dragTack->selected = TRUE;
	}
	else if(rightNeighbor && leftNeighbor) { /* insert between tacks */
	  /* get indices of neighbors */
	  r = getIndexOfTack(tacks, rightNeighbor);
	  l = getIndexOfTack(tacks, leftNeighbor);

	  /* insert tack */
	  if(NULL != (dragTack = insertTack(tacks,mousePos.x,mousePos.y, curSlice, min(r,l)))) {
	    if(abs(r-l) > 1) {
	      fprintf(stderr, "warning: the right and left neighbors specified aren't adjacent\ninserting after lower index.\n");
	      break;
	    }
	    dragTack->selected = TRUE;

	    /* unset the right and left neighbors */
	    rightNeighbor->selected = FALSE;
	    rightNeighbor = NULL;
	    leftNeighbor->selected = FALSE;
	    leftNeighbor = NULL;
	    curNeighborNum = 0;
	  }
	} /* insert a new tack at the end */
	else if(NULL != (dragTack = addTack(tacks,mousePos.x,mousePos.y,curSlice))) {
	  if(echoNewTackCoords) {
	    fprintf(NEW_TACK_COORDS_FP,"%0.5f %0.5f %d\n",
		    mousePos.x,mousePos.y,curSlice);
	  }

	  dragTack->selected = TRUE;	  
	}
      }
      break;
    case GLUT_UP:
      /* stop the dragging */
      if(NULL != dragTack) {
	if(dragTack != leftNeighbor && dragTack != rightNeighbor) {
	  dragTack->selected = FALSE;
	}

	dragTack = NULL;
      }
      else { /* select all the tacks in the rectangle */
	if(mouseAnchorPos == NULL) break;

	tackClone = cloneList(tacks);
	markerClone = cloneList(markers);
	selectedCount = 0;

	while(NULL != (t = getTackInRect(tackClone,mouseAnchorPos->x,mouseAnchorPos->y,
					 mousePos.x,mousePos.y)) ||
	      NULL != (t = getTackInRect(markerClone,mouseAnchorPos->x,mouseAnchorPos->y,
					 mousePos.x,mousePos.y))) {
	  /* select  */
	  t->selected = TRUE;
	  lastT = t;
	  selectedCount++;

	  /* delete based on list */
	  if(-1 != (r = getIndexOfTack(tackClone, t))) {
	    removeListNode(tackClone, r);
	  }
	  else if(-1 != (r = getIndexOfTack(markerClone, t))) {
	    removeListNode(markerClone, r);
	  }
	}
	
	/* store the last two selected vertices */
	if(selectedCount == 1) {	  
	  rightNeighbor = leftNeighbor;
	  leftNeighbor = lastT;
	}

	free(mouseAnchorPos);
	freeList(tackClone);
	freeList(markerClone);
	mouseAnchorPos = NULL;
      }
      break;
    }
    break;
  case GLUT_MIDDLE_BUTTON: /* delete */
    if(state == GLUT_DOWN) {
      /* test to see if we are specifying contour connectivity across slices*/
      if(connectingContours) {
	disconnectContours(curSlice,curContour,&mousePos);
	break;
      }
      else { /* store the location for possible rectangle delete */
	mouseAnchorPos = (vector*) malloc(sizeof(vector));
	*mouseAnchorPos = mousePos;
      }
    }
    else if(state == GLUT_UP) {
      if(mouseAnchorPos == NULL) break;

      while(NULL != (t = getTackInRect(tacks,mouseAnchorPos->x,mouseAnchorPos->y,
				       mousePos.x,mousePos.y)) ||
	    NULL != (t = getTackInRect(markers,mouseAnchorPos->x,mouseAnchorPos->y,
				       mousePos.x,mousePos.y))) {
	/* delete based on list */
	if(-1 != (r = getIndexOfTack(tacks, t))) {
	  removeListNode(tacks, r);
	}
	else if(-1 != (r = getIndexOfTack(markers, t))) {
	  removeListNode(markers, r);
	}
      }
	
      free(mouseAnchorPos);
      mouseAnchorPos = NULL;
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
void contourMouseMotion(vector mousePos) {
  static long nextTack = 0;
  int needRedraw = FALSE;

  /* we need to redraw if the rectangle is being drawn */
  if(mouseAnchorPos != NULL) {
    needRedraw = TRUE;
  }

  /* check if the mouse dragging should place tacks by timing */
  if(altDown
     && glutGet(GLUT_ELAPSED_TIME) >= nextTack
     && dragTack == getTackByLoc(tacks,mousePos.x,mousePos.y)) {
    /* unselect the dragged tack, if there is one */
    if(dragTack != NULL) {
      dragTack->selected = FALSE;
    }

    /* add a new tack */
    if(NULL != (dragTack = addTack(tacks,mousePos.x,mousePos.y,curSlice))) {
      dragTack->selected = TRUE;
    }
    nextTack = glutGet(GLUT_ELAPSED_TIME) + tackPlacePeriod;
  }
  /* update the position of the dragged tack */
  else if(NULL != dragTack) {
    dragTack->x = mousePos.x;
    dragTack->y = mousePos.y;
    needRedraw = TRUE;
  }

  /* repaint if told to the rectangle thing is happening */
  if(needRedraw) {
    redisplay();
  }

  lastPos = mousePos;
}

/** util **/

/**
 * unselect all tacks in a contour
 */
void unselectAllTacksContour(list *tacks) {
  listNode *ln;

  /* validate */
  if(tacks == NULL) return;

  for(ln = getListNode(tacks,0); ln; ln = (listNode*) ln->next) {
    ((vertex*) ln->data)->selected = FALSE;
  }
}
/**
 * select all tacks in a contour
 */
void selectAllTacksContour(list *tacks) {
  listNode *ln;

  /* validate */
  if(tacks == NULL) return;

  for(ln = getListNode(tacks,0); ln; ln = (listNode*) ln->next) {
    ((vertex*) ln->data)->selected = TRUE;
  }
}

/**
 * unselect all tacks in a slice
 */
void unselectAllTacksSlice(list *contours) {
  listNode *ln;

  /* validate */
  if(contours == NULL) return;

  for(ln = getListNode(contours,0); ln; ln = (listNode*) ln->next) {
    unselectAllTacksContour(((contour*) ln->data)->vertices);
  }  
}

/**
 * select all tacks in a slice
 */
void selectAllTacksSlice(list *contours) {
  listNode *ln;

  /* validate */
  if(contours == NULL) return;

  for(ln = getListNode(contours,0); ln; ln = (listNode*) ln->next) {
    selectAllTacksContour(((contour*) ln->data)->vertices);
  }  
}

/**
 * unselect all tacks in the dataset
 */
void unselectAllTacksDataset(dataset *ds) {
  listNode *ln;

  /* validate */
  if(ds == NULL) return;

  for(ln = getListNode(ds->sliceContourLists,0); ln; ln = (listNode*) ln->next) {
    unselectAllTacksSlice((list*) ln->data);
  }  
}

/**
 * select all tacks in the dataset
 */
void selectAllTacksDataset(dataset *ds) {
  listNode *ln;

  /* validate */
  if(ds == NULL) return;

  for(ln = getListNode(ds->sliceContourLists,0); ln; ln = (listNode*) ln->next) {
    selectAllTacksSlice((list*) ln->data);
  }  
}

/**
 * validate contours by removing empty contours and finding intersecting ones
 * THIS IS POSSIBLY BUGGY DUE TO THE SIMULTANEOUS DELETION AND REFERENCE OF 
 * CONTOURS
 */
int validateContours(dataset *ds) {
  int sliceInd, contInd, intersectingFound = 0, localIntersecting;
  list *slice;
  listNode *sliceNode, *contNode;
  contour *c;

  /* iterate over slices, checking each */
  for(sliceNode = getListNode(ds->sliceContourLists,0), sliceInd = 0;sliceNode;
      sliceNode = (listNode*) sliceNode->next, sliceInd++) {
    slice = (list*) sliceNode->data;

    if(sliceInd == curSlice) {
      continue;
    }

    /* iterate over contours, checking each */
    for(contNode = getListNode(slice,0), contInd = 0; contNode;
	contNode = (listNode*) contNode->next, contInd++) {
      
      c = (contour*) contNode->data;

      /* remove if empty */
      if(listSize(c->vertices) < 1) {
	removeContour(ds,slice,c);
      }
    }

    localIntersecting = selectIntersecting(slice);

    intersectingFound = intersectingFound || localIntersecting;

    if(DEBUG && localIntersecting) {
      fprintf(stdout,"found contour intersection on slice %d\n", sliceInd);
    }      
  }

  return intersectingFound == TRUE ? FAILURE : SUCCESS;
}

/** 
 * select the tacks at endpoints of segments that intersect 
 */
int selectIntersecting(list *contours) {
  int foundIntersecting = 0;
  int localIntersecting = 0;
  contour *thisCont, *otherCont;
  listNode *thisNode, *otherNode;

  /* iterate over each segment, testing for intersection with the others */
  for(thisNode = getListNode(contours,0); thisNode; thisNode = (listNode*) thisNode->next) {
    thisCont = (contour*) thisNode->data;

    /* test this contour against all others, up to and including itself */
    for(otherNode = getListNode(contours,0); otherNode; otherNode = (listNode*) otherNode->next) {
      otherCont = (contour*) otherNode->data;
      localIntersecting = selectContourIntersections(thisCont, otherCont);
      foundIntersecting = foundIntersecting || localIntersecting;

      /* move to the next, if we've just checked ourself */
      if(otherNode == thisNode) break;
    }
  }
  
  return foundIntersecting;
}

/**
 * select contour intersection points 
 */
int selectContourIntersections(contour *c1, contour *c2) {
  int foundIntersecting = 0;
  listNode *ln1, *ln2;
  vertex *v11, *v12, *v21, *v22;

  /* test ech segment for intersection */
  for(ln1 = getListNode(c1->vertices,0); ln1; ln1= (listNode*) ln1->next) {
    if(ln1->next == NULL) break;

    /* vertices for this segment */
    v11 = (vertex*) ln1->data;
    v12 = (vertex*) ((listNode*)ln1->next)->data;

    for(ln2 = getListNode(c2->vertices,0); ln2; ln2 = (listNode*) ln2->next) {
      if(ln2->next == NULL) break;

      /* vertices for this segment */
      v21 = (vertex*) ln2->data;
      v22 = (vertex*) ((listNode*)ln2->next)->data;

      /* if they are the same vertex, dont test */
      if(v21 == v11 || v22 == v11 || v22 == v12 || v21 == v12) continue;

      

      /* test the segments */      
      if(segmentsIntersect2D(v11,v12,v21,v22) == TRUE) {
	v11->selected = v12->selected = v21->selected = v22->selected = TRUE;
	foundIntersecting = TRUE;
      }
    }
  }
  
  return foundIntersecting;
}

/**
 * tests for two dimensional segment intersection 
 */
int segmentsIntersect2D(vertex *v11, vertex *v12, vertex *v21, vertex *v22) {
  return segmentsIntersect(v11->x, v11->y, 0.0f,
			   v12->x, v12->y, 0.0f,
			   v21->x, v21->y, 0.0f,
			   v22->x, v22->y, 0.0f); 
}



/**
 * smooth all contours
 */
void smoothAllContours(double factor) {
  int slice, contour, numContours;

  /* iterate over slices and contours, smoothing each */
  for(slice = 0; slice < curDataset->numSlices; slice++) {
    numContours = listSize((list*) getListNode(curDataset->sliceContourLists,
					       slice)->data);

    for(contour = 0; contour < numContours; contour++) {
      smoothContour(slice, contour, factor);
    }
  }
}

/**
 * smooth the contour across slices
 */
void smoothContour(int sliceIndex, int contourIndex, double factor) {
  smoothContourGauss(sliceIndex, contourIndex, factor);
}

/**
 * smooth the contour across slices using a gaussian kernel
 */
void smoothContourGauss(int sliceIndex, int contourIndex, double factor) {
  int tackInd, sliceInd, contInd, contTackInd;
  double newX, newY, weight;
  list *slice = (list*) getListNode(curDataset->sliceContourLists,
				    sliceIndex)->data;
  list *tacks = ((contour*) getListNode(slice,contourIndex)->data)->vertices,
    *contTacks;
  vertex *contT, *t;

  /* smoothing parm */
  const int sliceRange = 2;


  for(tackInd = 0; tackInd < listSize(tacks); tackInd++) {
    /* init */
    newX = newY = 0.0;
    t = (vertex*) getListNode(tacks,tackInd)->data;

    /* iterate over each slice within range, take weighted average
       of tacks in nearby slices */
    for(sliceInd = curSlice-sliceRange; sliceInd < curSlice+sliceRange;
	sliceInd++) {
      /* validate slice range */
      if(sliceInd < 0 ||
	 sliceInd > curDataset->numSlices ||
	 sliceInd == curSlice) {
	continue;
      }

      /* get the slice */
      slice = (list*) getListNode(curDataset->sliceContourLists,
				  sliceInd)->data;

      /* weighted average of tacks in the slice */
      for(contInd = 0;
	  contInd < listSize(slice);
	  contInd++) {
	contTacks = ((contour*)
		     getListNode(slice,contInd)->data)->vertices;
	for(contTackInd = 0; contTackInd < listSize(contTacks);
	    contTackInd++) {
	  contT = (vertex*) getListNode(contTacks,contTackInd)->data;

	  /* get the weight */
	  weight = gaussian(tackDistance(t,contT));

	  /* update the coordinates */
	  newX += weight * (contT->x - t->x);
	  newY += weight * (contT->y - t->y);;
	}
      }
    }

    /* update the tack location */
    t->x += factor*newX;
    t->y += factor*newY;
  }
}

/**
 * remove a contour from a dataset
 */
void removeContour(dataset *ds, list* slice, contour* cont) {
  int sl,adj;
  list *adjacentList;
  listNode *ln, *aln;
  contour *c;

  /* validate */
  if(slice == NULL || cont == NULL) return;

  /* get the indices of the slice and contour */
  sl = findInListI(ds->sliceContourLists,slice);
  if(sl < 0) return;

  /* validate for adjacent slice correction */
  if(sl > 0) {
    /* delete all refences in the adjacent contours of the previous slice */
    for(ln = getListNode((list*) getListNode(ds->sliceContourLists,sl-1)->data,0); ln; ln = (listNode*) ln->next) {
      adjacentList = ((contour*) ln->data)->adjacentContours;
      for(aln = getListNode(adjacentList,0), adj = 0; aln;
	  aln = (listNode*) aln->next, adj++) {
	c = (contour*) aln->data;

	/* test for this contour */
	if(c == cont) {
	  removeListNode(adjacentList,adj);
	}
      }
    }
  }

  /* remove this contour from the slice contours list */
  removeListNode(slice,findInListI(slice,cont));
}

/**
 * break the contour at selected neighbors
 */
void breakContour() {
  int r,l;
  list *slice;
  contour *newCont, *curCont;

  if(!rightNeighbor || !leftNeighbor) {
    strcpy(alertString,"two neighbors must be selected to break the contour");
    return;
  }

  /* get indices of neighbors */
  r = getIndexOfTack(tacks, rightNeighbor);
  l = getIndexOfTack(tacks, leftNeighbor);

  /* break between tacks */
  if(abs(r-l) > 1) {
    strcpy(alertString, "right and left neighbors specified aren't adjacent\n");
    return;
  }

  slice = (list*) getListNode(curDataset->sliceContourLists,curSlice)->data;
  curCont = (contour*) getListNode(slice,curContour)->data;

  /* perform the split */
  newCont = cloneContour(curCont);
  freeList(newCont->vertices);
  newCont->vertices = splitList(curCont->vertices,max(r,l));

  /* add the new contour */
  enqueue(slice,newCont);

  /* unset the right and left neighbors */
  rightNeighbor->selected = FALSE;
  rightNeighbor = NULL;
  leftNeighbor->selected = FALSE;
  leftNeighbor = NULL;
  curNeighborNum = 0;

  redisplay();
}

/**
 * join the first two contours found with selected vertices
 */
void joinTwoContours() {
  list *slice = (list*) getListNode(curDataset->sliceContourLists,curSlice)->data;
  list *prevSlice;
  contour *tmpCont, *newCont = NULL, *otherCont = NULL;
  listNode *ln, *ln2;

  /* find the newCont */
  for(ln = getListNode(slice,0); ln; ln = (listNode*) ln->next) {
    tmpCont = (contour*) ln->data;

    // find a selected vertex
    if(findFirstSelectedVertex(tmpCont) > -1) {
      newCont = tmpCont;
      break;
    }
  }

  if(newCont == NULL) {
    return;
  }

  /* find the otherCont */
  for(ln = getListNode(slice,0); ln; ln = (listNode*) ln->next) {
    tmpCont = (contour*) ln->data;

    if(tmpCont == newCont) continue;

    // find a selected vertex
    if(findFirstSelectedVertex(tmpCont) > -1) {
      otherCont = tmpCont;
      break;
    }
  }

  if(otherCont == NULL) {
    return;
  }

  /* perform the join */
  appendList(newCont->vertices,otherCont->vertices);
  appendList(newCont->adjacentContours,otherCont->adjacentContours);
  appendList(newCont->adjacentBackwardContours,otherCont->adjacentBackwardContours);

  /* update the adjacency list of the last slice */
  prevSlice = (list*) getListNode(curDataset->sliceContourLists,curSlice-1)->data;
  for(ln = getListNode(prevSlice,0); ln; ln = (listNode*) ln->next) {
    tmpCont = (contour*) ln->data;
    for(ln2 = getListNode(tmpCont->adjacentContours,0); ln2; ln2 = (listNode*) ln2->next) {
      if(ln2->data == otherCont) {
	ln2->data = newCont;
      }
    }
  }

  ln = removeListNode(slice,findInListI(slice,otherCont));
  free((contour*) ln->data);
  free(ln);

  unselectAllTacksSlice(slice);
  redisplay();
}

/**
 * toggles the closure state of a contour on the current slice
 */
void toggleClosureState(int contIndex) {
  contour *cont = (contour*) getListNode((list*) getListNode(curDataset->sliceContourLists,curSlice)->data,contIndex)->data;
  cont->closed = !cont->closed;
}

/**
 * increase the current contour
 */
void increaseContour() {
  list *slice = (list*) getListNode(curDataset->sliceContourLists,curSlice)->data;
  curContour += 1;
  curContour %= listSize(slice);
  tacks = ((contour*) getListNode(slice,curContour)->data)->vertices;

  redisplay();
}

/**
 * decrease the current contour
 */
void decreaseContour() {
  list *slice = (list*) getListNode(curDataset->sliceContourLists,curSlice)->data;
  curContour -= 1;
  if(curContour < 0) {
    curContour += listSize(slice);
  }
  tacks = ((contour*) getListNode(slice,curContour)->data)->vertices;
  redisplay();
}

/**
 * attempt to connect this contour to contours on adjacent slices
 * based on a mouse click position
 */
void connectContours(int sliceInd, int contInd, vector *mousePos) {
  contour *curCont;
  contour *testCont = NULL;
  list *nextSlice;
  listNode *i;

  /* validate the slice indices */
  if(sliceInd < 0 || sliceInd >= listSize(curDataset->sliceContourLists)-1) {
    return;
  }

  /* assign the current contour and next slice */
  curCont = (contour*)
    getListNode((list*) getListNode(curDataset->sliceContourLists,
				    sliceInd)->data,contInd)->data;

  nextSlice = (list*) getListNode(curDataset->sliceContourLists,sliceInd+1)->data;

  /* iterate over the contours in the next slice to test for in tacks */
  for(i = getListNode(nextSlice,0); nextSlice && i; i = (listNode*) i->next) {
    testCont = (contour*) i->data;
    /* test the tacks for contatinment */
    if(NULL != getTackByLoc(testCont->vertices,mousePos->x,mousePos->y)) {
      if(!listContains(curCont->adjacentContours,testCont)) {
	enqueue(curCont->adjacentContours,testCont);
	return;
      }
    }
  }
}

/**
 * attempt to disconnect this contour to contours on adjacent slices
 * based on a mouse click position
 */
void disconnectContours(int sliceInd, int contInd, vector *mousePos) {
  contour *curCont = (contour*) getListNode((list*) getListNode(curDataset->sliceContourLists,sliceInd)->data,contInd)->data;
  contour *testCont = NULL;
  list *nextSlice;
  int ind;
  listNode *i;

  /* validate */
  i = getListNode(curDataset->sliceContourLists,sliceInd+1);
  if(i == NULL) return;

  /* get next slice */
  nextSlice = (list*) i->data;

  /* iterate over the contours in the next slice to test for in tacks */
  for(i = getListNode(nextSlice,0); nextSlice && i; i = (listNode*) i->next) {
    testCont = (contour*) i->data;
    /* test the tacks for contatinment */
    if(NULL != getTackByLoc(testCont->vertices,mousePos->x,mousePos->y)) {
      if(FAILURE != (ind = findInListI(curCont->adjacentContours,testCont))) {
	removeListNode(curCont->adjacentContours,ind);
	return;
      }
    }
  }
}

/**
 * attach the passed label to all the selected vertices
 */
void setLabelSelected(int l) {
  dataset *ds = curDataset;
  listNode *ln, *ln2;
  list *slice = (list*) getListNode(ds->sliceContourLists,curSlice)->data;
  contour *cont = NULL;
  vertex *vert = NULL;
  label *lab;

  for(ln = getListNode(slice,0); ln; ln = (listNode*) ln->next) {
    cont = (contour*) ln->data;
    for(ln2 = getListNode(cont->vertices,0); ln2; ln2 = (listNode*) ln2->next) {
      vert = (vertex*) ln2->data;

      /* if the vertex is selected, label it */
      if(vert->selected) {
	lab = (label*) malloc(sizeof(label));

	/* assign the fields of the label */
	lab->slice = curSlice;
	lab->cont = curContour;
	
	lab->lab = l;

	lab->sliceLink = slice;
	lab->contourLink = cont;
	lab->vertexLink = vert;

	vert->label = lab->lab;

	vert->selected = FALSE;
      }
    }
  }

  rightNeighbor = leftNeighbor = NULL;
}

/**
 * find the first selected vertex on the contour, or -1 if none
 */
int findFirstSelectedVertex(contour *cont) {
  int ind = -1;
  listNode *ln;
  
  if(cont->vertices == NULL) return -1;
  for(ln = getListNode(cont->vertices,0), ind = 0; ln; 
      ln = (listNode*) ln->next, ind++) {
    if(((vertex*)ln->data)->selected) {
      return ind;
    }
  }

  if(ln == NULL) {
    ind = -1;
  }
  return ind;
}

/**
 * gets a contour point color based on a contour id
 */
color getTackColor(int contourNum) {
  return COLORS[contourNum%NUMTACKCOLORS];
}

/**
 * gets a contour point color based on a contour id and grayed
 */
color getGrayedTackColor(int contourNum) {
  return COLORS[contourNum%NUMTACKCOLORS+NUMCOLORS];
}

/**
 * gets a line color based on a contour id
 */
color getLineColor(int contourNum) {
  return COLORS[(contourNum+contourNum/NUMTACKCOLORS)%NUMTACKCOLORS];
}

/**
 * gets a line color based on a contour id and grayed
 */
color getGrayedLineColor(int contourNum) {
  return COLORS[(contourNum+contourNum/NUMTACKCOLORS)%NUMTACKCOLORS+NUMCOLORS];
}

/**
 * gets a label color
 */
color getLabelColor(int labelNum) {
  return COLORS[FIRST_LABEL_COLOR+labelNum%NUMLABELCOLORS];
}

/* evalautes a gaussian density with zero mean and unit sd */
double gaussian(double x) {
  double sd = 7;
  return 1/(sd*sqrt(2*PI)) * exp(-x*x/(2*sd*sd));
}
