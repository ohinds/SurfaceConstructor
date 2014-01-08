/*****************************************************************************
 * surfCon.h is the main header file for an application to
 * reconstruct surfaces from serial sections
 * Oliver Hinds <oph@cns.bu.edu> 2004-02-04
 *
 *
 *
 *****************************************************************************/

#ifndef SURF_RECON_H
#define SURF_RECON_H

#define SURF_RECON_VERSION_H "$Id: surfCon.h,v 1.10 2007/05/22 19:18:00 oph Exp $"

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include<time.h>

#include"colors.extern"
#include"surfConTypes.h"
#include"surfConUtil.h"
#include"align.h"
#include"align.extern"
#include"contour.h"
#include"contour.extern"
#include"dataset.h"

/* libsr */
//#include"nuages.h"
#include"correspondence.h"
#include"tile.h"
#include"surfUtil.h"

#ifndef MAC
#include<GL/glut.h>
#include"GL/glext.h"
#else
#include<GLUT/glut.h>
#endif

#ifndef CALLBACK
#define CALLBACK
#endif

/******************************************************
 * getopt declarations
 ******************************************************/

/* argument parsing stuff */

#include <unistd.h>

int getopt(int argc, char * const argv[],
           const char *optstring);

extern char *optarg;
extern int optind, opterr, optopt;

#define _GNU_SOURCE
#include <getopt.h>

int getopt_long(int argc, char * const argv[],
                const char *optstring,
                const struct option *longopts, int *longindex);

/******************************************************
 * functions
 ******************************************************/

/** creation functions **/

/**
 * initializes the application
 */
void init(int argc, char **argv);

/**
 * initializes dataset
 */
void datasetInit();

/**
 * initialization of a mode
 */
void modeInit();

/**
 * uninitialization of a mode
 */
void modeUninit();

/**
 * cleans up, closes files, etc...
 */
void destroy(void);

/**
 * checks if the display should update
 */
void idle(void);


/** display related functions **/

/**
 * handles the user resizing the window
 */
void reshape(int w, int h);

/**
 * handles the update of the display screen
 */
void draw(void);

/**
 * post a buffer swap event
 */
void swapBuffers();

/**
 * post a display event
 */
void redisplay();


/** event handlers **/

/**
 * blocks forr a key press
 * NOTE: this will peg your cpu in current implementation
 */
char blockForKeyPress();

/**
 * surf menu creation
 */
void createSurfMenu();

/**
 * surf actions
 */
void surfAction(int action);

/**
 * keyboard handler
 */
void keyboard(unsigned char key, int x, int y);

/**
 * mouse button handler
 */
void mouse(int button, int state, int x, int y);

/**
 * mouse motion handler
 */
void mouseMotion(int x, int y);

/**
 * error handler
 */
void CALLBACK error(GLenum errCode);


/** utility functions **/

/**
 * change the current slice number
 */
void changeSlice(int dir);

/**
 * gets the display width
 */
int getDisplayWidth();

/**
 * gets the display height
 */
int getDisplayHeight();

/**
 * draws a black box that will enclose a string of text
 */
void drawBlackBoxForString(char *str, GLint x, GLint y);

/**
 * draws a string at the specified coordinates
 */
void drawString(GLint x, GLint y, char* s, const GLfloat *color);

/**
 * parse the command line args
 */
void parseArgs(int argc, char** argv);

/**
 * prints usage and command line options / flags
 */
void printUsage(void);

/**
 * gets a string that shows the version of the files used
 */
char *getSurfReconVersionString();

/**
 * gets a string that shows user and time info
 */
char *getUserString();

/**
 * validate the input arguments
 */
int validateArgs();

/**
 * main function to control the stimulator
 */
int main(int argc, char **argv);

#endif
