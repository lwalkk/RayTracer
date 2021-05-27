// rtWindow.h


#ifndef RTWINDOW_H
#define RTWINDOW_H

class RTwindow;


#include "main.h"
#include "arcball.h"
#include "scene.h"


class RTwindow {

  Scene *scene;

 public:

  GLFWwindow *window;
  Arcball *arcball;
  float    fovy;
  bool     redisplay;
  bool     viewpointChanged;

  bool     arcballActive;
  bool     dragging;
  bool     draggingWasDone;

  RTwindow( int x, int y, int width, int height, const char *title, Scene *s, GLFWwindow *w );

  void userDraw( mat4 &WCS_to_VCS, mat4 &VCS_to_CCS );
  void raytrace( mat4 &MVP );
  void readView();
  void writeView();

  void mouseButtonCallback( GLFWwindow* window, int button, int action, int keyModifiers );
  void mouseMovementCallback( GLFWwindow* window, double xpos, double ypos );
  void mouseScrollCallback( GLFWwindow* window, double xoffset, double yoffset );
  void mouseClick( vec3 mouse, int button, int keyModifiers );
  void keyCallback( GLFWwindow* window, int key, int scancode, int action, int mods );
};


#endif
