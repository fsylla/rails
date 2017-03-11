
/* ____________________________________________________________________________
 *
 * file:        video.h
 * created:     Fri Feb 12 19:21:11 2016
 * by:          frank
 *
 * description: OpenGL setup
 * ____________________________________________________________________________
 */


#ifndef VIDEO_H
#define VIDEO_H


#include <GL/gl.h>
#include <GL/glu.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

/* Setup useful booleans */
#define TRUE                    1
#define FALSE                   0

/* screen width, height, and bit depth */
#define SCREEN_WIDTH         1200
#define SCREEN_HEIGHT         600
#define SCREEN_BPP             16

/* This is our SDL surface */
extern SDL_Surface *surface;

/* Release resources and quit  */
void videoQuit(int returnCode);

/* Loads in a bitmap as a GL texture */
int videoLoadGLTextures(const char *bmpPath, GLuint *texturePtr);

/* Loads in a jpeg file as a GL texture */
int videoLoadGLTexturesJpg(const char *jpgPath, GLuint *texturePtr);

/* function to reset our viewport after a window resize */
int videoResizeWindow(int width, int height);

/* OpenGL initialization function */
int videoInitGL();

int videoInit();


#endif

