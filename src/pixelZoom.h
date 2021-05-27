// pixelZoom.h
//
// show zoomed view of pixels at a position in the window


#ifndef PIXEL_ZOOM_H
#define PIXEL_ZOOM_H

#include "gpuProgram.h"
#include "linalg.h"


class PixelZoom {

  GPUProgram program;
  static const char *vertShader;
  static const char *fragShader;

 public:

  PixelZoom();

  void zoom( GLFWwindow *window, vec2 mouse, vec2 windowDim );
};

#endif
