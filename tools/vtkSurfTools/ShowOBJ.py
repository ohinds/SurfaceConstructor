#!/usr/bin/env python

# ShowOBJ.py reads an obj surface file and displays it interactively
#
# Oliver Hinds <oph@bu.edu>
# 2005-03-18

import vtk, sys


# turns wireframe view on
def SetRepresentation(rep):
    actors = ren.GetActors()
    actors.InitTraversal()
    actor = actors.GetNextItem()
    while actor:
        if rep == "wireframe":
            actor.GetProperty().SetRepresentationToWireframe()
        elif rep == "surface":
            actor.GetProperty().SetRepresentationToSurface()
        elif rep == "points":
            actor.GetProperty().SetRepresentationToPoints()
        actor = actors.GetNextItem()

    renWin.Render()

# turn edge viewing on
def EdgeView(onoff):
    actors = ren.GetActors()
    actors.InitTraversal()
    actor = actors.GetNextItem()
    while actor:
        actor.GetProperty().SetEdgeVisibility(onoff)
        actor = actors.GetNextItem()

    renWin.Render() 

# function to handle the key presses in the window
def Keyboard(obj, event):

    key = obj.GetKeySym()

    # representation switching
    if key == "w":
        SetRepresentation("wireframe")
    elif key == "s":
        SetRepresentation("surface")
    elif key == "p":
        SetRepresentation("points")
    elif key == "l":
        EdgeView(1)
    elif key == "L":
        EdgeView(0)

## begin main 

# check that the required number of parameters were passed
if len(sys.argv) < 2:
    print "usage: ShowOBJ.py <OBJ surface file>"
    sys.exit(1)

# store the argument
OBJFile = sys.argv[1]

# reader for the OBJ file
surfRead = vtk.vtkOBJReader()
surfRead.SetFileName(OBJFile)

surf = surfRead.GetOutput()

## rendering 

# window stuff
ren = vtk.vtkRenderer()
renWin = vtk.vtkRenderWindow()
renWin.AddRenderer(ren)
iren = vtk.vtkRenderWindowInteractor()
iren.SetInteractorStyle(vtk.vtkInteractorStyleTrackballCamera())
iren.SetRenderWindow(renWin)

# surface display attr
surfMapper = vtk.vtkPolyDataMapper()
surfMapper.SetInput(surf)
surfMapper.ScalarVisibilityOff()
surfActor = vtk.vtkActor()
surfActor.SetMapper(surfMapper)
surfActor.GetProperty().SetColor(0.5, 0.5, 0.5)
surfActor.GetProperty().SetDiffuseColor(1, .49, .25)
surfActor.GetProperty().SetSpecular(.3)
surfActor.GetProperty().SetSpecularPower(50)

surfActor.GetProperty().EdgeVisibilityOn()
surfActor.GetProperty().SetEdgeColor(0.75,1,1)
surfActor.GetProperty().SetLineWidth(2)

surfActor.GetProperty().SetPointSize(3)

# add the stuff to be displayed
ren.AddActor(surfActor)
ren.SetBackground(0, 0, 0)
renWin.SetSize(800, 800)

# set to handle kill
def CheckAbort(obj, event):
    if obj.GetEventPending() != 0:
        obj.SetAbortRender(1)
 
renWin.AddObserver("AbortCheckEvent", CheckAbort)

# start displaying
iren.AddObserver("KeyPressEvent", Keyboard)
iren.Initialize()
renWin.Render()
iren.Start()

