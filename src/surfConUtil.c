/*****************************************************************************
 * surfConUtil.c is the source file for utility functions for the surface
 * recon
 * Oliver Hinds <oph@cns.bu.edu> 2004-02-04
 *
 *
 *
 *****************************************************************************/

#include"surfConUtil.h"

#ifdef GL_TEXTURE_RECTANGLE_EXT
#ifndef GL_TEXTURE_RECTANGLE_NV
#define GL_TEXTURE_RECTANGLE_NV GL_TEXTURE_RECTANGLE_EXT
#endif
#else
#ifdef GL_TEXTURE_RECTANGLE_NV
#ifndef GL_TEXTURE_RECTANGLE_EXT
#define GL_TEXTURE_RECTANGLE_EXT GL_TEXTURE_RECTANGLE_NV
#endif
//#else
//#define GL_TEXTURE_RECTANGLE_NV GL_TEXTURE_2D
//#define GL_TEXTURE_RECTANGLE_EXT GL_TEXTURE_2D
#endif
#endif


static GLenum textureMethod = -1;

/**
 * save the current display to an image file
 */
void screenCapture(int slice) {
  char filename[255];
  sprintf(filename,"slice%04d.pnm",slice);

  /* create the image */
  image *img = createImage(getDisplayWidth(),getDisplayHeight(),3);
  glReadPixels(0,0,getDisplayWidth(),getDisplayHeight(),
               GL_RGB,GL_UNSIGNED_SHORT,img->pixels);
  flipImage(img);

  /* write the image */
  fprintf(stdout,"saving %s\n",filename);
  writePNM(img,filename,1);

  freeImage(img);
}

/** coordinate system manipulation **/

/**
 * sets the slice direction of a dataset
 */
void setSliceDirection(dataset *ds, int sliceDir) {
  float voxelSize[3];
  int dims[3];

  /* validate */
  if(ds == NULL || ds->vol == NULL) {
    return;
  }

  if(sliceDir > SLICE || sliceDir < ROW) {
    sliceDir = SLICE;
  }

  getVoxelSize(ds->vol,voxelSize);
  getSliceDims(sliceDir,dims);

  ds->vol->sliceDir = sliceDir;

  ds->pixelsPerMM.y = 1/voxelSize[dims[0]-1];
  //ds->origin.y = ds->vol->vox2wrld[dims[0]-1][3];
  ds->height = ds->vol->size[dims[0]];

  ds->pixelsPerMM.x = 1/voxelSize[dims[1]-1];
  //ds->origin.x = ds->vol->vox2wrld[dims[1]-1][3];
  ds->width = ds->vol->size[dims[1]];

  ds->pixelsPerMM.z = 1/voxelSize[dims[2]-1];
  //ds->origin.z = ds->vol->vox2wrld[dims[2]-1][3];
  ds->sliceDist = 1.0;//voxelSize[dims[2]-1];
  ds->numSlices = ds->vol->size[dims[2]];

}

/**
 * permute coords of tacks if slice dir not SLICE
 */
void permuteCoordsFromSliceDir(list *slices, int sliceDir) {
  int dims[3];
  list *slice, *verts;
  listNode *i,*j,*k;
  vertex *v;
  int d;
  double tmp[3];

  /* validate */
  if(slices == NULL || sliceDir == SLICE) {
    return;
  }

  getSliceDims(!(sliceDir-1)+1,dims);

  for(i = getListNode(slices,0); i; i = (listNode*) i->next) {
    slice = (list*) i->data;
    for(j = getListNode(slice,0); j; j = (listNode*) j->next) {
      verts = ((contour*) j->data)->vertices;
      for(k = getListNode(verts,0); k; k = (listNode*) k->next) {
        v = (vertex*) k->data;
        tmp[0] = v->x;
        tmp[1] = v->y;
        tmp[2] = v->z;
        for(d = 0; d < 3; d++) {
          if(dims[d] == ROW) {
            v->x = tmp[d];
          }
          else if(dims[d] == COL) {
            v->y = tmp[d];
          }
          if(dims[d] == SLICE) {
            v->z = tmp[d];
          }
        }
      }
    }
  }
}



/**
 * get the image coords of a window space vertex
 */
vertex getImageCoordsVert(vertex v) {
  v.x = v.x/scale-offset.x;
  v.y = v.y/scale-offset.y;
  v.z = v.z;

  return v;
}

/**
 * get the image coords of a window space vector
 */
vector getImageCoordsV(vector v) {
  v.x = v.x/scale-offset.x;
  v.y = v.y/scale-offset.y;
  v.z = v.z;

  return v;
}

/**
 * get the image coords of a window space quad
 */
quadri getImageCoordsQ(quadri q) {
  q.v1 = getImageCoordsV(q.v1);
  q.v2 = getImageCoordsV(q.v2);
  q.v3 = getImageCoordsV(q.v3);
  q.v4 = getImageCoordsV(q.v4);
  return q;
}

/**
 * get the window coords of an image space vertex
 */
vertex getWindowCoordsVert(vertex v) {
  v.x = scale*(v.x+offset.x);
  v.y = scale*(v.y+offset.y);
  v.z = v.z;

  return v;
}

/**
 * get the window coords of an image space vector
 */
vector getWindowCoordsV(vector v) {
  v.x = scale*(v.x+offset.x);
  v.y = scale*(v.y+offset.y);
  v.z = v.z;

  return v;
}

/**
 * get the window coords of an image space quad
 */
quadri getWindowCoordsQ(quadri q) {
  q.v1 = getWindowCoordsV(q.v1);
  q.v2 = getWindowCoordsV(q.v2);
  q.v3 = getWindowCoordsV(q.v3);
  q.v4 = getWindowCoordsV(q.v4);
  return q;
}

/**
 * get the world coords of a vector
 */
vector getWorldCoordsV(vector v) {
  if(DEBUG) {
    fprintf(stderr,"vox2wrld=\n%02.3f %02.3f %02.3f %02.3f\n%02.3f %02.3f %02.3f %02.3f \n%02.3f %02.3f %02.3f %02.3f \n%02.3f %02.3f %02.3f %02.3f\n",
            curDataset->vol->vox2wrld[0][0], curDataset->vol->vox2wrld[0][1],
            curDataset->vol->vox2wrld[0][2], curDataset->vol->vox2wrld[0][3],
            curDataset->vol->vox2wrld[1][0], curDataset->vol->vox2wrld[1][1],
            curDataset->vol->vox2wrld[1][2], curDataset->vol->vox2wrld[1][3],
            curDataset->vol->vox2wrld[2][0], curDataset->vol->vox2wrld[2][1],
            curDataset->vol->vox2wrld[2][2], curDataset->vol->vox2wrld[2][3],
            curDataset->vol->vox2wrld[3][0], curDataset->vol->vox2wrld[3][1],
            curDataset->vol->vox2wrld[3][2], curDataset->vol->vox2wrld[3][3]
            );

    fprintf(stderr,"window coords v= [%02.3f %02.3f %02.3f]\n", v.x, v.y, v.z);
  }

  matrixMult4byV(curDataset->vol->vox2wrld,v,&v);

  if(DEBUG) {
    fprintf(stderr,"world coords v= [%02.3f %02.3f %02.3f]\n", v.x, v.y, v.z);
  }
  return v;
}

/**
 * get the world coords of a quad
 */
quadri getWorldCoordsQ(quadri q) {
  q.v1 = getWorldCoordsV(q.v1);
  q.v2 = getWorldCoordsV(q.v2);
  q.v3 = getWorldCoordsV(q.v3);
  q.v4 = getWorldCoordsV(q.v4);
  return q;
}

/**
 * copies a list of contours, transforming them into world coords based on
 * the matrix provided
 */
list *cloneSliceContours(dataset *ds) {
  list *slices, *newSlices;
  list *slice, *newSlice = NULL, *lastNewSlice;
  contour *cont, *newCont;
  vertex *v;
  listNode *i, *j, *k;
  float CoordShuffle[4][4] = {{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0}};
  int dims[3], shuffleCoords = 0, ind;

  /* coord shuffle, if we need it */
  if(ds->vol != NULL && ds->vol->sliceDir != SLICE) {
    getSliceDims(ds->vol->sliceDir,dims);
    /* need the inverse of the normal, but since matrix is orthogonal, use transpose */
    for(ind = 0; ind < 3; ind++) {
      CoordShuffle[dims[ind]-1][ind] = 1.0f; /* this is the transpose */
    }
    CoordShuffle[3][3] = 1;
    shuffleCoords = 1;
  }

  /* vslidate input */
  if(ds == NULL || listSize(ds->sliceContourLists) < 1) {
    return NULL;
  }

  slices = ds->sliceContourLists;

  /* iterate over slices, transforming the contours in each */
  newSlices = newList(LIST);
  for(i = getListNode(slices,0); i; i = (listNode*) i->next) {
    slice = (list*) i->data;

    /* iterate over the contours, transforming the vertices in each */
    lastNewSlice = newSlice;
    newSlice = newList(LIST);
    enqueue(newSlices,newSlice);
    for(j = getListNode(slice,0); j; j = (listNode*) j->next) {
      cont = (contour*) j->data;
      newCont = cloneContour(cont);

      /* add a cloned version of this contour */
      enqueue(newSlice,newCont);

      /* fixes the contour adjacency info */
      for(k = getListNode(lastNewSlice,0); k; k = (listNode*) k->next) {
        setListNodeData(findInListLN(((contour*)k->data)->adjacentContours,cont),newCont);
      }

      /* transform the vertices */
      for(k = getListNode(newCont->vertices,0); k; k = (listNode*) k->next) {
        v = (vertex*) k->data;

        if(shuffleCoords) {
          matrixMult4byVert(CoordShuffle,*v,v);
        }
      }
    }
  }

  return newSlices;
}

/**
 * applies a desired offset of the display
 */
void doOffset(double x, double y) {
  offset.x += x;
  offset.y += y;

  if(DEBUG) fprintf(stdout, "offset changed to (%g,%g)\n",offset.x,offset.y);

  redisplay();
}

/**
 * applies a desired scaling of the display
 */
void doScale(double s) {
  double oldScale = scale;
  vector ds;

  scale *= 1+s;

  if(scale > maxScale || scale < minScale) {
    scale = oldScale;
  }
  else {
    ds.x = -curDataset->width*s/2.0;
    ds.y = -curDataset->height*s/2.0;

    offset.x *= 1-s;
    offset.y *= 1-s;

    doOffset(ds.x,ds.y);

    if(DEBUG) fprintf(stdout, "scale changed to %g\n",scale);
    redisplay();
  }
}

/**
 * write a list of markers to a file stream
 */
int writeMarkers(dataset *ds, FILE* fp) {
  int ind;
  listNode *i,*j;
  list *curSlice;
  vertex *t;
  FILE *posFile = NULL;

  /* slice positions*/
  double slicePos;
  int positionsFromFile = strcmp(ds->positionFilename,"");

  /* open the positions file if we need it */
  if(positionsFromFile) {
    posFile = fopen(ds->positionFilename,"r");
    if(!posFile) { /* error opening the file */
      fprintf(stderr,"error: can't open file %s for reading\n",
              ds->positionFilename);
      return 0;
    }
  }

  /* write each marker */
  for(i=getListNode(ds->sliceMarkerLists,0), ind=0; i; i = (listNode*) i->next, ind++) {
    if(posFile) {
      fscanf(posFile, "%lf", &slicePos);
    }
    else {
      slicePos = ind*ds->sliceDist;
    }

    curSlice = (list*) i->data;
    /* print each contour on the slice */
    for(j=getListNode(curSlice,0); j; j = (listNode*) j->next) {
      t = (vertex*) j->data;
      fprintf(fp, "%lf %lf %lf\n", t->x/ds->pixelsPerMM.x,
              t->y/ds->pixelsPerMM.y, slicePos);
    }
  }

  /* close files */
  fclose(fp);

  /*  if(positionsFromFile) {
      fclose(posFile);
      }
  */
  return 1;
}

/**
 * create a list of slice contours to be used as input to the fuchs
 * algorithm from a dataset
 */
list *buildSlices(dataset *ds) {
  /* validate */
  if(ds == NULL || ds->sliceContourLists == NULL || listSize(ds->sliceContourLists) < 1) return NULL;

  listNode *i, *j, *k;
  list *verts;
  list *slice;
  vertex *v;

  assignZCoords(curDataset);
  list *sl = cloneSliceContours(ds);

  if(ds->vol != NULL) {
    permuteCoordsFromSliceDir(sl,ds->vol->sliceDir);
  }
  else { // convert pixels into mm

    for(i = getListNode(sl,0); i; i = (listNode*) i->next) {
      slice = (list*) i->data;
      for(j = getListNode(slice,0); j; j = (listNode*) j->next) {
        verts = ((contour*) j->data)->vertices;
        for(k = getListNode(verts,0); k; k = (listNode*) k->next) {
          v = (vertex*) k->data;
          v->x/=ds->pixelsPerMM.x;
          v->y/=ds->pixelsPerMM.y;
        }
      }
    }
  }
  return sl;
}

/**
 * transform surface into world coordinates
 */
int surfaceVerts2WorldCoords(surface *surf, dataset *ds) {
  float towrld[4][4];

  /* validate */
  if(ds == NULL || surf == NULL || surf->numVertices < 1) return FAILURE;

  if(ds->vol != NULL && ds->vol->vox2wrld != NULL) {
    transformSurfaceVertices(surf,ds->vol->vox2wrld);
  }
  else {
    towrld[0][0] = 1/ds->pixelsPerMM.x;
    towrld[1][1] = 1/ds->pixelsPerMM.y;
    towrld[2][2] = 1/ds->pixelsPerMM.z;
    towrld[3][3] = 1.0f;

    towrld[0][1] = towrld[0][2] = towrld[0][3] =
        towrld[1][0] = towrld[1][2] = towrld[1][3] =
        towrld[2][0] = towrld[2][1] = towrld[2][3] =
        towrld[3][0] = towrld[3][1] = towrld[3][2] = 0.0f;
    transformSurfaceVertices(surf,towrld);
  }

  return SUCCESS;
}

/** texturing support **/

/**
 * gets the texture method used based on availability of extensions
 */
int getTextureMethod() {
  const char *ext;

  /* set the texture method if we haven't yet */
  if(textureMethod == -1) {
    ext = (char*) glGetString(GL_EXTENSIONS);

    /* check the extensions string for the texture rectangle extensions */
    if(strstr(ext,"GL_EXT_texture_rectangle")) {
      textureMethod = GL_TEXTURE_RECTANGLE_EXT;
    }
    else if(strstr(ext, "GL_NV_texture_rectangle")) {
      textureMethod = GL_TEXTURE_RECTANGLE_NV;
    }
    else {
      textureMethod = GL_TEXTURE_2D;
    }
  }

  return textureMethod;
}


/********************************************************************
 * $Source: /home/cvs/PROJECTS/SurfaceConstructor/src/surfConUtil.c,v $
 * Local Variables:
 * mode: C
 * fill-column: 76
 * comment-column: 0
 * End:
 ********************************************************************/
