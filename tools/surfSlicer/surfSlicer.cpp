/******************************************************************************
 * surfSlicer.cpp reads in an obj file, slices it up into parallel slices and
 * writes out a nuages compatible list of contours
 * Oliver Hinds <oph@bu.edu> 2004-08-11
 *
 * This application is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this application; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *****************************************************************************/

#include "surfSlicer.h"

#include "vtkOBJReader.h"
#include "vtkConeSource.h"
#include "vtkProperty.h"
#include "vtkCellArray.h"
#include "vtkPlane.h"
#include "vtkSurfCutter.h"
#include "vtkStripper.h"
#include "vtkTriangleFilter.h"
#include "vtkPolyDataMapper.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkInteractorStyleTrackballCamera.h"
#include "vtkCamera.h"
#include "vtkActor.h"
#include "vtkRenderer.h"

#include <vector>
#include <algorithm>
using namespace std;

#define FTOL      0.0000000001
#define FCOMPTOL  0.0000001
#define SLICEPERT 0.0000001
#define EPSILON   (FTOL*100000)
#define BIG       100000000
#define PI        3.14159265358979
#define X         0
#define Y         1
#define Z         2
#define XOR(a,b)  (!(a) ^ !(b))
#define IN        1
#define OUT       -1

// some geometric utility functions

/**
 * reconstruct a 3d point using two dimensions and the other slice dim value
 */
void reconstructPoint(float *inp, int dim, float val, float *outp) {
  for(int i = 0, curDim = 0; i < 3; i++) {
    if(i == dim) {
      outp[i] = val;
    }
    else {
      outp[i] = inp[curDim++];
    }
  }
}

/**
 * point assignment using two dimensions from a three d point
 */
void assignPoint(float *inp, int dim, float *outp) {
  for(int i = 0, curDim = 0; i < 3; i++) {
    if(i == dim) continue;
    outp[curDim++] = inp[i];
  }
}

/**
 *
 */
float area2d(float *p0, float *p1, float *p2) {
  return (p1[X] - p0[X]) * (p2[Y] - p0[Y]) -
    (p2[X] - p0[X]) * (p1[Y] - p0[Y]);
}

/**
 * tests three points for collinearity
 */
bool collinear(float *p0, float *p1, float *p2) {
  return fabs(area2d(p0,p1,p2)) < FTOL;
}

/**
 *
 */
bool left(float *p0, float *p1, float *p2) {
  return area2d(p0,p1,p2) > 0;
}

/**
 * tests two line segments for intersection
 */
bool segmentsIntersect(float *e00, float *e01, float *e10, float *e11) {
  // test the segments for intersection
  if(collinear(e00,e01,e10) || collinear(e00,e01,e11) ||
     collinear(e00,e10,e11) || collinear(e01,e10,e11)) {
    return false;
  }

  return XOR(left(e00,e01,e10), left(e00,e01,e11)) &&
    XOR(left(e00,e10,e11), left(e01,e10,e11));
}

// some debugging functions to print points and lists of points
void printPoint(vtkPoints *points, int i) {
  float *p = points->GetPoint(i);
  fprintf(stderr,"%f %f %f\n",p[0],p[1],p[2]);
}

void printContour(vtkPoints *points, vector<int> &contour) {
  fprintf(stderr,"[ ");
  for(vector<int>::iterator i = contour.begin(); i != contour.end(); i++) {
    printPoint(points,*i);
  }
  fprintf(stderr,"]\n");
}

/**
 * gets a slightly perturbed point, perturbed by epsilon from the
 * point in the direction of the average of the two normals
 */
void getPerturbedPoint(float *point, float *left, float *right,
			int dim, int dir, float *perturbed) {
  float left2d[2];
  float point2d[2];
  float right2d[2];

  // get the 2d versions (ignore the constant dimension)
  assignPoint(left,dim,left2d);
  assignPoint(point,dim,point2d);
  assignPoint(right,dim,right2d);

  // get the angle of the tangent line at the point
  float rightAngle = atan2(right2d[Y]-point2d[Y],right2d[X]-point2d[X]) + PI/2;
  rightAngle += rightAngle < 0 ? 2*PI : 0;
  float leftAngle = atan2(point2d[Y]-left2d[Y],point2d[X]-left2d[X]) + PI/2;
  leftAngle += leftAngle < 0 ? 2*PI : 0;

  // get the perturbed angle as the average of the other normals
  float pertAngle = (rightAngle+leftAngle)/2 + (dir < 0 ? PI : 0);

  // create a perturbed 2d point
  float perturbed2d[2];
  perturbed2d[0] = point2d[0] + cos(pertAngle)*EPSILON;
  perturbed2d[1] = point2d[1] + sin(pertAngle)*EPSILON;

  // traansform it into a 3d point
  reconstructPoint(perturbed2d,dim,point[dim],perturbed);
}


/**
 * find the convex hull of a set of a contour
 */
vector<int> getHull(vector<int> &contour, vtkPoints *points, int dim) {
  // first find the point with the smallest y coordinate to start from
  int smallPointId = -1;
  float smallPoint[] = {BIG,BIG};
  for(vector<int>::iterator i = contour.begin(); i != contour.end(); i++) {
    float point[2];
    assignPoint(points->GetPoint(*i),dim,point);

    if(point[Y] < smallPoint[Y]) {
      smallPoint[X] = point[X];
      smallPoint[Y] = point[Y];
      smallPointId = *i;
    }
  }

  // find the points in the hull by finding the next point of lowest
  // counterclockwise angle from the current point
  vector<int> hull;
  hull.push_back(smallPointId);

  int curPointId = smallPointId;
  float curPoint[] = {smallPoint[X], smallPoint[Y]};
  float lastPoint[] = {smallPoint[X]-1.0, smallPoint[Y]};
  float accumAngle = 0;
  do {
    // find the point with smallest counterclockwise angle
    float curSmallAngle = 6.29;
    int curSmallAnglePointId = -1;
    for(vector<int>::iterator i = contour.begin(); i != contour.end(); i++) {
      if(*i == curPointId) continue;

      float searchPoint[2];
      assignPoint(points->GetPoint(*i),dim,searchPoint);
      float searchAngle = atan2(searchPoint[Y]-curPoint[Y],
				searchPoint[X]-curPoint[X]);
      searchAngle = searchAngle < 0 ? searchAngle + 2*PI : searchAngle;
      searchAngle -= accumAngle;
      if(searchAngle >= -FTOL && searchAngle < curSmallAngle) {
	curSmallAngle = searchAngle;
	curSmallAnglePointId = *i;
      }
    }

    // add the smallest angled point to the hull
    hull.push_back(curSmallAnglePointId);

    // update search variables
    lastPoint[X] = curPoint[X];
    lastPoint[Y] = curPoint[Y];
    assignPoint(points->GetPoint(curSmallAnglePointId),dim,curPoint);
    curPointId = curSmallAnglePointId;
    accumAngle += curSmallAngle;
  } while(curPointId != smallPointId);

  return hull;
}

/**
 * corrects a contour that intersects itself by adding vertices just
 * to the outside of the convex hull to the end of the contour
 */
void unintersectContour(vector<int> &contour, vtkPoints *points, int dim,
			int pertDir) {
  // check size of contour
  if(contour.size() < 3) return;

  // first find the convex hull of the contour then traverse it from
  // the end point of the contour to the begin point. when vertices
  // are encountered, append a new vertex to the end of the contour
  // perturbed slightly from the itersecting vertex. this will do the
  // least violence to the contour while still assuring its
  // non-self-intersection

  /* compute the convex hull of the contour */
  vector<int> hull = getHull(contour,points,dim);

  // traverse the contour backwards until we encounter a point on the
  // convex hull, adding perturbed points. next traverse the contour
  // forward from the beginning adding perturbed points until a point
  // on the hull is encountered. then start adding perturbed points
  // while traversing the convex hull from the closeest to the end
  // vertex on the hull until the closest vertex to the start vertex
  // on the hull is encountered
  vector<int> beginVerticesAdded;
  vector<int> endVerticesAdded;

  // first (from begining) and last (from the end) hull points on the contour
  vector<int>::iterator firstHullPoint = contour.begin();
  vector<int>::iterator lastHullPoint = contour.end()-1;
  vector<int>::reverse_iterator ri;
  vector<int>::iterator i;

  // tmp vars for traversals
  float perturbed[3];

  // traverse contour from the end back to a vertex in the hull,
  // adding perturbed points

  // add the first perturbed point if the endpoint isnt on the hull
  if(hull.end() == find(hull.begin(),hull.end(),contour.back())) {
    // iterate backwards till we find a point on the hull
    for(ri = contour.rbegin()+1; ri != contour.rend()-1 &&
	  hull.end() == find(hull.begin(),hull.end(),*ri); ri++) {
      getPerturbedPoint(points->GetPoint(*(ri)),
			points->GetPoint(*(ri-1)),
			points->GetPoint(*(ri+1)),
			dim,pertDir,perturbed);
      endVerticesAdded.push_back(points->InsertNextPoint(perturbed));
    }
  }
  if(endVerticesAdded.size() > 0) lastHullPoint =
				    find(hull.begin(), hull.end(), *ri);

  // traverse from the beginning until we find a point on the hull,
  // adding perturbed points

  // add the first perturbed point if the begin point isnt on the hull
  if(hull.end() == find(hull.begin(),hull.end(),contour.front())) {
    // iterate till we find a point on the hull
    for(i = contour.begin()+1; i != contour.end()-1 &&
	  hull.end() == find(hull.begin(),hull.end(),*i); i++) {
      getPerturbedPoint(points->GetPoint(*(i)),
			points->GetPoint(*(i-1)),
			points->GetPoint(*(i+1)),
			dim,pertDir,perturbed);
      beginVerticesAdded.push_back(points->InsertNextPoint(perturbed));
    }
  }
  if(beginVerticesAdded.size() > 0) firstHullPoint =
				      find(hull.begin(), hull.end(), *i);

  // if the first and last points are the same we are done
  if(firstHullPoint != lastHullPoint) {
    // traverse the hull from the last to first point, adding
    // perturbed points to the contour
    int hullIncr = hull.end() ==
      find(find(hull.begin(),hull.end(),*lastHullPoint),
	   hull.end(),*firstHullPoint) ? -1 : 1;
    for(vector<int>::iterator hullPoint =
	  find(hull.begin(),hull.end(),*lastHullPoint) + hullIncr;
	*hullPoint != *firstHullPoint; hullPoint+=hullIncr) {
      getPerturbedPoint(points->GetPoint(*hullPoint),
			points->GetPoint(*(hullPoint-1)),
			points->GetPoint(*(hullPoint+1)),
			dim,pertDir,perturbed);
      endVerticesAdded.push_back(points->InsertNextPoint(perturbed));
    }
  }

  /* construct the new contour */

  // insert the begining added points
  contour.insert(contour.begin(), beginVerticesAdded.begin(),
		 beginVerticesAdded.end());

  // insert the end vertices
  contour.insert(contour.end(), endVerticesAdded.begin(),
		 endVerticesAdded.end());
}

/**
 * tests for self-intersection, returns the first point in the segment
 * that intersects another segement, or -1 if none do
 */
int selfIntersecting(vector<int> contour, vtkPoints *points, int dim) {
  contour.push_back(contour[0]);
  // test if the edge connecting begin and end points intersects
  // any of the oher edges.
  for(vector<int>::iterator i = contour.begin(); i != contour.end()-1; i++) {
    float begptContour[2], endptContour[2], tmpbegpt[2], tmpendpt[2];
    assignPoint(points->GetPoint(*i), dim, begptContour);
    assignPoint(points->GetPoint(*(i+1)), dim, endptContour);

    // test each edge for intersection with the edge from begin to end
    int c = 0;
    for(vector<int>::iterator j = contour.begin(); j != contour.end()-1; j++,c++) {
      if(i == j) break;

      // get the endpoints of this edge
      assignPoint(points->GetPoint(*j), dim, tmpbegpt);
      assignPoint(points->GetPoint(*(j+1)), dim, tmpendpt);

      // test for intersection, fix it if necessary
      if(segmentsIntersect(begptContour, endptContour, tmpbegpt, tmpendpt)) {
	return c;
      }
    }
  }

  return -1;
}

/**
 * tests for self-intersection, returns the first point in the segment
 * that intersects the begin point to end point contour segment, or -1 if none
 */
int selfIntersectingWhenClosed(vector<int> contour, vtkPoints *points, int dim) {
  // test if the edge connecting begin and end points intersects
  // any of the oher edges.
  float begptContour[2], endptContour[2], tmpbegpt[2], tmpendpt[2];
  assignPoint(points->GetPoint(contour.front()), dim, begptContour);
  assignPoint(points->GetPoint(contour.back()), dim, endptContour);

  // test each edge for intersection with the edge from begin to end
  int c = 0;
  for(vector<int>::iterator i = contour.begin(); i != contour.end()-1; i++,c++) {
    // get the endpoints of this edge
    assignPoint(points->GetPoint(*i), dim, tmpbegpt);
    assignPoint(points->GetPoint(*(i+1)), dim, tmpendpt);

    // test for intersection, fix it if necessary
    if(segmentsIntersect(begptContour, endptContour, tmpbegpt, tmpendpt)) {
      return c;
    }
  }

  return -1;
}

/**
 * fixes self intersection of a contour if the
 */
bool fixSelfIntersection(vector<int> &contour, vtkPoints *points, int dim) {
  // check that the end segment of the closed contour does not
  // intersect the rest of the contour. if it does, add points taken
  // from the convex hull that have been perturbed slightly from the
  // convex hull

  vector<int> tempContour = contour;
  int intersectSegment;
  // unintersect trying the normals in
  if(-1 != (intersectSegment = selfIntersecting(contour,points,dim))) {
    unintersectContour(tempContour,points,dim,IN);
    // if still intersecting try normals out
    if(-1 != (intersectSegment = selfIntersecting(tempContour,points,dim))) {
      tempContour = contour;
      unintersectContour(tempContour,points,dim,OUT);
    }
  }

  contour = tempContour;

  // test if still intersecting
  if(-1 != (intersectSegment = selfIntersecting(contour,points,dim))) {
    fprintf(stderr,"segment %d still intersecting the end contour ");
    return false;
  }

  return true;
}


/**
 * given a set of lines, divide the lines into slices and contours
 * this function also ensures that the contours are non-intersecting
 * and corrects self-intersecting contours by adding vertices if necessary
 */
vector<vector<vector<int> > >
      connectContours(vtkCellArray *lines,
		      vtkPoints *points,
		      vtkPolyData *surf,
		      vector<float*> segmentDirections,
		      int sliceDimension) {
  // allocate slice data
  vector<vector<vector<int> > > sliceData;
  vector<vector<int> > sliceEdges;
  sliceEdges.push_back(*(new vector<int>));

  // segment edges into slices by traversing the line segments and
  // binning by sliceDimension coordinate -- this assumes that sort by
  // value was used when the surface was sliced by vtk
  lines->InitTraversal();
  int nPts,*pts, lineId;
  float *lastp = NULL;

  int travLoc = lines->GetTraversalLocation();
  lineId = lines->GetNextCell(nPts,pts);
  for(int curEdge = 0, curSlice = 0; lineId; curEdge++) {
    sliceEdges[curSlice].push_back(travLoc);

    // get the next point, decide if we've switched slices
    travLoc = lines->GetTraversalLocation();
    lineId = lines->GetNextCell(nPts,pts);
    if(lineId) { // see if we should increase the slice
      float *nextp = points->GetPoint(pts[0]);

      if(lastp && fabs(lastp[sliceDimension] - nextp[sliceDimension]) > FCOMPTOL) {
	sliceEdges.push_back(*(new vector<int>));
	curSlice++;
      }
      lastp = nextp;
    }
  }

  // go through slices one by one connecting the connected components
  // of each slice. after all the edges in a slice have been assigned
  // an order in a contour, check that the contours in the slice are
  // non-intersecting and correct contour intersections if there are any
  for(uint slice = 0; slice < sliceEdges.size(); slice++) {
    fprintf(stderr,"connecting slice %d...",slice);

    sliceData.push_back(*(new vector<vector<int> >));
    // pick a random starting vertex, traverse neighbors on one
    // side. add neighbors on the opposite side in the opposite
    // direction after
    for(uint contour = 0; sliceEdges[slice].size() > 0; contour++) {
      // add all the points for one contour
      sliceData[slice].push_back(*(new vector<int>));

      // add the first edge
      lines->SetTraversalLocation(sliceEdges[slice].back());
      lines->GetNextCell(nPts,pts);
      sliceEdges[slice].pop_back();

      sliceData[slice][contour].push_back(pts[0]);
      sliceData[slice][contour].push_back(pts[1]);

      int begpt = pts[0];
      int endpt = pts[1];

      // find the next line containing the endpt, repeatedly add the
      // segments from the end
      vector<int>::iterator i;
      bool moreSegments = true;
      while(moreSegments) {
	/* find the new endpt */
	moreSegments = false;
	for(i = sliceEdges[slice].end()-1; i != sliceEdges[slice].begin()-1; i--) {
	  lines->SetTraversalLocation(*i);
	  lines->GetNextCell(nPts,pts);

	  if(pts[0] == endpt || pts[1] == endpt) {
	    endpt = pts[0] == endpt ? pts[1] : pts[0];
	    sliceData[slice][contour].push_back(endpt);

	    // update other info
	    moreSegments = true;
	    sliceEdges[slice].erase(i);
	    break;
	  }
	}

      }

      // find the next line containing the begpt, repeatedly add the
      // segments from the begining.
      moreSegments = true;
      while(moreSegments) {
	/* find the new endpt */
	moreSegments = false;
	for(i = sliceEdges[slice].end()-1; i != sliceEdges[slice].begin()-1; i--) {
	  lines->SetTraversalLocation(*i);
	  lines->GetNextCell(nPts,pts);

	  if(pts[0] == begpt || pts[1] == begpt) {
	    begpt = pts[0] == begpt ? pts[1] : pts[0];
	    sliceData[slice][contour].insert(sliceData[slice][contour].begin(),
					     begpt);

	    // update other info
	    moreSegments = true;
	    sliceEdges[slice].erase(i);
	    break;
	  }
	}
      }

      // erase the last point in the contour
      sliceData[slice][contour].erase(sliceData[slice][contour].end()-1);

      // test if we are still yet intersecting
      if(!fixSelfIntersection(sliceData[slice][contour],points,sliceDimension)) {
	fprintf(stderr,"slice %d contour %d\n",slice,contour);
      }
    }
    fprintf(stderr,"done\n");
  }

  return sliceData;
}

/**
 * given a surface, slice it up
 */
vtkPolyData *sliceSurf(vtkPolyData *surf, int numSlices, int dimension, vector<vector<vector<int> > > &slices) {
  // validate input
  if(surf == NULL || numSlices <= 0 || dimension < 0 || dimension > 2) {
    return NULL;
  }

  // tell the surface to compute its bounds
  surf->ComputeBounds();

  // create a slice plane based on the dimension to be cut along
  vtkPlane *slicePlane = vtkPlane::New();

  float normal[3] = {0.0, 0.0, 0.0};  // plane normal
  normal[dimension] = 1.0;
  slicePlane->SetNormal(normal);

  float origin[3]; // plane origin
  surf->GetCenter(origin);
  slicePlane->SetOrigin(origin);

  // get the bounds of the surface in the dimension we are cutting along
  surf->Update();
  float *bounds = surf->GetBounds();
  float min = bounds[2*dimension]+SLICEPERT;
  float max = bounds[2*dimension+1]-SLICEPERT;

  // make a cutter
  vtkSurfCutter *cutEdges = vtkSurfCutter::New();
  cutEdges->SetInput(surf);
  cutEdges->SetCutFunction(slicePlane);
  cutEdges->SetSortBy(1); // sort the resulting edges by slice

  // slice the surface
  float sliceDist = (max-min)/(float)(numSlices-1);
  for(int slice = 0; slice < numSlices; slice++) {
    cutEdges->SetValue(slice,(min+SLICEPERT)+(slice*sliceDist));
  }

  cutEdges->Update();
  vtkPolyData *cutData = cutEdges->GetOutput();

  // get the faces intersected by the slices
  slices = connectContours(cutData->GetLines(),cutData->GetPoints(),surf,
			   cutEdges->GetSegmentDirections(),dimension);

  return cutEdges->GetOutput();
}

void writeNuagesFile(vtkPoints *points,
		     vector<vector<vector<int> > > &slices,
		     int sliceDimension,
		     FILE *fp) {
  // find some normalization constants that center the data and make
  // the slice dimension range from -512 to 512
  points->ComputeBounds();
  float *bounds = points->GetBounds();
  float min = bounds[2*sliceDimension];
  float max = bounds[2*sliceDimension+1];
  float scale = 1024/(max-min);

  // find the offsets to center the data
  float offset[3];
  for(int i = 0; i < 3; i++) {
    offset[i] = scale * (bounds[2*i+1]+bounds[2*i])/2;
  }

  // write the number of slices
  fprintf(fp,"S %d\n",slices.size());

  for(vector<vector<vector<int> > >::iterator i = slices.begin(); i != slices.end(); i++) {
    // calculate the number of vertices in this slice
    int sliceVerts = 0;
    for(vector<vector<int> >::iterator j = (*i).begin(); j != (*i).end(); j++) {
      sliceVerts += (*j).size();
    }

    // print the header for this slice
    float *p = points->GetPoint((*i)[0][0]);
    fprintf(fp, "v %d z %f\n", sliceVerts,
	    //	    p[sliceDimension]);
      	    scale*p[sliceDimension]-offset[sliceDimension]);

    // print each contour
    for(vector<vector<int> >::iterator j = (*i).begin(); j != (*i).end(); j++) {
      fprintf(fp,"{\n");
      // print the points in the contour
      for(vector<int>::iterator k = (*j).begin(); k != (*j).end(); k++) {
	float *p = points->GetPoint(*k);
	// print the coordinates, skipping the slice dimension
	for(int d = 0; d < 3; d++) {
	  if(d == sliceDimension) continue;
	  fprintf(fp,"%f ",
		  //		  p[d]);
		  scale*p[d]-offset[d]);
	}
	fprintf(fp,"\n");
      }
      fprintf(fp,"}\n");
    }
  }
}

/**
 * main function -- requires a filename, number of slices and the
 * dimension along which to slice. prints a nuages formatted file to stdout
 */
int main(int argc, char **argv) {
  if(argc < 4) {
    fprintf(stderr,"usage: surfSlicer filename.obj n d\n exit");
    exit(0);
  }

  vtkOBJReader *surfSource = vtkOBJReader::New();
  surfSource->SetFileName(argv[1]);

  vector<vector<vector<int> > > sliceIndices;
  vtkPolyData *slices = sliceSurf(surfSource->GetOutput(),
				  atoi(argv[2]),
				  atoi(argv[3]),
				  sliceIndices);

  // write a nuages file
  writeNuagesFile(slices->GetPoints(), sliceIndices, atoi(argv[3]), stdout);

  return 0;
}
