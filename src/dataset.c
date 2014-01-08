/*****************************************************************************
 * dataset.c is the source file containing functions for handling
 * datasets in the surfCon application
 * Oliver Hinds <oph@cns.bu.edu> 2004-06-22
 *
 *
 *
 *****************************************************************************/

#define DATASET_VERSION_C "$Id: dataset.c,v 1.31 2007/06/01 21:48:33 oph Exp $"

#include "dataset.h"

/* number of decimal places that represents image file number */
const int numDigitsInFileNumber = 4;

/* whether we should validate the contours on dataset file save */
static int validateContoursOnSave = FALSE;

/**
 * common actions for a new dataset, independent of the image source
 */
void newDatasetCommon(dataset *ds) {
  int i;

  /* allocate lists */
  ds->sliceActionLists = newList(LIST);
  ds->sliceContourLists = newList(LIST);
  ds->sliceMarkerLists = newList(LIST);
  ds->vertices = newList(LIST);
  ds->faces = newList(LIST);
  ds->hasAdjacency = 0;
  ds->hasLabels = 0;
  ds->hasMarkers = 0;

  /* set the initial offset and display scale */
  /* zero the offset */
  offset.x = 0.0;
  offset.y = 0.0;

  /* scale */
  scale = 1.0;

  /* image processing init */
  ds->brightnessAdjust = 0.0f;
  ds->contrastAdjust = 1.0f;

  /* allocate slice images  */
  ds->slices = (image*) malloc(ds->numSlices*sizeof(image));

  for(i = 0; i < ds->numSlices; i++) {
    ds->slices[i].pixels = NULL;
    ds->slices[i].width = 0;
    ds->slices[i].height = 0;
  }

  /* allocate space for the texture ids */
  ds->sliceTextures = (unsigned int*) malloc(ds->numSlices*sizeof(GLuint));
}

/**
 * load slice contours from a file and view them with a black background
 */
dataset *newDatasetContourFile(char *filename, char *sliceFilename) {
  listNode *i,*j,*k;
  list *sl, *cv, *cList, *l;
  contour *cont;
  list *slices;
  int slind;
  vector minbounds, maxbounds;
  vertex *v;
  double w,h;

  slices = readSliceContours(sliceFilename);
  if(slices == NULL) {
    fprintf(stderr,"newDatasetSliceContourFile: error loading slice contours\n");
  }

  /* allocate the dataset */
  dataset *ds = (dataset*) malloc(sizeof(dataset));  

  ds->imageSource = VP_NOIMAGES;
  ds->imageFormat = INVALID_FORMAT;

  /* assign fields */
  strcpy(ds->filename, filename);
  ds->directory[0] = '\0';
  strcpy(ds->positionFilename, "");
  strcpy(ds->versionString,DATASET_VERSION_C);
  unCVS(ds->versionString);

  setSliceDirection(ds,SLICE);
  
  ds->numSlices = listSize(slices);

  newDatasetCommon(ds);

  /* get the slice distance */
  ds->sliceDist = getAverageIntersliceDistance(slices);
  if(ds->sliceDist < 0.0) ds->sliceDist *= -1.0;

  minbounds.x = SR_BIG;
  minbounds.y = SR_BIG;
  maxbounds.x = -SR_BIG;
  maxbounds.y = -SR_BIG;
  minbounds.z = SR_BIG;
  for(i = getListNode(slices,0), slind = 0; i; 
      i = (listNode*) i->next, slind++) {
    sl = (list*) i->data;

    /* allocate a list for this slice's tacks */
    l = newList(LIST);
    enqueue(ds->sliceContourLists,l);

    for(j = getListNode(sl,0); j; j = (listNode*) j->next) {
      cv = ((contour*) j->data)->vertices;

      /* allocate the contour */
      cont = createContour();
      enqueue(l,cont);
      cList = cont->vertices;
      cont->closed = ((contour*) j->data)->closed;

      for(k = getListNode(cv,0); k; k = (listNode*) k->next) {
	v = (vertex*) k->data;
	addTack(cList, v->x, v->y, slind);

	/* check bounds */
	if(v->x < minbounds.x) minbounds.x = v->x;
	if(v->x > maxbounds.x) maxbounds.x = v->x;
	if(v->y < minbounds.y) minbounds.y = v->y;
	if(v->y > maxbounds.y) maxbounds.y = v->y;
	if(v->z < minbounds.z) minbounds.z = v->z;
      }
    }
  }

  ds->firstSliceZ = minbounds.z;

  w = maxbounds.x-minbounds.x;
  h = maxbounds.y-minbounds.y;

  double ar = w/h;
  
  if(ar > 1.0) {
    windowH = windowW/ar;
  }
  else {
    windowW = windowH*ar;
  }

  ds->pixelsPerMM.x = 1.0;
  ds->pixelsPerMM.y = 1.0;
  ds->pixelsPerMM.z = 1.0;
  
  // set a good initial scale and offset
  scale = 0.9*max(windowW,windowH)/max(w,h);

  ds->width = windowW;
  ds->height = windowH;

  offset.x = (1/scale*windowW/2 - (minbounds.x+w/2.0));
  offset.y = (1/scale*windowH/2 - (minbounds.y+h/2.0));

  return ds;
}

/**
 * creates a dataset from an MGH volume
 */
dataset *newDatasetVolume(char *filename, char *volumeFilename) {
  /* allocate the dataset */
  dataset *ds = (dataset*) malloc(sizeof(dataset));

  /* assign fields */
  strcpy(ds->filename, filename);
  ds->directory[0] = '\0';
  ds->imageSource = VP_VOLUME;
  strcpy(ds->positionFilename, "");
  strcpy(ds->versionString,DATASET_VERSION_C);
  unCVS(ds->versionString);

  /* attempt to load the volume */
  ds->vol = loadMGHVolume(volumeFilename);

  if(ds->vol == NULL) {
    fprintf(stderr,"newDatasetVolume: error loading volume file\n");
    return NULL;
  }

  strcpy(ds->vol->filename,volumeFilename);

  /* set and initial scale (number of pixels in a unit of world coords) */
  setSliceDirection(ds,SLICE);
  
  ds->firstSliceZ = 0.0;

  /* perform common init functions */
  newDatasetCommon(ds);

  setInitialVolumeContrast(ds->vol,1.0);
  ds->contrastAdjust = ds->vol->contrastAdjust;

  return ds;
}

/**
 * creates a dataset
 */
dataset *newDatasetImages(char *filename, char *directory,
			  int imageFormat, char **sliceFilenames,
			  int numSlices, double xDist,
			  double yDist, double sliceDist) {
  int i;
  vector aveSize;

  /* allocate the dataset */
  dataset *ds = (dataset*) malloc(sizeof(dataset));

  /* assign fields */
  strcpy(ds->filename, filename);
  strcpy(ds->directory, directory);
  ds->imageFormat = imageFormat;
  ds->numSlices = numSlices;
  ds->sliceDist = sliceDist;
  ds->imageSource = VP_IMAGE;
  strcpy(ds->positionFilename, "");
  strcpy(ds->versionString,DATASET_VERSION_C);
  unCVS(ds->versionString);

  /* set and initial scale (number of pixels in a unit of world coords) */
  ds->pixelsPerMM.x = 1/xDist;
  ds->pixelsPerMM.y = 1/yDist;
  ds->pixelsPerMM.z = 1/ds->sliceDist;
  ds->firstSliceZ = 0.0;
  /* allocate space for the slice filenames */
  ds->sliceFilenames = (char**) malloc(numSlices*sizeof(char*));

  /* copy the slice filenames */
  for(i = 0; i < numSlices; i++) {
    ds->sliceFilenames[i] = (char*) malloc(MAX_STR_LEN*sizeof(char));
    strcpy(ds->sliceFilenames[i], sliceFilenames[i]);
  }

  /* perform common init functions */
  newDatasetCommon(ds);

  loadImages(ds);

  /* get some width and height info for hints */
  ds->width = windowW;
  ds->height = windowH;

  /* compute a good scale */
  aveSize.x = aveSize.y = 0;
  for(i = 0; i < numSlices; i++) {
    aveSize.x += ds->slices[i].width;
    aveSize.y += ds->slices[i].height;
  }
  aveSize.x/=numSlices;
  aveSize.y/=numSlices;

  // set the initial scale and origin
  if(fabs(ds->width-aveSize.x) > fabs(ds->height-aveSize.y)) {
    scale = ds->width/aveSize.x;
    offset.y = (ds->height-scale*aveSize.y)/(scale*2.0);
  }
  else {
    scale = ds->height/aveSize.y;
    offset.x = (ds->width-scale*aveSize.x)/(scale*2.0);
  }

  return ds;
}

/**
 * deallocates a dataset
 */
void freeDataset(dataset *ds) {

}

/**
 * reads a dataset from a dataset file
 * NOTE: this function is monolithic on purpose, keep all the version
 * dependent stuff in this one function
 */
dataset *readDataset(char *filename) {
  FILE *fp;
  dataset *ds;
  int numAlignedSlices,numTackedSlices,numLabels,numMarkedSlices,
    numVertices,numFaces,winW,winH,index,curSl,curCt;
  char str[MAX_STR_LEN] = "", trash[MAX_STR_LEN] = "";
  int i,v1,v2,v3;
  double a;
  vector x;
  list *l, *cList = NULL, *slice;
  listNode *ln;
  contour *cont, *adjCont;
  vertex *t;
  vertex *v;
  face *f;
  label *lab;
  int status = SUCCESS;

  /* build the entire filename */
  if(filename[0] != '/') {
    strcat(str,getenv("PWD"));
    strcat(str,"/");
  }
  strcat(str,filename);

  /* open the file */
  if(!(fp = fopen(str,"r"))) {
    fprintf(stderr,"error: couldn't open file %s for reading. exit\n", str);
  }

  /* allocate space for the dataset to be read and some of its lists */
  ds = (dataset*) malloc(sizeof(dataset));
  ds->sliceActionLists = newList(LIST);
  ds->sliceContourLists   = newList(LIST);
  ds->sliceMarkerLists   = newList(LIST);
  ds->imageSource = INVALID_INPUT_TYPE;
  ds->hasAdjacency = 0;
  ds->hasLabels = 0;
  ds->hasMarkers = 0;
  ds->vol = NULL;


  /* assign this filename */
  strcpy(ds->filename,filename);

  /* initialize the position filename */
  strcpy(ds->positionFilename, "");
  ds->firstSliceZ = 0.0;

  /* read the dataset header info */
  skipComments(fp,'#');

  /* decide based on the first line if this is a volume file or image file */
  fscanf(fp, "%s %s: ", str, trash);
  if(!strcmp(str,"volume")) {
    status = readDatasetVolume(fp,ds);
  }
  else if(!strcmp(str,"no")) {
    status = readDatasetNoImages(fp,ds);
  }
  else {
    status = readDatasetImage(fp,ds);
  }

  /* check the exit status */
  if(status == FAILURE) {
    return NULL;
  }

  /* read actions into the lists */

  /* read the number of slices we have actions for */
  fscanf(fp, "slices aligned: %d\n", &numAlignedSlices);
  fprintf(stdout, "reading %d align actions...",numAlignedSlices);
  fflush(stdout);

  /* read in each set of actions */
  for(i = 0; i < numAlignedSlices; i++) {
    /* allocate a list for this slice's actions */
    l = newList(LIST);
    enqueue(ds->sliceActionLists,l);

    /* read the begin string */
    fscanf(fp, "begin actions\n");

    /* read the id for this line */
    fscanf(fp, "%s", str);
    while(strcmp(str,"end")) {
      /* add the action based on type */
      if(!strcmp(str,"rotation:")) {
	/* get the properties of the rotation */
	fscanf(fp, " (%lf,%lf) %lf\n", &x.x, &x.y, &a);
	if(!feof(fp)) addRotationAction(l, x, a);
      }
      else if(!strcmp(str,"translation:")) {
	/* read the properties of the translation */
	fscanf(fp," (%lf,%lf)\n", &x.x, &x.y);
	if(!feof(fp)) addTranslationAction(l, x);
      }
      else if(!strcmp(str,"scale:")) {
	/* read the properties of the scale action */
	fscanf(fp," (%lf,%lf)\n", &x.x, &x.y);
	if(!feof(fp)) addScaleAction(l, x);
      }

      /* read the id for this line */
      fscanf(fp, "%s", str);
    }
    fscanf(fp, "\n");
  }
  fprintf(stdout,"done\n");

  /* read tacks into the lists */

  /* read the number of slices we have actions for */
  fscanf(fp, "\n\nslices tacked: %d\n", &numTackedSlices);
  fprintf(stdout, "reading %d tack lists...", numTackedSlices);

  /* read in each set of tacks */
  for(i = 0; i < numTackedSlices; i++) {
    /* allocate a list for this slice's tacks */
    l = newList(LIST);
    enqueue(ds->sliceContourLists,l);

    /* read the begin string */
    fscanf(fp, "begin slice tacks\n");

    /* read the id for this line */
    fscanf(fp, "%s", str);

    while(strcmp(str,"end")) {
      /* test for begin of new contour */
      if(!strcmp(str,"begin")) {
	fscanf(fp, " %s", str);
	/* backward compatibility */
	if(strcmp("contour",str)) {
	  fscanf(fp, " contour\n");
	}
	else {
	  strcpy(str,"open");
	  fscanf(fp, "\n");
	}

	/* allocate the contour */
	cont = createContour();
	enqueue(l,cont);
	cList = cont->vertices;

	/* see if its closed or not */
	if(!strcmp(str,"closed")) {
	  cont->closed = CLOSED;
	}

	/* see if its empty */
	fscanf(fp, "%s", str);
	if(!strcmp(str,"end")) break;
      }

      /* read vars for one tack and add it */
      fscanf(fp, " (%lf,%lf)\n", &x.x, &x.y);
      if(!feof(fp)) t = addTack(cList, x.x, x.y, i);

      /* read the id for this line */
      fscanf(fp, "%s", str);
    }
    fscanf(fp,"\n\n");
  }
  fprintf(stdout,"done\n");

  fscanf(fp, "\n\n");

  /* if there are contour adjacency lists, read them */
  fscanf(fp, "%s", str);
  if(!strcmp(str,"contour")) {
    ds->hasAdjacency = 1;

    fprintf(stdout, "reading contour adjacency lists...");
    fflush(stdout);

    curSl = 0;
    fscanf(fp, " adjacency\n%s", str);

    /* until no more slices */
    while(!strcmp(str,"slice")) {
      curCt = 0;
      fscanf(fp, "%s", str);

      /* until no more contours */
      while(!strcmp(str,"contour")) {
	ln = getListNode((list*) getListNode(ds->sliceContourLists,
					     curSl)->data,curCt);
	if(ln == NULL) {
	  fprintf(stderr,"dataset.c: can't access contour %d on slice %d for adjacency\n", curCt, curSl);

	  /* skip this one */
	  readLine(fp,str);
	  fscanf(fp, "%s", str);
	  continue;
	}
	cont = (contour*) ln->data;

	/* read the adjacent contour indices on the next contour */
	fscanf(fp,"%d",&index);
	while(index != -1) {
	  /* add the contour in the next slice to the adjacency list, if its not already there */
	  adjCont = getListNode((list*) getListNode(ds->sliceContourLists,curSl+1)->data,index)->data;
	  if(NULL == findInListLN(cont->adjacentContours,adjCont)) {
	    enqueue(cont->adjacentContours,adjCont);
	  }

	  /* read the next index */
	  fscanf(fp,"%d",&index);
	}
	curCt++;
	fscanf(fp, "%s", str);
      }
      curSl++;
    }
    fprintf(stdout, "done\n");
    fflush(stdout);
  }

  /* read labels into the lists, if there are any */
  fscanf(fp, " %s:",str);

  str[6] = '\0';
  if(!strcmp(str,"labels")) {
    ds->hasLabels = 1;

    /* read the number of labels */
    fscanf(fp, "%d\n", &numLabels);
    fprintf(stdout, "reading %d labels...", numLabels);

    /* read in each label*/
    fscanf(fp, "%s", str);
    for(numLabels = 0; strcmp(str,"slices"); numLabels++) {
      /* allocate a new label */
      lab = (label*) malloc(sizeof(label));

      lab->slice = atoi(str);
      fscanf(fp, "%d %d %d\n", &lab->cont, &lab->vert, &lab->lab);

      /* search for the vertex in the slice contours */
      if(lab->slice >= listSize(ds->sliceContourLists)
	 || lab->cont >= listSize(slice = (list*) getListNode(ds->sliceContourLists,lab->slice)->data)
	 || lab->vert >= listSize((cont = (contour*)getListNode(slice,lab->cont)->data)->vertices)
	 || (v = (vertex*) getListNode(cont->vertices,lab->vert)->data) == NULL) {
	free(lab);
	continue;
      }

      lab->sliceLink = slice;
      lab->contourLink = cont;
      lab->vertexLink = v;
      v->label = lab->lab;

      fscanf(fp,"%s",str);
    }
    fprintf(stdout,"done\n");

    fscanf(fp, "\n%s:", str);
    str[6] = '\0';
  }

  /* read markers into the lists */

  /* read the number of slices we have markers for */
  fscanf(fp, "%d\n", &numMarkedSlices);
  fprintf(stdout, "reading %d marker lists...", numMarkedSlices);

  /* read in each set of markers */
  for(i = 0; i < numMarkedSlices; i++) {
    ds->hasMarkers = 1;

    /* allocate a list for this slice's markers */
    l = newList(LIST);
    enqueue(ds->sliceMarkerLists,l);

    /* read the begin string */
    fscanf(fp, "begin slice markers\n");

    /* read the id for this line */
    fscanf(fp, "%s", str);
    while(strcmp(str,"end")) {
      /* read vars for one marker and add it */
      fscanf(fp, " (%lf,%lf)\n", &x.x, &x.y);
      if(!feof(fp)) t = addTack(l, x.x, x.y, i);

      /* read the id for this line */
      fscanf(fp, "%s", str);
    }
    fscanf(fp,"\n\n");
  }
  fprintf(stdout,"done\n");

  /* read surface information */

  /* read the number of vertices */
  fscanf(fp, "\n\nnum vertices: %d", &numVertices);
  fprintf(stdout, "reading %d vertices...", numVertices);

  /* allocate the vertex list */
  ds->vertices = newList(LIST);

  /* read in each vertex */
  for(i = 0; i < numVertices; i++) {
    /* read the coordinates */
    fscanf(fp,"vertex: (%lf,%lf,%lf)\n", &x.x, &x.y, &x.z);

    if(feof(fp)) break;

    /* allocte the vertex, assign coords, add to list */
    v = createVertex();
    v->x = x.x;
    v->y = x.y;
    v->z = x.z;

    v->number = -1;
    v->boundary = FALSE;

    enqueue(ds->vertices,v);
  }
  fprintf(stdout,"done\n");

  /* read the number of faces */
  fscanf(fp, "\nnum faces: %d", &numFaces);
  fprintf(stdout, "reading %d faces...", numFaces);

  /* allocate the vertex list */
  ds->faces = newList(LIST);

  /* read in each face */
  for(i = 0; i < numFaces; i++) {
    /* read the coordinates */
    fscanf(fp,"face: (%d,%d,%d)\n", &v1, &v2, &v3);

    if(feof(fp)) break;

    /* allocte the face, assign vertices, add to list */
    f = (face*) malloc(sizeof(face));
    f->v1 = v1;
    f->v2 = v2;
    f->v3 = v3;

    enqueue(ds->faces,f);
  }
  fprintf(stdout,"done\n\n");

  /** read the preferences and such **/
  fscanf(fp, "\npreferences:\n\n");
  fprintf(stdout, "reading preferences...");

  /* read the window size */
  fscanf(fp,"window size: (%d,%d)\n", &winW, &winH);

  glutReshapeWindow(winW,winH);

  /* read the current slice */
  fscanf(fp,"curslice: %d\n", &curSlice);

  /* read the mode */
  fscanf(fp,"mode: %d\n", &mode);

  /* read the scale */
  fscanf(fp,"scale: %lf\n", &scale);

  /* read the offset */
  fscanf(fp,"offset: (%lf,%lf)", &offset.x, &offset.y);

  /* image processing stuff */
  fscanf(fp, "%s", str);
  while(!feof(fp)) {
    /* find the image processing parameter name */
    if(!strcmp("brightnessAdjust:",str)) {
      /* read the brightness */
      fscanf(fp,"%f", &ds->brightnessAdjust);
    }
    else if(!strcmp("contrastAdjust:",str)) {
      /* read the contrast */
      fscanf(fp,"%f", &ds->contrastAdjust);
    }
    fscanf(fp, "%s", str);
  }

  /* validate the brightness and contrast */
  if(ds->brightnessAdjust < minBrightness) {
    ds->brightnessAdjust = minContrast;
  }
  else if(ds->brightnessAdjust > maxBrightness) {
    ds->brightnessAdjust = maxBrightness;
  }

  if(ds->contrastAdjust < minContrast) {
    ds->contrastAdjust = minContrast;
  }
  else if(ds->contrastAdjust > maxContrast) {
    ds->contrastAdjust = maxContrast;
  }

  /* flag signaling succesful preference read */
  gotPreferences = TRUE;

  fprintf(stdout,"done\n");

  fprintf(stdout,"\n");

  /* allocate slice images  */
  ds->slices = (image*) malloc(ds->numSlices*sizeof(image));

  for(i = 0; i < ds->numSlices; i++) {
    ds->slices[i].pixels = NULL;
    ds->slices[i].width = 0;
    ds->slices[i].height = 0;
  }

  /* allocate space for the texture ids */
  ds->sliceTextures = (unsigned int*) malloc(ds->numSlices*sizeof(GLuint));

  /* if its an image dataset, read them to get size info */
  if(ds->imageSource == VP_IMAGE) {
    loadImages(ds);
  }

  return ds;
}

/**
 * reads the volume specific dataset params
 */
int readDatasetVolume(FILE *fp, dataset *ds) {
  int i;

  char str[MAX_STR_LEN] = "";

  /* assign the image source */
  ds->imageSource = VP_VOLUME;

  skipWhite(fp);
  readLine(fp,str);
  fprintf(stdout, "volume filename: %s\n", str);

  /* load the volume */
  ds->vol = loadMGHVolume(str);

  if(ds->vol == NULL) {
    fprintf(stderr,"dataset: error loading the volume file\n");
    return FAILURE;
  }

  strcpy(ds->vol->filename,str);

  fprintf(stdout,"pixelsPerMM: (%f,%f,%f)\n", 
	  ds->pixelsPerMM.y, ds->pixelsPerMM.x, ds->pixelsPerMM.z);

  /* determine if there is an external slice position file or a slice direction */
  fscanf(fp, "%s", str);

  if(!strcmp(str,"slice")) {
    fscanf(fp, " direction: %d", &i);
    setSliceDirection(ds,i);
    fscanf(fp, "%s", str);
  }
  else {
    setSliceDirection(ds,SLICE);
  }

  if(!strcmp(str,"position")) {
    fscanf(fp, " file: %s\n", ds->positionFilename);
    fprintf(stdout, "position file: %s\n", ds->positionFilename);
    fscanf(fp, "%s", str);
  }
  else if(!strcmp(str,"first")) {
    fscanf(fp, " slice Z: %lf\norigin: ", &ds->firstSliceZ);
    fprintf(stdout, "first slice Z: %lf\n", ds->firstSliceZ);    
  }

  /* now get the origin BE CAREFUL, THE STRING 'origin:' WAS ALREADY READ! */
  fscanf(fp, " (%lf,%lf)\n", &ds->origin.x, &ds->origin.y);
  fprintf(stdout, "origin (%lf,%lf)\n", ds->origin.x, ds->origin.y);

  skipComments(fp,'#');
  fscanf(fp, "dataset version string: ");
  readLine(fp,ds->versionString);

  /* validate the version */
  if(!checkVersionString(ds->versionString)) {
    fprintf(stderr,"error: dataset in file %s is the wrong version. exit\n",
	    ds->filename);
    return FAILURE;
  }

  fscanf(fp,"\n");

  return SUCCESS;
}

/**
 * reads the image specific dataset params
 */
int readDatasetImage(FILE *fp, dataset *ds) {
  char str[MAX_STR_LEN] = "";
  int i;

  /* assign the image source */
  ds->imageSource = VP_IMAGE;

  skipWhite(fp);
  readLine(fp,ds->directory);
  fprintf(stdout, "slice directory: %s\n", ds->directory);

  skipComments(fp,'#');
  fscanf(fp, "slice image format: %d\n", &i);
  ds->imageFormat = i;

  /* read the pixel to world space coordinate transform */
  skipComments(fp,'#');
  fscanf(fp,"pixels per mm: (%lf,%lf)",
	 &ds->pixelsPerMM.x, &ds->pixelsPerMM.y);
  fprintf(stdout, "pixels per mm: (%lf,%lf)\n",
	  ds->pixelsPerMM.x, ds->pixelsPerMM.y);

  skipComments(fp,'#');
  fscanf(fp, "mm between slices: %lf\n", &ds->sliceDist);
  ds->pixelsPerMM.z = 1/ds->sliceDist;
  fprintf(stdout, "slice distance (mm): %lf\n", ds->sliceDist);

  skipComments(fp,'#');

  /* determine if there is an external slice position file or not */
  fscanf(fp, "%s", str);

  if(!strcmp(str,"position")) {
    fscanf(fp, " file: %s\norigin: ", ds->positionFilename);
    fprintf(stdout, "position file: %s\n", ds->positionFilename);
  }
  else if(!strcmp(str,"first")) {
    fscanf(fp, " slice Z: %lf\norigin: ", &ds->firstSliceZ);
    fprintf(stdout, "first slice Z: %lf\n", ds->firstSliceZ);    
  }

  /* now get the origin BE CAREFUL, THE STRING 'origin:' WAS ALREADY READ! */
  fscanf(fp, " (%lf,%lf)\n", &ds->origin.x, &ds->origin.y);
  fprintf(stdout, "origin (%lf,%lf)\n", ds->origin.x, ds->origin.y);

  skipComments(fp,'#');
  fscanf(fp, "dataset version string: ");
  readLine(fp,ds->versionString);

  /* validate the version */
  if(!checkVersionString(ds->versionString)) {
    fprintf(stderr,"error: dataset in file %s is the wrong version. exit\n",
	    ds->filename);
    return FAILURE;
  }

  /* read number of slice files */
  fscanf(fp, "number of slices: %d\n\n", &ds->numSlices);

  /* allocate space for the slice filenames */
  ds->sliceFilenames = (char**) malloc(ds->numSlices*sizeof(char*));

  /* read the slice filenames */
  fscanf(fp, "slice filenames:\n");
  for(i = 0; i < ds->numSlices; i++) {
    ds->sliceFilenames[i] = (char*) malloc(MAX_STR_LEN*sizeof(char));
    fscanf(fp, "%s\n", ds->sliceFilenames[i]);
  }
  fscanf(fp,"\n");

  return SUCCESS;
}

/**
 * reads the no images specific dataset params
 */
int readDatasetNoImages(FILE *fp, dataset *ds) {
  char str[MAX_STR_LEN] = "";

  /* assign the image source */
  ds->imageSource = VP_NOIMAGES;

  skipWhite(fp);

  /* read the pixel to world space coordinate transform */
  skipComments(fp,'#');
  fscanf(fp,"pixels per mm: (%lf,%lf)",
	 &ds->pixelsPerMM.x, &ds->pixelsPerMM.y);
  fprintf(stdout, "pixels per mm: (%lf,%lf)\n",
	  ds->pixelsPerMM.x, ds->pixelsPerMM.y);

  skipComments(fp,'#');
  fscanf(fp, "mm between slices: %lf\n", &ds->sliceDist);
  ds->pixelsPerMM.z = 1/ds->sliceDist;
  fprintf(stdout, "slice distance (mm): %lf\n", ds->sliceDist);

  skipComments(fp,'#');

  /* determine if there is an external slice position file or not */
  fscanf(fp, "%s", str);

  if(!strcmp(str,"position")) {
    fscanf(fp, " file: %s\norigin: ", ds->positionFilename);
    fprintf(stdout, "position file: %s\n", ds->positionFilename);
  }
  else if(!strcmp(str,"first")) {
    fscanf(fp, " slice Z: %lf\norigin: ", &ds->firstSliceZ);
    fprintf(stdout, "first slice Z: %lf\n", ds->firstSliceZ);    
  }

  /* now get the origin BE CAREFUL, THE STRING 'origin:' WAS ALREADY READ! */
  fscanf(fp, " (%lf,%lf)\n", &ds->origin.x, &ds->origin.y);
  fprintf(stdout, "origin (%lf,%lf)\n", ds->origin.x, ds->origin.y);

  skipComments(fp,'#');
  fscanf(fp, "dataset version string: ");
  readLine(fp,ds->versionString);

  /* validate the version */
  if(!checkVersionString(ds->versionString)) {
    fprintf(stderr,"error: dataset in file %s is the wrong version. exit\n",
	    ds->filename);
    return FAILURE;
  }

  /* read number of slice files */
  fscanf(fp, "number of slices: %d\n\n", &ds->numSlices);


  return SUCCESS;
}

/**
 * load the images from the desired locations
 */
int loadImages(dataset *ds) {
  char imgFilename[MAX_STR_LEN];
  int i;

  /* validate the input */
  if(ds == NULL || ds->imageSource == INVALID_INPUT_TYPE 
     || ds->imageSource == VP_NOIMAGES) {
    return 0;
  }

  /* print some info */
  fprintf(stdout,"loading %d slices images ...\n", ds->numSlices);

  /* load the images and create textures */
  for(i = 0; i < ds->numSlices; i++) {
    /* get the filename */
    getSliceFilename(ds,i,imgFilename);

    /* load the texture */
    ds->sliceTextures[i] = loadTexture(ds, imgFilename, ds->imageFormat,
				       &ds->slices[i]);

    /* if we are only loading as necessary, unload now */
    if(dynamicTextures) {
      unloadTexture(i);
    }
  }

  fprintf(stdout,"\n");

  return 1;
}

/**
 * creates a blank image
 */
image *createBlankImage() {
  image *img = createImage(1,1,1);
  img->pixels[0] = 0;
  return img;
}

/**
 * loads the nth image from a dataset
 */
image *loadImage(dataset *ds, int n) {
  image *img = NULL;
  char filename[MAX_STR_LEN];

  /* validate the input */
  if(ds == NULL || n < 0 || n >= ds->numSlices 
     || ds->imageSource == VP_NOIMAGES
     || ds->imageSource == INVALID_INPUT_TYPE) {
    return createBlankImage();
  }

  if(ds->imageSource == VP_VOLUME) {
    ds->vol->selectedVoxel[ds->vol->sliceDir] = n;
    img = sliceVolume(ds->vol,0,ds->vol->sliceDir,0);
    ds->sliceTextures[n] = imageTexture(ds,img);
  }
  else {
    img = (image*) malloc(sizeof(image));
    getSliceFilename(ds,n,filename);

    /* delete the old texture, if its one */
    if(glIsTexture(ds->sliceTextures[n])) {
      unloadTexture(n);
    }

    /* store the new texture */
    ds->sliceTextures[n] = loadTexture(ds, filename,ds->imageFormat,img);
  }

  return img;
}

/**
 * loads an image and makes a texture for it
 * either the image is read from a file into memory, or
 * retreived if already loaded
 */
GLuint loadTexture(dataset *ds, char *filename, int format, image *outputIm) {
  GLuint sliceTex = 0;

  image *im = NULL;
  //printf("%d\n", ds->imageSource);
  /* validate the input */
  if(ds == NULL 
     || ds->imageSource == VP_NOIMAGES
     || ds->imageSource == INVALID_INPUT_TYPE) {
    im = createBlankImage();
  }
  else {
    im = (image*) malloc(sizeof(image));
    /* perform texture init */
    glPixelStorei(GL_UNPACK_ALIGNMENT,1);

    if(VERBOSE) {
      fprintf(stdout,"loading %s ... ", filename);
      fflush(stdout);
    }

    /* load the file */
    switch(format) {
    case DICOM:
      im = readDICOM(filename);
      break;
    case JPEG:
      im = readJPEG(filename);
      break;
    case TIFF:
      im = readTIFF(filename);
      break;
    case PNM:
      im = readPNM(filename);
      break;
    default:
      im = NULL;
      break;
    }

    /* validate the image load */
    if(NULL != im) {
      flipImage(im);
    }
    else {
      fflush(stdout);
      fprintf(stderr, "\nerror: can't load image %s\n", filename);
      return FAILURE;
    }
  }

  if(VERBOSE) {
    fprintf(stdout,"done");
    fprintf(stdout,"\n");
  }
  fflush(stdout);

  /* perform texture imaging if not just loading for info */
  if(ds != NULL) {
    sliceTex = imageTexture(ds,im);
  }

  /* deallocate the image buffer */
  free(im->pixels);
  im->pixels = NULL;
  *outputIm = *im;

  return sliceTex;
}

/**
 * image the texture with opengl
 */
GLuint imageTexture(dataset *ds, image *im) {
  GLenum format = GL_LUMINANCE;
  GLenum textureMethod = getTextureMethod();
  GLuint tex;
  int maxTex;
  double mult;

  /* get the texture format */
  if(im->numChannels == 3) {
    format = GL_RGB;
  }
  else if(im->numChannels == 1) {
    format = GL_LUMINANCE;
  }

  /* get the id for the textures */
  glGenTextures(1, &tex);

  /* set the contrast and brightness */
  glPixelTransferf(GL_RED_SCALE, ds->contrastAdjust);
  glPixelTransferf(GL_GREEN_SCALE, ds->contrastAdjust);
  glPixelTransferf(GL_BLUE_SCALE, ds->contrastAdjust);

  /* brightness adjustment */
  glPixelTransferf(GL_RED_BIAS, ds->brightnessAdjust);
  glPixelTransferf(GL_GREEN_BIAS, ds->brightnessAdjust);
  glPixelTransferf(GL_BLUE_BIAS, ds->brightnessAdjust);

  /* create the image texture */
  glBindTexture(textureMethod, tex);
  glTexParameteri(textureMethod, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri(textureMethod, GL_TEXTURE_WRAP_T, GL_CLAMP);
  glTexParameteri(textureMethod, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(textureMethod, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

  /* pad to power of two if there are no extensions */
  if(textureMethod == GL_TEXTURE_2D) {
    glGetIntegerv(GL_MAX_TEXTURE_SIZE,(GLint*)&maxTex);
    if(max(im->width,im->height) > maxTex) {
      fprintf(stdout,"warning: image size larger than GL_MAX_TEXTURE_SIZE. resizing...");
      mult = maxTex/(double)(max(im->width,im->height));
      resizeImageBilinear(im,mult*im->width,mult*im->height);
      resizeMultiplier = mult;
      fprintf(stdout,"done\n");
    }
    else {
      resizeMultiplier = 1.0f;
    }
    padImagePow2(im);
  }

  /* image the texture */
  glTexImage2D(textureMethod, 0, im->numChannels, im->width+im->padX,
	       im->height+im->padY, 0, format, GL_UNSIGNED_SHORT,
	       im->pixels);

  return tex;
}

/**
 * unloads a texture to free up memory
 */
void unloadTexture(int i) {
/*   if(VERBOSE) { */
/*     fprintf(stdout,"unloading texture %d ... ", tex); */
/*   } */  
  if(curDataset == NULL || i < 0 || i > curDataset->numSlices) {
    return;
  }

  glDeleteTextures(1,(GLuint*)&curDataset->sliceTextures[i]);
  curDataset->sliceTextures[i] = 0;

/*   if(VERBOSE) { */
/*     fprintf(stdout,"done\n"); */
/*   } */
}

/*
 * gets a slice number's filename
 */
int getSliceFilename(dataset *ds, int sliceNum, char *imgFilename) {
  /* validate input */
  if(ds->imageSource == VP_VOLUME || sliceNum >= ds->numSlices) {
    fprintf(stderr,"error: trying to load nonexistant slice file\n");
    return FALSE;
  }

  /* build the filename */
  strcpy(imgFilename,"");
  if(ds->directory[0] != '/') {
    strcat(imgFilename,getenv("PWD"));
    strcat(imgFilename,"/");
  }

  strcat(imgFilename,ds->directory);
  /* check for dos crlf */
  if(imgFilename[strlen(imgFilename)-1] == '\r') {
    imgFilename[strlen(imgFilename)-1] = '\0';
  }

  if(strcmp("",ds->directory)) {
    strcat(imgFilename,"/");
  }

  strcat(imgFilename,ds->sliceFilenames[sliceNum]);
  /* check for dos crlf */
  if(imgFilename[strlen(imgFilename)-1] == '\r') {
    imgFilename[strlen(imgFilename)-1] = '\0';
  }


/*   /\* add leading zeros to filename *\/ */
/*   for(i = numDigitsInFileNumber; i > 1; i--) { */
/*     if(sliceNum < pow(10,i-1)) strcat(imgFilename,"0"); */
/*   } */

/*   /\* file number *\/ */
/*   strcat(imgFilename,itoa(sliceNum)); */

  /* file format testing and reading */
/*   switch(ds->sliceFormat) { */
/*   case JPEG: */
/*     strcat(imgFilename,".jpg"); */
/*     break; */
/*   default: */
/*     fprintf(stderr,"error: file format not supported\n"); */
/*     return FALSE; */
/*     break; */
/*   } */

  return TRUE;
}

/**
 * prepares the dataset structure for saving, updates the 
 * hasLables, hasMarkers, hasAdjacent flags are set
 */
void prepareDatasetForSave(dataset *ds) {
  listNode *s, *c, *v;
  list *slice, *verts;
  contour *cont;

  if(ds == NULL) return;

  // make sure all contours have vertices
  for(s = getListNode(ds->sliceContourLists,0); s; s = (listNode*) s->next) {
    slice = (list*) s->data;
    for(c = getListNode(slice,0); c; c = (listNode*) c->next) {
      cont = (contour*) c->data;
      verts = ((contour*)c->data)->vertices;
      if(listSize(cont->vertices) < 1) { // delete the contour
	markForDeletion(c);
	deleteContour(cont);
	c->data = NULL;
      }
    }
    deleteMarkedNodes(slice);
  }


  // adjacent 
  for(s = getListNode(ds->sliceContourLists,0); s; s = (listNode*) s->next) {
    slice = (list*) s->data;
    for(c = getListNode(slice,0); c; c = (listNode*) c->next) {
      if(listSize(((contour*)c->data)->adjacentContours) > 0) {
	ds->hasAdjacency = 1;
	break;
      }
    }
  }

  // markers
  for(s = getListNode(ds->sliceMarkerLists,0); s; s = (listNode*) s->next) {
    slice = (list*) s->data;
    for(c = getListNode(slice,0); c; c = (listNode*) c->next) {
      if(listSize(((contour*)c->data)->adjacentContours) > 0) {
	ds->hasMarkers = 1;
	break;
      }
    }
  }

  // labels
  for(s = getListNode(ds->sliceContourLists,0); s; s = (listNode*) s->next) {
    slice = (list*) s->data;
    for(c = getListNode(slice,0); c; c = (listNode*) c->next) {
      verts = ((contour*)c->data)->vertices;
      for(v = getListNode(verts,0); v; v= (listNode*) v->next) {
	
	if(((vertex*)v->data)->label != -1) {
	  ds->hasLabels = 1;
	  break;
	}
      }
    }
  }
}

/**
 * saves a dataset from a dataset struct to the file specified in the dataset
 */
int saveDataset(dataset *ds) {
  FILE *fp;
  char filename[MAX_STR_LEN] = "";
  int validated = SUCCESS, suc;

  /* validate the contours */
  if(validateContoursOnSave) {
    validated = validateContours(ds);
  }

  /* construct the filenme */
  if(ds->filename[0] != '/') {
    strcat(filename,getenv("PWD"));
    strcat(filename,"/");
  }
  strcat(filename,ds->filename);

  /* open the file */
  if(!(fp = fopen(filename,"w+"))) {
    fprintf(stderr,"error: couldn't open file %s for writing. exit\n",
	    ds->filename);
    return 0;
  }

  suc = writeDataset(ds,fp);

  if(suc == FAILURE) {
    sprintf(alertString,"save of %s failed",curDataset->filename);
  }
  else {
    sprintf(alertString,"wrote dataset file %s",curDataset->filename);
  }

  if(validated == FAILURE) {
    strcat(alertString,". INTERSECTING CONTOURS FOUND!");
  }

  return suc;
}

/**
 * write the dataset header for a volume image source
 */
int writeDatasetVolumeHeader(dataset *ds, FILE *fp) {
  char tmp[MAX_STR_LEN];

  fprintf(fp, "volume filename: %s\n", ds->vol->filename);

  /* decide if to write the slice direction */
  if(ds->vol->sliceDir != SLICE) {
    fprintf(fp, "slice direction: %d\n", ds->vol->sliceDir);
  }

  /* decide if to write the slice position file line */
  if(strcmp(ds->positionFilename,"")) {
    fprintf(fp, "position file: %s\n", ds->positionFilename);
  }
  else {
    fprintf(fp, "first slice Z: %lf\n", ds->firstSliceZ);    
  }

  fprintf(fp, "origin: (%lf,%lf)\n", ds->origin.x, ds->origin.y);

  strcpy(tmp,DATASET_VERSION_C);
  unCVS(tmp);
  fprintf(fp, "dataset version string: %s\n", tmp);

  fprintf(fp,"\n");

  return SUCCESS;
}

/**
 * write the dataset header for a volume image source
 */
int writeDatasetImageHeader(dataset *ds, FILE *fp) {
  char tmp[MAX_STR_LEN];
  int i;

  fprintf(fp, "slice directory: %s\n", ds->directory);
  fprintf(fp, "slice image format: %d\n", ds->imageFormat);

  /* write the world to pixel space transform */
  fprintf(fp,"pixels per mm: (%lf,%lf)\n",
	  ds->pixelsPerMM.x, ds->pixelsPerMM.y);

  fprintf(fp, "mm between slices: %g\n", ds->sliceDist);

  /* decide if to write the slice position file line */
  if(strcmp(ds->positionFilename,"")) {
    fprintf(fp, "position file: %s\n", ds->positionFilename);
  }
  else {
    fprintf(fp, "first slice Z: %lf\n", ds->firstSliceZ);    
  }

  fprintf(fp, "origin: (%lf,%lf)\n", ds->origin.x, ds->origin.y);

  strcpy(tmp,DATASET_VERSION_C);
  unCVS(tmp);
  fprintf(fp, "dataset version string: %s\n", tmp);

  /* write number of slices */
  fprintf(fp, "number of slices: %d\n", ds->numSlices);
  fprintf(fp, "\n");

  /* read the slice filenames */
  fprintf(fp, "slice filenames:\n");
  for(i = 0; i < ds->numSlices; i++) {
    fprintf(fp, "%s\n", ds->sliceFilenames[i]);
  }
  fprintf(fp,"\n");

  return SUCCESS;
}

/**
 * write the dataset header for no image source
 */
int writeDatasetNoImagesHeader(dataset *ds, FILE *fp) {
  char tmp[MAX_STR_LEN];

  fprintf(fp, "no images\n");

  /* write the world to pixel space transform */
  fprintf(fp,"pixels per mm: (%lf,%lf)\n",
	  ds->pixelsPerMM.x, ds->pixelsPerMM.y);

  fprintf(fp, "mm between slices: %g\n", ds->sliceDist);

  /* decide if to write the slice position file line */
  if(strcmp(ds->positionFilename,"")) {
    fprintf(fp, "position file: %s\n", ds->positionFilename);
  }
  else {
    fprintf(fp, "first slice Z: %lf\n", ds->firstSliceZ);    
  }

  fprintf(fp, "origin: (%lf,%lf)\n", ds->origin.x, ds->origin.y);

  strcpy(tmp,DATASET_VERSION_C);
  unCVS(tmp);
  fprintf(fp, "dataset version string: %s\n", tmp);

  /* write number of slices */
  fprintf(fp, "number of slices: %d\n", ds->numSlices);
  fprintf(fp, "\n");

  return SUCCESS;

}

/**
 * writes a dataset from a dataset to a file stream
 */
int writeDataset(dataset *ds, FILE* fp) {
  char tmp[MAX_STR_LEN] = "";

  /* printf */
  fprintf(stdout,"writing dataset...");

  /* write some info about the session this was called in */
  fprintf(fp, "# dataset file generated by surfCon\n");
  fprintf(fp, "# last saved by %s", getUserString());

  strcpy(tmp,getSurfReconVersionString());
  unCVS(tmp);
  fprintf(fp, "# surfCon version: %s\n\n", tmp);

  prepareDatasetForSave(ds);

  /* write dataset header info */
  if(ds->imageSource == VP_VOLUME) {
    writeDatasetVolumeHeader(ds,fp);
  }
  else if(ds->imageSource == VP_NOIMAGES) {
    writeDatasetNoImagesHeader(ds,fp);
  }
  else {
    writeDatasetImageHeader(ds,fp);
  }

  /* align action writing */
  dumpAlign(ds,fp);

  /* write contours */
  dumpContours(ds,fp);

  /* write contour adjacency */
  if(ds->hasAdjacency) {
    dumpContourAdjacency(ds,fp);
  }

  /* write labels */
  if(ds->hasLabels) {
    dumpLabel(ds,fp);
  }
  else {
    fprintf(fp,"slices labels: 0\n");
  }

  /* write markers from the lists */
  if(ds->hasMarkers) {
    dumpMarker(ds,fp);
  }
  else {
    fprintf(fp, "slices marked: 0\n");
  }

  /* write surface info */
  dumpSurface(ds,fp);

  /* write the preferences and such */
  fprintf(fp, "\npreferences:\n\n");

  /* write the window size */
  fprintf(fp,"window size: (%d,%d)\n", windowW, windowH);

  /* write the current slice */
  fprintf(fp,"curslice: %d\n", curSlice);

  /* write the mode */
  fprintf(fp,"mode: %d\n", mode);

  /* write the scale */
  fprintf(fp,"scale: %lf\n", scale);

  /* write the offset */
  fprintf(fp,"offset: (%lf,%lf)\n", offset.x, offset.y);

  /* read the brightness */
  fprintf(fp,"brightnessAdjust: %f\n", ds->brightnessAdjust);

  /* read the contrast */
  fprintf(fp,"contrastAdjust: %f\n", ds->contrastAdjust);

  fprintf(stdout,"done\n");

  fclose(fp);

  /* if the current slice has no contours, make one for tracing 
     (because we just deleted it above in prepareDatasetForSave) */
  if(listSize((list*)getListNode(ds->sliceContourLists,curSlice)->data) < 1) {
    contourInit();
  }

  return 1;
}

/**
 * dumps the align actions for a dataset to a file stream
 */
void dumpAlign(dataset *ds, FILE *fp) {
  list *l;
  action *a;
  listNode *i,*j;

  /* write the number of slices we have actions for */
  fprintf(fp, "slices aligned: %d\n", listSize(ds->sliceActionLists));

  /* write each set of actions */
  for(i = getListNode(ds->sliceActionLists,0); i; i = (listNode*) i->next) {
    /* read the begin string */
    fprintf(fp, "begin actions\n");

    /* get the list for these actions */
    l = (list*) i->data;

    /* iterate over actions */
    for(j = getListNode(l,0); j; j = (listNode*) j->next) {
      /* get the action */
      a = (action*) j->data;

      /* write the action on a line */
      switch(a->type) {
      case ROTATION:
	fprintf(fp, "rotation: (%lf,%lf) %lf\n",
		a->rotCenter.x, a->rotCenter.y, a->angle);
	break;
      case TRANSLATION:
	fprintf(fp,"translation: (%lf,%lf)\n", a->trans.x, a->trans.y);
	break;
      case SCALE:
	fprintf(fp,"scale: (%lf,%lf)\n", a->scale.x, a->scale.y);
	break;
      }
    }
    /* print the end action list id */
    fprintf(fp, "end\n\n");
  }
}

/**
 * dumps the tacks for a dataset to a file stream
 */
void dumpContours(dataset *ds, FILE *fp) {
  list *l,*cList;
  contour *cont;
  vertex*t;
  listNode *i,*j,*k;
  int sl,ct;

  /* write the number of slices we have tacks for */
  fprintf(fp, "slices tacked: %d\n", listSize(ds->sliceContourLists));

  /* write each set of tack lists */
  for(i = getListNode(ds->sliceContourLists,0), sl = 0; i;
      i = (listNode*) i->next, sl++) {
    /* get the list for this slice */
    l = (list*) i->data;

    if(listSize(l) == 0) {
      fprintf(fp, "begin slice tacks\nend\n\n");
      continue;
    }

    fprintf(fp, "begin slice tacks\n");
    /* write the tacks from each contour */
    for(j = getListNode(l,0), ct = 0; j; j = (listNode*) j->next, ct++) {
      cont = (contour*)j->data;

      /* check for zero length */
/*       if(listSize(cont->vertices) < 1) { */
/* 	removeContour(ds,sl,ct); */
/* 	deleteContour(cont); */
/* 	continue; */
/*       } */

      fprintf(fp, "begin %s contour\n",
	      (cont->closed == CLOSED) ? "closed" : "open");
      /* get the list for this contour */
      cList = cont->vertices;

      /* iterate over tacks, write each */
      for(k = getListNode(cList,0); k; k = (listNode*) k->next) {
	/* get the tack */
	t = (vertex*) k->data;

	/* write the tack on a line */
	fprintf(fp, "tack: (%lf,%lf)\n", t->x, t->y);
      }
    }
    /* print the end list id */
    fprintf(fp, "end\n\n");
  }
}

/**
 * dumps the contour adjacency information
 */
void dumpContourAdjacency(dataset *ds, FILE *fp) {
  list *l, *adjList;
  contour *cont;
  int index;
  listNode *i,*j,*k;

  /* write the number of slices we have tacks for */
  fprintf(fp, "contour adjacency\n");

  /* iterate over slices */
  for(i = getListNode(ds->sliceContourLists,0); i; i = (listNode*) i->next) {
    /* get the list for this slice */
    l = (list*) i->data;
    fprintf(fp, "slice\n");

    /* iterate over contours */
    for(j = getListNode(l,0); j; j = (listNode*) j->next) {
      cont = (contour*) j->data;
      adjList = cont->adjacentContours;

      fprintf(fp, "contour");

      /* print each index */
      for(k = getListNode(adjList,0); k; k = (listNode*) k->next) {
	index = findInListI((list*) ((listNode*)i->next)->data, k->data);
	if(index != FAILURE) {
	  fprintf(fp, " %d", index);
	}
	else {
	  fprintf(stderr,"couldnt find adjacent contour, %p, %p, %p\n",i,j,k);
	}
      }

      fprintf(fp, " -1\n");
    }
  }

  fprintf(fp,"\n");
}

/**
 * dumps the labels for the vertices
 */
void dumpLabel(dataset *ds, FILE *fp) {
  int slice, cont, vert, numLabels = 0;
  listNode *i,*j,*k;
  list *l;
  contour *c;
  vertex *v;

  
  for(i = getListNode(ds->sliceContourLists,0), slice = 0; 
      i; i = (listNode*) i->next, slice++) {
    /* get the list for this slice */
    l = (list*) i->data;

    /* iterate over contours */
    for(j = getListNode(l,0), cont = 0; j; j = (listNode*) j->next, cont++) {
      c = (contour*) j->data;
      if(c->vertices == NULL) continue;

      /* iterate over vertices */
      for(k = getListNode(c->vertices,0), vert = 0; k; 
	  k = (listNode*) k->next, vert++) {
	v = (vertex*) k->data;
	if(v->label != -1) {
	  numLabels++;
	}
      }
    }
  }

  fprintf(fp,"slices labels: %d\n", numLabels);

  for(i = getListNode(ds->sliceContourLists,0), slice = 0; 
      i; i = (listNode*) i->next, slice++) {
    /* get the list for this slice */
    l = (list*) i->data;

    /* iterate over contours */
    for(j = getListNode(l,0), cont = 0; j; j = (listNode*) j->next, cont++) {
      c = (contour*) j->data;
      if(c->vertices == NULL) continue;

      /* iterate over vertices */
      for(k = getListNode(c->vertices,0), vert = 0; k; 
	  k = (listNode*) k->next, vert++) {
	v = (vertex*) k->data;
	if(v->label != -1) {
	  fprintf(fp,"%d %d %d %d\n",slice, cont, vert, v->label);
	}
      }
    }
  }

  fprintf(fp,"\n");
}

/**
 * dumps the markers for a dataset to a file stream
 */
void dumpMarker(dataset *ds, FILE *fp) {
  list *l;
  vertex *t;
  listNode *i, *j;

  /* write the number of slices we have markers for */
  fprintf(fp, "slices marked: %d\n", listSize(ds->sliceMarkerLists));

  /* write each set of marker lists */
  for(i = getListNode(ds->sliceMarkerLists,0); i; i = (listNode*) i->next) {
    /* get the list for this slice */
    l = (list*) i->data;

    if(listSize(l) == 0) {
      fprintf(fp, "begin slice markers\nend\n\n");
      continue;
    }

    fprintf(fp, "begin slice markers\n");
    /* write the markers from each contour */
    for(j = getListNode(l,0); j; j = (listNode*) j->next) {
      /* get the marker */
      t = (vertex*) j->data;

      /* write the marker on a line */
      fprintf(fp, "marker: (%lf,%lf)\n", t->x, t->y);
    }
    /* print the end list id */
    fprintf(fp, "end\n\n");
  }
}

/**
 * dumps the surface info  for a dataset to a file stream
 */
void dumpSurface(dataset *ds, FILE *fp) {
  vertex *v;
  face *f;
  listNode *i;

  /* write the number of vertices */
  fprintf(fp, "num vertices: %d\n", listSize(ds->vertices));

  /* write each vertex */
  for(i = getListNode(ds->vertices,0); i; i = (listNode*) i->next) {
    v = (vertex*) i->data;
    fprintf(fp,"vertex: (%lf,%lf,%lf)\n", v->x, v->y, v->z);
  }
  fprintf(fp, "\n");

  /* write the number of faces */
  fprintf(fp, "num faces: %d\n", listSize(ds->faces));

  /* write each face */
  for(i = getListNode(ds->faces,0); i; i = (listNode*) i->next) {
    f = (face*) i->data;
    fprintf(fp,"face: (%d,%d,%d)\n", f->v1, f->v2, f->v3);
  }
  fprintf(fp, "\n");
}

/**
 * assign the boundary vertices
 */
void assignBoundaries(dataset *ds) {
  list *slice;
  contour *cont = NULL;
  listNode *sliceNode;
  listNode *contNode;
  listNode *vertNode;

  /* iterate over slices */
  for(sliceNode = getListNode(ds->sliceContourLists,0); sliceNode;
      sliceNode = (listNode*) sliceNode->next) {
    slice = (list*) sliceNode->data;

    /* iterate over contours in a slice */
    for(contNode = getListNode(slice,0); contNode;
	contNode = (listNode*) contNode->next) {
      cont = (contour*) contNode->data;
    }

    /* reset all the vertices in this contour to be non-boundary */
    for(vertNode = getListNode(cont->vertices,0); vertNode;
	vertNode = (listNode*) vertNode->next) {
      ((vertex*) vertNode)->boundary = FALSE;
    }

    /* if the contour is not closed and the contour has at least one
       vertex, identify the boundary vertices */
    if(!cont->closed && listSize(cont->vertices) > 1) {
      ((vertex*)getListNode(cont->vertices,0)->data)->boundary = TRUE;
      ((vertex*)getListNode(cont->vertices,
			    listSize(cont->vertices)-1)->data)->boundary
	= TRUE;
    }
  }
}

/**
 * assigns the z coordinates to the contour vertices
 */
void assignZCoords(dataset *ds) {
  list *slice;
  list *verts;
  double z;
  listNode *i, *j, *k;
  FILE* fp = NULL;
  int ind;

  /* open the slice positions file, if we should */
  if(strcmp(ds->positionFilename,"")) {
    fp = fopen(ds->positionFilename,"r");
  }

  /* give slice positions */
  for(i = getListNode(ds->sliceContourLists,0), ind = 0; i; i = (listNode*) i->next, ind++) {
    slice = (list*) i->data;

    /* get the slice position */
    if(fp) {
      fscanf(fp,"%lf",&z);
    }
    else {
      z = ds->firstSliceZ+ds->sliceDist*ind;
    }

    /* go through all contours in slice */
    for(j = getListNode(slice,0); j; j = (listNode*) j->next) {
      verts = ((contour*) j->data)->vertices;

      /* go through all vertices in vertex list */
      for(k = getListNode(verts,0); k; k = (listNode*) k->next) {
	/* assign the z coordinate */
	((vertex*) k->data)->z = z;
      }
    }
  }
}

/**
 * checks the version string for compatibility with this version
 */
int checkVersionString(char *otherVersion) {
  return TRUE;
}

/**
 * makes an id string not an id string by replacing Id with ID
 */
char *unCVS(char *str) {
  if(str != NULL) {
    str[2] -= 'a'-'A';
  }
  return str;
}
