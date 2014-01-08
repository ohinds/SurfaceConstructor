#!/usr/bin/env python

# PNM2StructuredPoints.py converts a directory of pnm (ppm) images to
# a vtk volume in the vtkStructuredPoints format
#
# Oliver Hinds <oph@bu.edu>
# 2004-12-02

import vtk, sys, string

## begin main 

# check that the required number of parameters were passed
if len(sys.argv) < 6:
    print "usage: PNM2StructuredPoints.py <PNM basename> <image width> <image height> <number of images> <vtkStructuredPoints file>"
    sys.exit(1)

# get the args
PNMbase = sys.argv[1]
imgW = string.atoi(sys.argv[2])
imgH = string.atoi(sys.argv[3])
numImg = string.atoi(sys.argv[4])
vtkFile = sys.argv[5]

# reader for slices that make up the volume
volread = vtk.vtkPNMReader()
volread.SetDataExtent([1, imgW, 1, imgH, 1, numImg])
volread.SetDataByteOrderToLittleEndian()
volread.SetFilePrefix(PNMbase)

# the actual volume
vol = volread.GetOutput()

# writer to write out the slices
volwrite = vtk.vtkStructuredPointsWriter()
volwrite.SetFileName(vtkFile)
volwrite.SetInput(vol)
volwrite.SetFileTypeToBinary()
volwrite.Write()


