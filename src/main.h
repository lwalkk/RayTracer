/* main.h
 */


#ifndef MAIN_H
#define MAIN_H


#include "seq.h"
#include "scene.h"
#include "rtWindow.h"
#include "gpuProgram.h"
#include "pixelZoom.h"


void skipComments( istream &in );
extern int lineNum;
extern Scene *scene;
extern RTwindow *win;
extern PixelZoom *pixelZoom;

extern int windowWidth;
extern int windowHeight;

#endif
