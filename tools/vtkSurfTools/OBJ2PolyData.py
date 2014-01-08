#!/usr/bin/env python

# OBJ2PolyData.py converts a surface in OBJ format to vtkPolyData and
# saves it to a file
#
# Oliver Hinds <oph@bu.edu>
# 2004-12-02

import vtk, sys

## begin main 

# check that the required number of parameters were passed
if len(sys.argv) < 3:
    print "usage: OBJ2PloyData.py <OBJ surface file> <vtkPolyData file>"
    sys.exit(1)

# store the arguments
OBJFile = sys.argv[1]
vtkFile = sys.argv[2]

# reader for the OBJ file
surfRead = vtk.vtkOBJReader()
surfRead.SetFileName(OBJFile)

surf = surfRead.GetOutput()

surfWrite = vtk.vtkPolyDataWriter()
surfWrite.SetFileName(vtkFile)
surfWrite.SetInput(surf)
surfWrite.SetFileTypeToBinary()
surfWrite.Write()

## end main

