
/* ____________________________________________________________________________
 *
 * file:        rails_test.cpp
 * created:     Mo 23. Jan 17:51:52 CET 2017
 * by:          frank
 *
 * description: simple visualization for railroad simulation
 * ____________________________________________________________________________
 */


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include "SDL/SDL.h"

#include "matrix.h"
#include "model.h"
#include "nodemap.h"
#include "train.h"
#include "video.h"


#define DEG2RAD     M_PI / 180
#define BASE_RADIUS 0.2f

#define CAM_VMAX    1.0f
#define CAM_VSENS   0.01f

#define SIZE_TRAINS 4


/* Used for camera position */
GLfloat cameraVX        = 0;
GLfloat cameraVY        = 0;
GLfloat cameraVZ        = 0;

GLfloat baseX           = - MODEL_WIDTH / 2;
GLfloat baseY           = - MODEL_HEIGHT / 2;

GLfloat destX           = 0;
GLfloat destY           = 0;

GLfloat cameraZ         = -20.0f;
//GLfloat mouseZ = 724.26f;     // 300 / tan 22.5 magic !!!
GLfloat mouseZ          = 927.05;

/* Used for camera rotation */
GLfloat cameraRVX       = 0;
GLfloat cameraRVZ       = 0;

GLfloat cameraRX        = 0;
GLfloat cameraRZ        = 0;

SDLKey gMod             = SDLK_UNKNOWN;
SDLKey gSym             = SDLK_UNKNOWN;

/* Storage For Textures */
GLuint texture[12];

int     gNodeId = 1;            // rails hack !!!

NodeMap*    nodeMap;
Train*      trains[SIZE_TRAINS];


void dump() {
    printf("dump: \n");
    printf("rx: %9.3f\n", cameraRX);
    printf("rz: %9.3f\n", cameraRZ);
    printf("\n");
    printf("tx: %9.3f\n", baseX);
    printf("ty: %9.3f\n", baseY);
    printf("tz: %9.3f\n", cameraZ);
    printf("\n");
    printf("sh: %9d\n", surface->h);
    printf("sw: %9d\n", surface->w);
    printf("\n");
}


int convertMouse2Plane(GLfloat mouseX, GLfloat mouseY, GLfloat *planeX, GLfloat *planeY) {
    GLfloat m0[3][3];
    GLfloat m1[3][3];
    GLfloat m2[3][3];

    GLfloat d0;
    GLfloat d1;
    GLfloat d2;

    int rc = 0;

    mouseZ = 1.207 * surface->h;    // 1 / 2 * tan 22.5 = 1.207

    GLfloat cosRX = cos(DEG2RAD * cameraRX);
    GLfloat sinRX = sin(DEG2RAD * cameraRX);

    GLfloat cosRZ = cos(DEG2RAD * cameraRZ);
    GLfloat sinRZ = sin(DEG2RAD * cameraRZ);

    m0[0][0] = mouseX;
    m0[1][0] = mouseY;
    m0[2][0] = mouseZ;

    m0[0][1] = -cosRZ;
    m0[1][1] = -sinRZ * cosRX;
    m0[2][1] = -sinRZ * sinRX;

    m0[0][2] =  sinRZ;
    m0[1][2] = -cosRZ * cosRX;
    m0[2][2] = -cosRZ * sinRX;


    m1[0][0] = mouseX;
    m1[1][0] = mouseY;
    m1[2][0] = mouseZ;

    m1[0][1] = 0;
    m1[1][1] = 0;
    m1[2][1] = cameraZ;

    m1[0][2] =  sinRZ;
    m1[1][2] = -cosRZ * cosRX;
    m1[2][2] = -cosRZ * sinRX;


    m2[0][0] = mouseX;
    m2[1][0] = mouseY;
    m2[2][0] = mouseZ;

    m2[0][1] = -cosRZ;
    m2[1][1] = -sinRZ * cosRX;
    m2[2][1] = -sinRZ * sinRX;

    m2[0][2] = 0;
    m2[1][2] = 0;
    m2[2][2] = cameraZ;

    d0 = determinant(m0);

//    printf("d0 = %f\n", d0);

    if (d0 != 0) {
        d1 = determinant(m1);
        d2 = determinant(m2);

//  printf("d1 = %f\n", d1);
//  printf("d2 = %f\n", d2);

        *planeX = d1 / d0;
        *planeY = d2 / d0;
    } else {
        rc = -1;
    }

    return(rc);
}


void handleKeyDown(SDL_keysym *keysym)
{
    switch (keysym->sym)
    {
    case SDLK_ESCAPE:
        videoQuit(0);
        break;

    case SDLK_F1:
        SDL_WM_ToggleFullScreen(surface);
        break;

    case SDLK_F2:
        dump();
        break;

    case SDLK_DOWN:
        gSym = SDLK_DOWN;
        break;

    case SDLK_LEFT:
        gSym = SDLK_LEFT;
        break;

    case SDLK_LSHIFT:
        gMod = SDLK_LSHIFT;
        break;

    case SDLK_PAGEDOWN:
        gSym = SDLK_PAGEDOWN;
        break;

    case SDLK_PAGEUP:
        gSym = SDLK_PAGEUP;
        break;

    case SDLK_RIGHT:
        gSym = SDLK_RIGHT;
        break;

    case SDLK_UP:
        gSym = SDLK_UP;
        break;

    default:
        fprintf(stderr, "handleKeyDown: %d ignored\n", keysym->sym);
        break;

    }

    return;
}


void handleKeyUp(SDL_keysym *keysym)
{
    switch (keysym->sym) {
    case SDLK_LSHIFT:
        gMod = SDLK_UNKNOWN;
        break;

    default:
        gSym = SDLK_UNKNOWN;
        break;

    }

    cameraRVX = 0;
    cameraRVZ = 0;
    cameraVX = 0;
    cameraVY = 0;
    cameraVZ = 0;
}


void handleMouseButtonDown(SDL_Event event)
{
    GLfloat  x, y;
    GLfloat planeX, planeY;

    switch (event.button.button) {
    case SDL_BUTTON_LEFT:
        x = (GLfloat) (surface->w / 2 - event.button.x);
        y = (GLfloat) (event.button.y - surface->h / 2);
        convertMouse2Plane(x, y, &planeX, &planeY);
        printf("%5d;%8.3f;%8.3f\n", gNodeId++, planeX - baseX, planeY - baseY);
        break;

    case SDL_BUTTON_RIGHT:
        x = (GLfloat) (surface->w / 2 - event.button.x);
        y = (GLfloat) (event.button.y - surface->h / 2);
        printf("handleMouseButtonDown: x = %f, y = %f\n", x, y);
        convertMouse2Plane(x, y, &planeX, &planeY);
        printf("handleMouseButtonDown: xp = %f, yp = %f\n", planeX, planeY);

        destX = planeX;
        destY = planeY;
        break;

    default:
        printf("handleMouseButtonDown: button %d\n", event.button.button);

    }
}


void handleMouseButtonUp(SDL_Event event)
{
//    printf("handleMouseButtonUp: button %d\n", event.button.button);
}


void handleMouseMotion(SDL_Event event)
{
    int x, y;
    int mst = SDL_GetMouseState(&x, &y);

    switch (mst) {
    case 4:             // SDL_BUTTON_RIGHT pressed
        x = x - surface->w / 2;
        y = y - surface->h / 2;
        printf("handleMouseMotion: x = %d, y = %d\n", x, y);
        break;

    }
}


void drawBase(GLfloat x, GLfloat y)
{
    glBindTexture(GL_TEXTURE_2D, texture[11]);
    glBegin(GL_QUADS);

    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(x - BASE_RADIUS, y - BASE_RADIUS, 0.01f);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(x - BASE_RADIUS, y + BASE_RADIUS, 0.01f);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(x + BASE_RADIUS, y + BASE_RADIUS, 0.01f);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(x + BASE_RADIUS, y - BASE_RADIUS, 0.01f);

    glEnd();
}


void drawModel()
{
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    glBegin(GL_QUADS);

    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(0.0f, MODEL_HEIGHT, 0.0f);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(MODEL_WIDTH, MODEL_HEIGHT, 0.0f);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(MODEL_WIDTH, 0.0f, 0.0f);

    glEnd();
}


void drawTexture(GLfloat x, GLfloat y, GLfloat xr, GLfloat yr, int tid)
{
    glBindTexture(GL_TEXTURE_2D, texture[tid]);
    glBegin(GL_QUADS);

    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(x - xr, y - yr, 0.01f);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(x - xr, y + yr, 0.01f);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(x + xr, y + yr, 0.01f);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(x + xr, y - yr, 0.01f);

    glEnd();
}


/* Here goes our drawing code */
int drawGLScene()
{
    uint16_t    id;
    float   x;
    float   y;

    /* Clear The Screen And The Depth Buffer */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glLoadIdentity();
    glTranslatef(0.0f, 0.0f, cameraZ);

    glRotatef(cameraRX, 1.0f, 0.0f, 0.0f); /* Rotate On The X Axis */
    glRotatef(cameraRZ, 0.0f, 0.0f, 1.0f); /* Rotate On The Z Axis */

    drawBase(destX, destY);
    glTranslatef(baseX, baseY, 0.0f);
    /*
        drawTexture(trainX, trainY, BASE_RADIUS, BASE_RADIUS, 1);
     */

    for (int i = 0; i < SIZE_TRAINS; ++i) {
        id = trains[i]->getHead();
        x = nodeMap->nodeGetX(id);
        y = nodeMap->nodeGetY(id);

        drawTexture(x, y, BASE_RADIUS, BASE_RADIUS, i + 1);
    }

    drawModel();

    /* Draw it to the screen */
    SDL_GL_SwapBuffers();

    return(TRUE);
}


GLfloat velCamera(GLfloat *vel)
{
    *vel += CAM_VSENS * (CAM_VMAX - *vel);

    return(*vel);
}


void moveBase()
{
    switch (gSym) {
    case SDLK_DOWN:
        baseY += velCamera(&cameraVY);
        break;

    case SDLK_LEFT:
        baseX += velCamera(&cameraVX);
        break;

    case SDLK_RIGHT:
        baseX -= velCamera(&cameraVX);
        break;

    case SDLK_UP:
        baseY -= velCamera(&cameraVY);
        break;

    }
}


void moveCamera()
{
    switch (gSym) {
    case SDLK_DOWN:
        cameraRX -= velCamera(&cameraRVX);

        if (cameraRX < -90.0f) {
            cameraRX = -90.0f;
        }

        break;

    case SDLK_LEFT:
        cameraRZ += velCamera(&cameraRVZ);

        if (cameraRZ > 180.0f) {
            cameraRZ = cameraRZ - 360.0f;
        }

        break;

    case SDLK_PAGEDOWN:
        cameraZ -= velCamera(&cameraVZ);

        if (cameraZ < -99.0f) {
            cameraZ = -99.0f;
        }

        break;

    case SDLK_PAGEUP:
        cameraZ += velCamera(&cameraVZ);

        if (cameraZ > -0.1f) {
            cameraZ = -0.1f;
        }

        break;

    case SDLK_RIGHT:
        cameraRZ -= velCamera(&cameraRVZ);

        if (cameraRZ <= -180.0f) {
            cameraRZ = cameraRZ + 360.0f;
        }

        break;

    case SDLK_UP:
        cameraRX += velCamera(&cameraRVX);

        if (cameraRX > 0.0f) {
            cameraRX = 0.0f;
        }

        break;

    }
}


void update()
{
    uint16_t    dst, headCur, posNxt;

    for (int i = 0; i < SIZE_TRAINS; ++i) {
        dst = trains[i]->getDest();
        headCur = trains[i]->getHead();

        if (headCur == dst) {
            switch (dst) {
                case 57:
                    trains[i]->setDest(83);
                    break;
    
                case 58:
                    trains[i]->setDest(81);
                    break;
    
                case 81:
                    trains[i]->setDest(58);
                    break;
    
                case 83:
                    trains[i]->setDest(57);
                    break;
    
            }
        } else {
            nodeMap->trainRun(trains[i]);
        }
    }
}


int main(int argc, char **argv)
{
    int     i;
    int     videoFlags;
    char    path[80];
    time_t  tCur        = time(NULL);

    /* main loop variable */
    int     done        = FALSE;

    /* used to collect events */
    SDL_Event   event;

    /* whether or not the window is active */
    int     isActive    = TRUE;

    nodeMap = new NodeMap();
    nodeMap->nodesLoad("nodes.txt");
    nodeMap->hopsLoad("hops.txt");

    trains[0]           = new Train(1, 70, 71);
    trains[1]           = new Train(2, 75, 76);
    trains[2]           = new Train(3, 77, 78);
    trains[3]           = new Train(4, 79, 80);

    trains[0]->setDest(57);
    trains[1]->setDest(58);
    trains[2]->setDest(81);
    trains[3]->setDest(83);

    for (int i = 0; i < SIZE_TRAINS; ++i) {
        nodeMap->trainAdd(trains[i]);
        trains[i]->dump();
    }


    /* initialize Video */
    videoFlags = videoInit();

    /* Load in the textures */
    for (i = 0; i <= 9; ++i) {
        sprintf(path, "data/number%d.bmp", i);

        if (!videoLoadGLTextures(path, &texture[i])) {
            return(FALSE);
        }
    }

    if (!videoLoadGLTexturesJpg("data/rails000.jpg", &texture[10])) {
        return(FALSE);
    }

    if (!videoLoadGLTextures("data/Kompass.bmp", &texture[11])) {
        return(FALSE);
    }

    tCur        = time(NULL);

    /* wait for events */
    while (!done) {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_ACTIVEEVENT:
                if (event.active.gain == 0) {
                    isActive = FALSE;
                } else {
                    isActive = TRUE;
                }
                break;

            case SDL_VIDEORESIZE:
                surface = SDL_SetVideoMode(event.resize.w,
                                           event.resize.h, 16, videoFlags);

                if (!surface) {
                    fprintf(stderr, "Could not get a surface after resize: %sn", SDL_GetError());
                    videoQuit(1);
                }

                videoResizeWindow(event.resize.w, event.resize.h);
                break;

            case SDL_KEYDOWN:
                handleKeyDown(&event.key.keysym);
                break;

            case SDL_KEYUP:
                handleKeyUp(&event.key.keysym);
                break;

            case SDL_MOUSEMOTION:
                handleMouseMotion(event);
                break;

            case SDL_MOUSEBUTTONDOWN:
                handleMouseButtonDown(event);
                break;

            case SDL_MOUSEBUTTONUP:
                handleMouseButtonUp(event);
                break;

            case SDL_QUIT:
                done = TRUE;
                break;

            case SDL_VIDEOEXPOSE:
                break;

            default:
                fprintf(stderr, "unknown event type: %d\n", event.type);
                break;

            }
        }

        if (isActive) {
            if (gMod == SDLK_LSHIFT) {
                moveCamera();
            } else {
                moveBase();
            }

            if (fabs(destX) > 0.01f || fabs(destY) > 0.01f) {
                baseX = baseX - 0.1f * destX;
                baseY = baseY - 0.1f * destY;

                destX = 0.9f * destX;
                destY = 0.9f * destY;
            }

            if (time(NULL) > tCur) {
                update();
                ++tCur;
            }

            drawGLScene();
        }
    }

    videoQuit(0);

    /* Should never get here */
    return(0);
}

