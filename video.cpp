
/* ____________________________________________________________________________
 *
 * file:        video.cpp
 * created:     Fri Feb 12 18:52:28 2016
 * by:          frank
 *
 * description: OpenGL setup
 * ____________________________________________________________________________
 */


#include "video.h"


/* This is our SDL surface */
SDL_Surface *surface;

/* Release resources and quit  */
void videoQuit(int returnCode) {
    SDL_Quit();
    exit(returnCode);
}

/* Loads in a bitmap as a GL texture */
int videoLoadGLTextures(const char *bmpPath, GLuint *texturePtr) {
    int Status = FALSE;

    /* Create storage space for the texture */
    SDL_Surface *TextureImage;

    /* Load The Bitmap into Memory */
    if ((TextureImage = SDL_LoadBMP(bmpPath))) {
        Status = TRUE;
        glGenTextures(1, texturePtr);
        glBindTexture(GL_TEXTURE_2D, *texturePtr);

        glTexImage2D(GL_TEXTURE_2D, 0, 3, TextureImage->w,
                     TextureImage->h, 0, GL_BGR,
                     GL_UNSIGNED_BYTE, TextureImage->pixels);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }

    /* Free up some memory */
    if (TextureImage) {
        SDL_FreeSurface(TextureImage);
    }

    return Status;
}

/* Loads in a jpeg file as a GL texture */
int videoLoadGLTexturesJpg(const char *jpgPath, GLuint *texturePtr) {
    int Status = FALSE;

    /* Create storage space for the texture */
    SDL_Surface *TextureImage;

    /* Load The jpeg file into Memory */
    if ((TextureImage = IMG_Load(jpgPath))) {
        Status = TRUE;
        glGenTextures(1, texturePtr);
        glBindTexture(GL_TEXTURE_2D, *texturePtr);

        glTexImage2D(GL_TEXTURE_2D, 0, 3, TextureImage->w,
                     TextureImage->h, 0, GL_BGR,
                     GL_UNSIGNED_BYTE, TextureImage->pixels);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }

    /* Free up some memory */
    if (TextureImage) {
        SDL_FreeSurface(TextureImage);
    }

    return Status;
}

/* function to reset our viewport after a window resize */
int videoResizeWindow(int width, int height) {
    /* Height / width ration */
    GLfloat ratio;

    /* Protect against a divide by zero */
    if (height == 0) {
        height = 1;
    }

    ratio = (GLfloat) width / (GLfloat) height;

    /* Setup our viewport. */
    glViewport(0, 0, (GLint) width, (GLint) height);

    /*
     * change to the projection matrix and set
     * our viewing volume.
     */

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    /* Set our perspective */
    gluPerspective(45.0f, ratio, 0.1f, 1000.0f);

    /* Make sure we're chaning the model view and not the projection */
    glMatrixMode(GL_MODELVIEW);

    /* Reset The View */
    glLoadIdentity();

    return(TRUE);
}


/* OpenGL initialization function */
int videoInitGL()
{
    /* Enable Texture Mapping ( NEW ) */
    glEnable(GL_TEXTURE_2D);

    /* Enable smooth shading */
    glShadeModel(GL_SMOOTH);

    /* Set the background black */
    glClearColor(0.0f, 0.0f, 0.0f, 0.5f);

    /* Depth buffer setup */
    glClearDepth(1.0f);

    /* Enables Depth Testing */
    glEnable(GL_DEPTH_TEST);

    /* The Type Of Depth Test To Do */
    glDepthFunc(GL_LEQUAL);

    /* Really Nice Perspective Calculations */
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    return(TRUE);
}


int videoInit()
{
    /* Flags to pass to SDL_SetVideoMode */
    int videoFlags;

    /* this holds some info about our display */
    const SDL_VideoInfo *videoInfo;

    /* whether or not the window is active */
    int isActive = TRUE;

    /* initialize SDL */
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "Video initialization failed: %sn", SDL_GetError());
        videoQuit(1);
    }

    /* Fetch the video info */
    videoInfo = SDL_GetVideoInfo();

    if (!videoInfo) {
        fprintf(stderr, "Video query failed: %sn", SDL_GetError());
        videoQuit(1);
    }

    /* the flags to pass to SDL_SetVideoMode */
    videoFlags  = SDL_OPENGL;          /* Enable OpenGL in SDL */
    videoFlags |= SDL_GL_DOUBLEBUFFER; /* Enable double buffering */
    videoFlags |= SDL_HWPALETTE;       /* Store the palette in hardware */
    videoFlags |= SDL_RESIZABLE;       /* Enable window resizing */

    /* This checks to see if surfaces can be stored in memory */
    if (videoInfo->hw_available) {
        videoFlags |= SDL_HWSURFACE;
    } else {
        videoFlags |= SDL_SWSURFACE;
    }

    /* This checks if hardware blits can be done */
    if (videoInfo->blit_hw) {
        videoFlags |= SDL_HWACCEL;
    }

    /* Sets up OpenGL double buffering */
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    /* get a SDL surface */
    surface = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, videoFlags);

    /* Verify there is a surface */
    if (!surface) {
        fprintf(stderr, "Video mode set failed: %sn", SDL_GetError());
        videoQuit(1);
    }

    /* initialize OpenGL */
    videoInitGL();

    /* resize the initial window */
    videoResizeWindow(SCREEN_WIDTH, SCREEN_HEIGHT);

    return(videoFlags);
}

