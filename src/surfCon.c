/******************************************************************************
 * surfCon.c is the source file for an application to create
 * surfaces from serial sections
 * Oliver Hinds <oph@cns.bu.edu> 2004-02-04
 *
 *
 *
 *****************************************************************************/

#include"surfCon.h"
#include"colors.h"

#ifdef MEMLEAK
#include<mcheck.h>
#endif

#define SURF_RECON_VERSION_C "$Id: surfCon.c,v 1.36 2007/05/22 19:18:00 oph Exp $"

/******************************************************
 * constants and types
 ******************************************************/

/* mode of operation */
enum MODE {BLANK, ALIGN, TACK};//, LACE};
int mode = BLANK;

/******************************************************
 * globals
 ******************************************************/

/* dataset info */
dataset *curDataset;
int datasetModified = FALSE;

char datasetFilename[MAX_STR_LEN] = "";
char sliceContourFilename[MAX_STR_LEN] = "";
char volumeFilename[MAX_STR_LEN] = "";
char slicefileFilename[MAX_STR_LEN] = "";
char sliceposFilename[MAX_STR_LEN] = "";
char markerFilename[MAX_STR_LEN] = "markers.txt";
char dataDir[MAX_STR_LEN];
char dataBasename[MAX_STR_LEN];
char **dataSliceNames;
enum INPUTTYPE datasetInputType = VP_IMAGE;
enum IMAGEFORMAT datasetImageFormat = JPEG;
int dataNumSlices;
double dataSliceDist;
vector metersPerPixel;
int createDataset = FALSE;
int curSlice = 0;

/* adjustment incremements */
float maxBrightness = 1000.0f;
float minBrightness = -1000.0f;
float brightnessIncr = 0.005;

float maxContrast = 100.0f;
float minContrast = 0.10f;
float contrastIncr = 0.1;

/* multiplier in case images are larger than texture size, need ratio */
float resizeMultiplier = 1.0f;

/* information strings */
char modeString[MAX_STR_LEN] = "";
char coordString[MAX_STR_LEN];
char alertString[MAX_STR_LEN] = "";

/* screen attributes */
int windowX = 0;
int windowY = 0;
int windowW = 1024;
int windowH = 1024;
int fullScreen = 0;

/* scale and offset of the image to display */
vector offset;
int offsetIncrement = 10;

double scale = 1.0;
double scaleIncrement = 0.01;
double minScale = 0.01;
double maxScale = 100;
double pixelSize;

/* if the textures should be dynamically loaded */
int dynamicTextures = TRUE;

/* initial action file name to apply to the displayed image to be modified */
char actionFilename[MAX_STR_LEN] = "";

/* holds last mouse x,y pos */
vector lastPos;

/* vars for gloabl key press block */
int blockKey = '\0';
int blockWait = FALSE;

/* flag for obtained prefs from file */
int gotPreferences = FALSE;

/* app globals */
char executeName[MAX_STR_LEN] = "";
int VERBOSE = 0;
int DEBUG = 0;

/** creation functions **/

/**
 * initialization of the application
 */
void init(int argc, char **argv) {
  double tempWinWidth, tempWinHeight;

  //SR_DEBUG = VP_DEBUG = DEBUG;
  //SR_VERBOSE = VP_VERBOSE = VERBOSE;

  if(DEBUG) fprintf(stdout,"initializing glut...\n");

  /* initialize glut */
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowSize(windowW, windowH);
  glutInitWindowPosition(windowX, windowY);
  glutCreateWindow("manual alignment of serial sections");

  /* initialize the app and register our functions */
  glutSetCursor(GLUT_CURSOR_CROSSHAIR);
  glutDisplayFunc(draw);
  glutReshapeFunc(reshape);
  //glutIdleFunc(idle);
  glutKeyboardFunc(keyboard);
  glutMouseFunc(mouse);
  glutMotionFunc(mouseMotion);
  glutPassiveMotionFunc(mouseMotion);

  glClearDepth(1.0f);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);
  glEnable(GL_COLOR_MATERIAL);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
  glClearColor(0,0,0,0);

  /* initialize the dataset */
  datasetInit();

  /* set an initial pixel size, can be changed later by user */
  pixelSize = 0.5/(max(curDataset->pixelsPerMM.x,curDataset->pixelsPerMM.y));

  /* size the window */
  if(!gotPreferences) {
    //offset.x = 0.0;
    //offset.y = 0.0;
    //scale = 1.0;
    curSlice = 0;
    mode = BLANK;
    tempWinWidth = windowW = curDataset->width;
    tempWinHeight = windowH = curDataset->height;
    while(windowW > glutGet(GLUT_SCREEN_WIDTH)
          || windowH > glutGet(GLUT_SCREEN_HEIGHT)) {
      scale -= scaleIncrement;
      windowW = scale*tempWinWidth;
      windowH = scale*tempWinHeight;
    }
    glutReshapeWindow(windowW, windowH);

    if(DEBUG) fprintf(stdout,"resizing window to %dx%d with scale %g...\n",
                      windowW,windowH,scale);
  }

  /* initialize the mode specific params */
  modeInit();

  /* fullscreen option */
  if(fullScreen) {
    glutFullScreen();
  }
}

/**
 * initializes dataset
 */
void datasetInit() {
  /* create dataset and load images */
  if(createDataset) {
    if(datasetInputType == VP_VOLUME) {
      curDataset = newDatasetVolume(datasetFilename, volumeFilename);
    }
    else if (datasetInputType == VP_IMAGE) {
      curDataset = newDatasetImages(datasetFilename, dataDir,
                                    datasetImageFormat, dataSliceNames,
                                    dataNumSlices, metersPerPixel.x,
                                    metersPerPixel.y, dataSliceDist);
    }
    else if (datasetInputType == VP_NOIMAGES) {
      curDataset = newDatasetContourFile(datasetFilename, sliceContourFilename);
    }
    /* read in the slice files and create the textures */
    if(!dynamicTextures && !loadImages(curDataset)) {
      fprintf(stderr,"error: there was a problem loading the slice images. exit\n");
    }

    strcpy(curDataset->positionFilename,sliceposFilename);
  }
  else if(strcmp(datasetFilename,"")) {
    curDataset = readDataset(datasetFilename);
    /* read in the slice files and create the textures */
    //if(!dynamicTextures && !loadImages(curDataset)) {
    //  fprintf(stderr,"error: there was a problem loading the slice images. exit\n");
    //}
  }
}

/**
 * initialization of a mode
 */
void modeInit() {
  /* make a menu */
  if(glutGetMenu() > 0) {
    glutDestroyMenu(glutGetMenu());
  }
  createSurfMenu();

  /* load required textures */

  /* call image init for mode */
  switch(mode) {
    case BLANK:
      strcpy(modeString,"blank: ");
      strcpy(alertString,"use the right mouse button for a menu");
      break;
    case ALIGN:
      alignImgInit();
      alignInit();
      createAlignMenu();
      break;
    case TACK:
      contourImgInit();
      contourInit();
      createContourMenu();
      break;
      /*   case LACE: */
      /*     laceImgInit(); */
      /*     laceInit(); */
      /*     //createLaceMenu(); */
      /*     break; */
  }
}

/**
 * uninitialization of a mode
 */
void modeUninit() {
  /* call image uninit for mode */
  switch(mode) {
    case ALIGN:
      alignUninit();
      break;
    case TACK:
      contourUninit();
      break;
      /*   case LACE: */
      /*     laceUninit(); */
      /*     break; */
  }
}

/**
 * cleans up, closes files, etc...
 */
void destroy(void) {
  char in;

  /* check for save prompt */
  if(datasetModified) {
    /*     sprintf(alertString,"%s modified, save? [y,n]:", curDataset->filename); */

    /*     /\* wait for key press *\/ */
    /*     in = blockForKeyPress(); */
    /*     while(in != 'y' || in != 'n') { */
    /*       sprintf(alertString,"Please answer 'y' or 'n'. %s modified, save? [y,n]:",curDataset->filename); */
    /*       in = blockForKeyPress(); */
    /*     } */

    sprintf(alertString,"!provide input on command line!");
    fprintf(stdout,"%s modified, save? [y,n,c]:", curDataset->filename);
    fscanf(stdin,"%c",&in);
    while(in != 'y' && in != 'n' && in != 'c') {
      fprintf(stdout,"read %c please answer 'y', 'n' or 'c'.\n%s modified, save? [y,n,c]:", in, curDataset->filename);
      fscanf(stdin,"%c",&in);
      fscanf(stdin,"%c",&in);
    }

    /* take action based on response */
    if(in == 'y') {
      saveDataset(curDataset);
    }
    else if(in == 'c') {
      return;
    }
  }

  alignDestroy();
  contourDestroy();
  //laceDestroy();
  exit(EXIT_SUCCESS);
}


/** state update functions **/

/**
 * checks if the display should update
 */
void idle(void) {
  static long nextUpdate = 0;
  long now = glutGet(GLUT_ELAPSED_TIME);

  return;

  /* check if we need to update */
  if(now >= nextUpdate) {

  }

  /* update based on mode */
  switch(mode) {
    case BLANK:
      break;
    case ALIGN:
      if(alignRepaintNeeded(glutGet(GLUT_ELAPSED_TIME))) {
        redisplay();
      }
      break;
    case TACK:
      if(contourRepaintNeeded(glutGet(GLUT_ELAPSED_TIME))) {
        redisplay();
      }
      break;
      /*   case LACE: */
      /*     if(laceRepaintNeeded(glutGet(GLUT_ELAPSED_TIME))) { */
      /*       redisplay(); */
      /*     } */
      /*     break; */
  }
}

/** display related functions **/

/**
 * handles the user resizing the window
 */
void reshape(int w, int h) {
  glViewport(0, 0, (GLsizei) w, (GLsizei) h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  glOrtho(0,w,0,h,-1.0,1.0);
  glMatrixMode(GL_MODELVIEW);

  glLoadIdentity();

  /* modify the scale */
  scale*=(w/(float)windowW+h/(float)windowH)/2.0f;

  //  float xpos = offset.x/windowW + 0.5;
  //  float ypos = offset.y/windowH + 0.5;
  //
  //  /* center the image in the new scale */
  //  offset.x = w*xpos - w/2;
  //  offset.y = h*ypos - h/2;
  //
  //  printf("%lf %lf\n", offset.x, offset.y);


  /* store the new window size */
  windowW = w;
  windowH = h;
}

/**
 * handles the update of the display screen
 */
void draw(void) {
  int winWidth = getDisplayWidth();
  int winHeight = getDisplayHeight();
  char tlStr[MAX_STR_LEN];

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();

  glOrtho(0,winWidth,0,winHeight,-1.0,1.0);
  glMatrixMode(GL_MODELVIEW);

  /* draw based on the current mode */
  switch(mode) {
    case BLANK:
      break;
    case ALIGN:
      alignDraw();
      break;
    case TACK:
      contourDraw();
      break;
  }

  /* draw the info strings */
  sprintf(tlStr,"%s%s",modeString,coordString);
  drawString(10, winHeight-20, tlStr, COLORS[WHITE]);

  if(strcmp(alertString,"")) {
    drawString(10, 10, alertString, COLORS[RED]);
  }

  glPopMatrix();
  swapBuffers();
}

/**
 * post a buffer swap event
 */
void swapBuffers() {
  glutSwapBuffers();
}

/**
 * post a redisplay event
 */
void redisplay() {
  glutPostRedisplay();
}

/** geom **/

/** event handlers **/

/**
 * blocks forr a key press
 * NOTE: this will peg your cpu in current implementation
 */
char blockForKeyPress() {
  blockWait = TRUE;
  while(blockWait);
  return blockKey;
}

/**
 * surf actions
 */
void surfAction(int act) {
  int handled = TRUE;
  FILE *fp;
  list *slices;
  surface *surf;
  char filename[MAX_STR_LEN];

  /* try to handle the action here */
  switch(act) {
    case 'q':
      destroy();
      break;
    case 'A': /* switch to align mode */
      if(mode != ALIGN) {
        modeUninit();
        mode = ALIGN;
        modeInit();
      }
      redisplay();
      break;
    case 'T': /* switch to tack mode */
      if(mode != TACK) {
        modeUninit();
        mode = TACK;
        modeInit();
      }
      redisplay();
      break;
    case 's': /* save the dataset */
      saveDataset(curDataset);
      redisplay();
      break;
    case 'S': /* save the slices raw */
      prepareDatasetForSave(curDataset);
      sprintf(filename,"%s.slices",curDataset->filename);
      slices = buildSlices(curDataset);
      writeSliceContours(slices,filename);

      if(curDataset->hasLabels == 1) {
        sprintf(filename,"%s.slices.lab",curDataset->filename);
        writeSliceContourLabels(slices,filename);
      }
      deleteSliceContours(slices);
      redisplay();
      break;
    case 'd': /* write the dataset to stdout */
      writeDataset(curDataset, stdout);
      fflush(stdout);
      break;
    case 'p': /* print the screen */
      screenCapture(curSlice);
      break;
    case 'i': /* zoom in */
    case 'I':
      doScale(scaleIncrement);
      break;
    case 'o': /* zoom out */
    case 'O':
      doScale(-scaleIncrement);
      break;
    case 'm': /* write out a marker list */
      if(NULL == (fp = fopen(markerFilename,"w"))) {
        fprintf(stderr, "warning: cannot open file %s to write markers.\n",
                markerFilename);
        break;
      }

      /* write the markers */
      if(writeMarkers(curDataset,fp)) {
        fprintf(stdout, "markers written succesfully to %s\n", markerFilename);
      }
      else {
        fprintf(stderr, "warning: write of marker file %s failed.\n",
                markerFilename);
      }

      break;
    case 'M':
      //writeObj(triangulateContour((contour*) getListNode((list*) getListNode(curDataset->sliceContourLists,0)->data,0)->data),"/tmp/tritest.obj");
      break;
    case '.': case '>': /* increment curSlice */
      changeSlice(1);
      break;
    case ',': case '<': /* decrement curSlice */
      changeSlice(-1);
      break;
    case 'h': /* change slice direction */
      if(curDataset->vol != NULL) {
        curDataset->vol->sliceDir++;
        if(curDataset->vol->sliceDir > SLICE) {
          curDataset->vol->sliceDir = ROW;
        }
        setSliceDirection(curDataset,curDataset->vol->sliceDir);
        modeUninit();
        modeInit();
        redisplay();
      }

      break;
    case '/': /* guess at contour correspondence */
      buildCorrespondenceGuess(curDataset->sliceContourLists);
      redisplay();
      break;
    case '?': /* clear all contour correspondence */
      clearAllCorrespondence(curDataset->sliceContourLists);
      redisplay();
      break;
    case 'z': /* perform tiling */
      resample = 0;
      prepareDatasetForSave(curDataset);
      slices = buildSlices(curDataset);
      preprocessSliceContours(slices);
      buildCorrespondenceGuess(curDataset->sliceContourLists);
      surf = tileSlices(slices);
      fillBranchedHoles(surf);

      if(surf->manifoldness == SURF_MANIFOLD) {
        fprintf(stdout,"surface is manifold with %d connected components\n",
                listSize(surf->CC));
      }
      else {
        fprintf(stdout,"surface is not manifold\n");
      }

      surfaceVerts2WorldCoords(surf,curDataset);
      deleteSliceContours(slices);

      strcpy(filename,curDataset->filename);
      strcpy(strrchr(filename,'.'),".off");
      writeOFF(surf,filename);

      if(curDataset->hasLabels == 1) {
        strcpy(strrchr(filename,'.'),".lab");
        writeLabelFile(surf,filename);
      }

      deleteSurface(surf);

      sprintf(alertString,"reconstruction completed");
      redisplay();

      break;
    case '[': /* decrease contrast */
      curDataset->contrastAdjust *= 1-contrastIncr;
      switch(mode) {
        case ALIGN:
          alignImgInit();
          break;
        case TACK:
          contourImgInit();
          break;
        default:
          break;
      }
      redisplay();
      break;
    case ']': /* increase contrast */
      curDataset->contrastAdjust *= 1+contrastIncr;
      switch(mode) {
        case ALIGN:
          alignImgInit();
          break;
        case TACK:
          contourImgInit();
          break;
        default:
          break;
      }
      redisplay();
      break;
    case ';': /* decrease brightness */
      curDataset->brightnessAdjust -= brightnessIncr;
      switch(mode) {
        case ALIGN:
          alignImgInit();
          break;
        case TACK:
          contourImgInit();
          break;
        default:
          break;
      }
      redisplay();
      break;
    case '\'': /* increase brightness */
      curDataset->brightnessAdjust += brightnessIncr;
      switch(mode) {
        case ALIGN:
          alignImgInit();
          break;
        case TACK:
          contourImgInit();
          break;
        default:
          break;
      }
      redisplay();
      break;
      /* motion of image using numeric keypad */
    case '1': /* down and left */
      doOffset(-1/scale*offsetIncrement, -1/scale*offsetIncrement);
      break;
    case '2': /* down */
      doOffset(0.0, -1/scale*offsetIncrement);
      break;
    case '3': /* down and right */
      doOffset(1/scale*offsetIncrement, -1/scale*offsetIncrement);
      break;
    case '4': /* left */
      doOffset(-1/scale*offsetIncrement, 0.0);
      break;
    case '6': /* right */
      doOffset(1/scale*offsetIncrement, 0.0);
      break;
    case '7': /* up and left  */
      doOffset(-1/scale*offsetIncrement, 1/scale*offsetIncrement);
      break;
    case '8': /* up */
      doOffset(0.0, 1/scale*offsetIncrement);
      break;
    case '9': /* up and right */
      doOffset(1/scale*offsetIncrement, 1/scale*offsetIncrement);
      break;
    default:
      handled = FALSE;
  }

  /* send the key along */
  if(!handled) {
    switch(mode) {
      case BLANK:
        break;
      case ALIGN:
        alignAction(act);
        break;
      case TACK:
        contourAction(act);
        break;
        /*     case LACE: */
        /*       laceAction(act); */
        /*       break; */
    }
  }
}

/**
 * menu creation
 */
void createSurfMenu() {
  /* create a menu if needed */
  glutCreateMenu(surfAction);
  glutAttachMenu(GLUT_RIGHT_BUTTON);

  /* add the menu items */
  glutAddMenuEntry("'A' enter align mode",'A');
  glutAddMenuEntry("'T' enter tack mode",'T');
  glutAddMenuEntry("'.' increase current slice",'.');
  glutAddMenuEntry("',' decrease current slice",',');
  glutAddMenuEntry("'i' zoom in",'i');
  glutAddMenuEntry("'o' zoom out",'o');
  glutAddMenuEntry("'h' change slice direction",'h');
  glutAddMenuEntry("'[' decrease contrast",'[');
  glutAddMenuEntry("']' increase contrast",']');
  glutAddMenuEntry("';' decrease brightness",';');
  glutAddMenuEntry("'\'' increase brightness",'\'');
  glutAddMenuEntry("'/' build correspondence guess",'/');
  glutAddMenuEntry("'z' reconstruct",'z');
  glutAddMenuEntry("'s' save dataset",'s');
  glutAddMenuEntry("'q' quit",'q');
}

/**
 * keyboard handler
 */
void keyboard(unsigned char key, int x, int y) {
  surfAction(key);
}

/**
 * mouse handler determines what kind of actions are being performed
 */
void mouse(int button, int state, int x, int y) {
  vector mousePos;

  y = getDisplayHeight() - y;

  mousePos.x = x;
  mousePos.y = y;
  mousePos.z = curDataset->firstSliceZ+curDataset->sliceDist*curSlice;

  mousePos = getImageCoordsV(mousePos);

  sprintf(coordString,"(%0.3f,%0.3f,%0.3f)",mousePos.x,mousePos.y,mousePos.z);

  switch(mode) {
    case BLANK:
      break;
    case ALIGN:
      alignMouse(button, state, mousePos);
      break;
    case TACK:
      contourMouse(button, state, mousePos);
      break;
      /*   case LACE: */
      /*     laceMouse(button, state, x, y); */
      /*     break; */
  }
}

/**
 * mouse motion handler determines the parameters of the action
 */
void mouseMotion(int x, int y) {
  vector mousePos;

  y = getDisplayHeight() - y;

  mousePos.x = x;
  mousePos.y = y;
  mousePos.z = curDataset->firstSliceZ+curDataset->sliceDist*curSlice;

  mousePos = getImageCoordsV(mousePos);

  sprintf(coordString,"(%0.3f,%0.3f,%0.3f)",mousePos.x,mousePos.y,mousePos.z);

  switch(mode) {
    case BLANK:
      break;
    case ALIGN:
      alignMouseMotion(mousePos);
      break;
    case TACK:
      contourMouseMotion(mousePos);
      break;
      /*   case LACE: */
      /*     laceMouseMotion(x,y); */
      /*     break; */
  }

  redisplay();
}

/**
 * mouse motion handler determines the parameters of the action
 */
void mousePassiveMotion(int x, int y) {
  vector mousePos;

  y = getDisplayHeight() - y;

  mousePos.x = x;
  mousePos.y = y;
  mousePos.z = curSlice;

  mousePos = getImageCoordsV(mousePos);

  sprintf(coordString,"(%0.3f,%0.3f,%0.3f)",mousePos.x,mousePos.y,mousePos.z);

  redisplay();
}

/**
 * error handler
 */
void CALLBACK error(GLenum errCode) {
  /*
    const GLubyte *errStr = gluErrorString(errCode);
    fprintf(stderr, "fatal error: %s\n", errStr);
  */
  exit(0);
}

/** util functions **/

/**
 * change the current slice number
 */
void changeSlice(int dir) {
  curSlice+=dir;
  modeUninit();
  modeInit();
  redisplay();
}

/**
 * gets the display width
 */
int getDisplayWidth() {
  return fullScreen ? glutGet(GLUT_SCREEN_WIDTH) : glutGet(GLUT_WINDOW_WIDTH);
}

/**
 * gets the display height
 */
int getDisplayHeight() {
  return fullScreen ? glutGet(GLUT_SCREEN_HEIGHT) : glutGet(GLUT_WINDOW_HEIGHT);
}

/**
 * draws a black box that will enclose a string of text
 */
void drawBlackBoxForString(char *str, GLint x, GLint y) {
  int strWid = 0;
  char *p;

  for(p = str; *p; p++) {
    strWid += glutBitmapWidth(GLUT_BITMAP_HELVETICA_18, *p);
  }

  glColor3fv(COLORS[BLACK]);
  glBegin(GL_QUADS); {
    glVertex3d(x-10,y+20,0.0);
    glVertex3d(x-10,y-10,0.0);
    glVertex3d(x+strWid+10,y-10,0.0);
    glVertex3d(x+strWid+10,y+20,0.0);
  } glEnd();
}

/**
 * draws a string at the specified coordinates
 * implementation taken from
 * http://www.york.ac.uk/services/cserv/sw/graphics/OPENGL/L23a.html
 */
void drawString(GLint x, GLint y, char* s, const GLfloat *color) {
  int lines;
  char* p;

  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  glOrtho(0.0, getDisplayWidth(),
          0.0, getDisplayHeight(), -1.0, 1.0);
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  drawBlackBoxForString(s,x,y);

  glColor3fv(color);
  glRasterPos2i(x, y);
  for(p = s, lines = 0; *p; p++) {
    if (*p == '\n') {
      lines++;
      glRasterPos2i(x, y-(lines*18));
    }
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *p);
  }
  glPopMatrix();
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
}

/**
 * parse the command line arguments
 */
void parseArgs(int argc, char **argv) {
  int opt, option_index = 0, i,
      missingRequired = FALSE,
      sliceContourSpecified = FALSE,
      datasetSpecified = FALSE,
      imageFormatSpecified = FALSE,
      volumeFilenameSpecified = FALSE,
      slicefileFilenameSpecified = FALSE,
      dirSpecified = FALSE,
      numImagesSpecified = FALSE;

  FILE *fp;

  static struct option long_options[] = {
    {"createdataset",   0, 0, 'c'},
    {"datasetfile",     1, 0, 'f'},
    {"slicecontourfile",1, 0, 'f'},
    {"echonewtackfile", 1, 0, 'e'},
    {"imageformat",     1, 0, 'i'},
    {"volumefile",      1, 0, 'v'},
    {"slicenamefile",   1, 0, 'l'},
    {"sliceposfile",    1, 0, 'z'},
    {"directory",       1, 0, 'd'},
    {"basename",        1, 0, 'b'},
    {"nodynamictex",    0, 0, 't'},
    {"numSlices",       1, 0, 'n'},
    {"size",            1, 0, 's'},
    {"mode",            1, 0, 'm'},
    {"minInd",          1, 0, 'M'},
    {"maxInd",          1, 0, 'X'},
    {"scale",           1, 0, 'S'},
    {"fullscreen",      0, 0, 'F'},
    {"verbose",         0, 0, 'V'},
    {"quiet",           0, 0, 'Q'},
    {"debug",           0, 0, 'D'},
    {"help",            0, 0, 'h'},
    {0, 0, 0, 0}
  };

  /* capture name of executable, argv[0], in global string variable */
  sprintf(executeName, "%s", argv[0]);

  /* loop through input arguments */
  while(1) {
    opt = getopt_long (argc, argv, "cf:C:e:i:v:l:z:td:b:w:h:n:s:m:M:X:S:FVQDh?",
                       long_options, &option_index);

    if(opt == -1)
      break;

    if(opt == ':') {
      printUsage();
      exit(EXIT_FAILURE);
    }

    if(opt == '?') {
      printUsage();
      exit(EXIT_FAILURE);
    }

    switch(opt) {
      case 'c':
        createDataset = TRUE;
        break;
      case 'f':
        strcpy(datasetFilename,optarg);
        datasetSpecified = TRUE;
        break;
      case 'C':
        strcpy(sliceContourFilename,optarg);
        sliceContourSpecified = TRUE;
        datasetInputType = VP_NOIMAGES;
        break;
      case 'e':
        strcpy(newTackCoordsFilename,optarg);
        if(NULL == (NEW_TACK_COORDS_FP = fopen(newTackCoordsFilename,"w+"))) {
          NEW_TACK_COORDS_FP = stdout;
        }
        echoNewTackCoords = TRUE;
        break;
      case 'i':
        /* find the format */
        if(!(strcmp(optarg,"jpeg") && strcmp(optarg,"jpg")
             && strcmp(optarg,"JPEG") && strcmp(optarg,"JPG"))) {
          datasetImageFormat = JPEG;
          imageFormatSpecified = TRUE;
        }
        else if(!(strcmp(optarg,"pnm") && strcmp(optarg,"PNM")
                  && strcmp(optarg,"ppm") && strcmp(optarg,"PPM"))) {
          datasetImageFormat = PNM;
          imageFormatSpecified = TRUE;
        }
        else {
          fprintf(stderr,"invalid image format, '%s'\n",optarg);
        }
        break;
      case 'v':
        strcpy(volumeFilename,optarg);
        volumeFilenameSpecified = TRUE;
        datasetInputType = VP_VOLUME;
        break;
      case 'l':
        strcpy(slicefileFilename,optarg);
        slicefileFilenameSpecified = TRUE;
        datasetInputType = VP_IMAGE;
        break;
      case 'z':
        strcpy(sliceposFilename,optarg);
        break;
      case 'd':
        strcpy(dataDir,optarg);
        dirSpecified = TRUE;
        break;
      case 'b':
        strcpy(dataBasename,optarg);
        break;
      case 'n':
        dataNumSlices = atoi(optarg);
        numImagesSpecified = TRUE;
        break;
      case 's':
        sscanf(optarg,"%lf,%lf,%lf",&metersPerPixel.x,&metersPerPixel.y,
               &dataSliceDist);
        break;
      case 't':
        dynamicTextures = FALSE;
        break;
      case 'm':
        if(!strcmp(optarg,"align")) {
          mode = ALIGN;
        }
        else if(!strcmp(optarg,"tack")) {
          mode = TACK;
        }
        else {
          fprintf(stderr,"warning: mode %s unrecognized\n",optarg);
        }
        break;
      case 'M':
        minSliceInd = atoi(optarg);
        break;
      case 'X':
        maxSliceInd = atoi(optarg);
        break;
      case 'S':
        scale = atof(optarg);
        break;
      case 'F':
        fullScreen = 1;
        break;
      case 'V':
        VERBOSE = 1;
        break;
      case 'Q':
        VERBOSE = 0;
        break;
      case 'D':
        DEBUG = 1;
        break;
      case 'H': case '?':
        printUsage();
        exit(EXIT_SUCCESS);
        break;
    }
  }

  /* validate args */
  if(createDataset == TRUE) {
    /* check each required argument */
    if(datasetSpecified != TRUE) {
      fprintf(stderr,"error: dataset file is a required argument during dataset creation.\n");
      missingRequired = TRUE;
    }
    if(datasetInputType == VP_NOIMAGES) {
      if(sliceContourSpecified != TRUE) {
        fprintf(stderr,"error: slice contour filename is a required argument during non-image dataset creation.\n");
        missingRequired = TRUE;
      }
    }
    else if(datasetInputType == VP_VOLUME) {
      if(volumeFilenameSpecified != TRUE) {
        fprintf(stderr,"error: volume filename is a required argument during volume dataset creation.\n");
        missingRequired = TRUE;
      }
    }
    else {
      if(dirSpecified != TRUE) {
        fprintf(stderr,"error: dataset directory is a required argument during image dataset creation.\n");
        missingRequired = TRUE;
      }
      if(imageFormatSpecified != TRUE) {
        fprintf(stderr,"error: slice image format is a required argument during image dataset creation.\n");
        missingRequired = TRUE;
      }
      if(numImagesSpecified != TRUE) {
        fprintf(stderr,"error: number of slice images is a required argument during image dataset creation.\n");
        missingRequired = TRUE;
      }
      //      if(pixSizeSpecified != TRUE) {
      //  fprintf(stderr,"error: size is a required argument during image dataset creation.\n");
      //  missingRequired = TRUE;
      //      }
    }

    /* check for missing required args */
    if(missingRequired) {
      printUsage();
      exit(EXIT_FAILURE);
    }

    /* if a slice filename file has been specified read from file */
    if(datasetInputType == VP_IMAGE && slicefileFilenameSpecified) {
      /* try to open the file */
      if(NULL == (fp = fopen(slicefileFilename,"r"))) {
        fprintf(stderr,"error: couldn't open file %s containing slice filenames.\n",slicefileFilename);
        exit(EXIT_FAILURE);
      }

      /* read the filenames */
      if(!(dataSliceNames = readStrings(fp,dataNumSlices))) {
        fprintf(stderr,"error: missing one or more slice filenames.\n");
        exit(EXIT_FAILURE);
      }
    }
    else if(datasetInputType != VP_NOIMAGES) { /* read from the command line */
      /* check that there are the required number of image names */
      if(argc - optind != dataNumSlices) {
        fprintf(stderr,"error: missing one or more slice filenames.\n");
        printUsage();
        exit(EXIT_FAILURE);
      }

      /* allocate space for the filenames */
      dataSliceNames = (char**) malloc(dataNumSlices*sizeof(char*));

      /* read the image names */
      for(i=0; optind < argc && i < dataNumSlices; i++, optind++) {
        /* allocate one name */
        dataSliceNames[i] = (char*) malloc(MAX_STR_LEN*sizeof(char));

        /* read the name from the args */
        strcpy(dataSliceNames[i],argv[optind]);
      }
    }
  }
  else if(datasetSpecified != TRUE) {
    fprintf(stderr,"error: dataset file is a required argument.\n");
    printUsage();
    exit(EXIT_FAILURE);
  }
}

/**
 * prints usage and command line options / flags
 */
void printUsage(void) {
  fprintf(stdout, "\n");

  /* print usage */
  fprintf(stdout, "Usage: %s [OPTIONS] \n", executeName);

  fprintf(stdout, "\nDataset operation:\n");
  fprintf(stdout, "  -f, --datasetfile         file to load and save to\n");
  fprintf(stdout, "  -C, --slicecontourfile    file to load slice contour vertces from\n");
  fprintf(stdout, "  -l, --slicenamefile       file to load slice image names from\n");
  fprintf(stdout, "  -z, --sliceposfile        file to load slice positions from\n");
  fprintf(stdout, "  -t, --nodynamictex        load all images and store them in memory\n");
  fprintf(stdout, "  -c, --createDataset       \n");
  fprintf(stdout, "    to create a dataset from a set of images:\n");
  fprintf(stdout, "      -f, --datasetfilename     file to load and save to\n");
  fprintf(stdout, "      -d, --directory           directory containing slice images\n");
  fprintf(stdout, "      -n, --numslices           number of slice image files\n");
  fprintf(stdout, "      -i, --imageformat         format of the slice images (jpeg,pnm)\n");
  fprintf(stdout, "     to create a dataset from a volume:\n");
  fprintf(stdout, "      -f, --datasetfilename     file to load and save to\n");
  fprintf(stdout, "      -v, --volumefile          input volume file (mgh format accepted)\n");
  //  fprintf(stdout, "          -s, --size                x,y,z pixel sizes (ex: -s 0.01,0.01,0.02)\n");

  fprintf(stdout, "\nModes of operation:\n");
  fprintf(stdout, "  -m, --mode                mode of operation (align, tack)\n");

  fprintf(stdout, "\nFuchs reconstruction flags:\n");
  fprintf(stdout, "  -M, --minInd              minimum slice index for reconstruction\n");
  fprintf(stdout, "  -X, --maxInd              maximum slice index for reconstruction\n");

  fprintf(stdout, "\nDisplay:\n");
  fprintf(stdout, "  -S, --scale               scale at which to display images\n");
  fprintf(stdout, "  -F, --fullscreen          start in fullscreen mode\n");

  fprintf(stdout, "\nMiscellaneous:\n");
  fprintf(stdout, "  -V, --verbose             output extra information (default)\n");
  fprintf(stdout, "  -Q, --quiet               supress extra information\n");
  fprintf(stdout, "  -D, --debug               enable extended output for debugging\n");
  fprintf(stdout, "  -h, --help                display this help and exit\n");
  fprintf(stdout, "  -?                        display this help and exit\n");

  fprintf(stdout, "\nReport bugs to <oph@cns.bu.edu>.\n");

  return;
}

/**
 * gets a string that shows the version of the files used
 */
char *getSurfReconVersionString() {
  return SURF_RECON_VERSION_C;
}

/**
 * gets a string that shows user and time info
 */
char *getUserString() {
  time_t rawtime;
  struct tm * timeinfo;
  char *str = (char*)malloc(1000*sizeof(char));
  str[0] = '\0';

  /* make string 'user@hostname:time' */
  getenv("USER") ? strcat(str,getenv("USER")) : NULL;;
  strcat(str,"@");
  getenv("HOSTNAME") ? strcat(str,getenv("HOSTNAME")) : NULL;;
  strcat(str,":");
  time(&rawtime);
  timeinfo = localtime(&rawtime);
  strcat(str,asctime(timeinfo));

  return str;
}

/** program flow control **/

/**
 * main function to control the stimulator
 */
#ifndef NOMAIN
int main(int argc, char** argv) {
  /* force syncing with vertical retrace (NVIDIA cards only!) */
  putenv("__GL_SYNC_TO_VBLANK=1") ;

  /* force syncing with vertical retrace (DRI drivers, e.g., ATi, only!) */
  putenv("LIBGL_SYNC_REFRESH=1");

  /* turn on memory tracing, if defined */
#ifdef MEMLEAK
  putenv("MALLOC_TRACE=surfCon_memtrace.txt");
  mtrace();
#endif

  /* parse and validate the arguments */
  parseArgs(argc,argv);

  /* perform init tasks */
  init(argc,argv);

  /* start running */
  glutMainLoop();

  return 0;
}
#endif
