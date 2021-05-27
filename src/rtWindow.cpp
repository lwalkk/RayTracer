// rtWindow.cpp



#include <fstream>
#include "rtWindow.h"


RTwindow::RTwindow( int x, int y, int width, int height, const char *title, Scene *s, GLFWwindow *w )

{
  window = w;

  glfwSetWindowUserPointer( window, this );
  
  // Key handler

  auto static_keyCallback = [](GLFWwindow* ww, int key, int scancode, int action, int mods ) {
    static_cast<RTwindow*>(glfwGetWindowUserPointer(ww))->keyCallback( ww, key, scancode, action, mods );
  };
 
  glfwSetKeyCallback( window, static_keyCallback );

  // Mouse callbacks

  auto static_mouseButtonCallback = [](GLFWwindow* ww, int button, int action, int keyModifiers ) {
    static_cast<RTwindow*>(glfwGetWindowUserPointer(ww))->mouseButtonCallback( ww, button, action, keyModifiers );
  };
 
  glfwSetMouseButtonCallback( window, static_mouseButtonCallback );

  auto static_mouseScrollCallback = [](GLFWwindow* ww, double xoffset, double yoffset ) {
    static_cast<RTwindow*>(glfwGetWindowUserPointer(ww))->mouseScrollCallback( ww, xoffset, yoffset );
  };
 
  glfwSetScrollCallback( window, static_mouseScrollCallback );

  // Set up arcball interface

  arcball = new Arcball( window );
  readView();

  // Misc
  
  scene = s;
  window = w;
  viewpointChanged = true;
  redisplay = true;
  arcballActive = false;
  dragging = false;
}



void RTwindow::keyCallback( GLFWwindow* window, int key, int scancode, int action, int mods )

{
  if (action == GLFW_PRESS)
    switch (key) {

    case GLFW_KEY_DELETE:
      scene->storedRays.clear();
      scene->storedRayColours.clear();
      break;
    case GLFW_KEY_ESCAPE:
      exit(0);
    case 'E':
      scene->outputEye();
      break;
    case '+':
    case '=':
      scene->maxDepth++;
      viewpointChanged = true;
      redisplay = true;
      break;
    case '-':
    case '_':
      if (scene->maxDepth > 1) {
	scene->maxDepth--;
	viewpointChanged = true;
	redisplay = true;
      }
      break;
    case 'P':
      if (mods & GLFW_MOD_SHIFT)
	scene->numPixelSamples++;
      else {
	scene->numPixelSamples--;
	if (scene->numPixelSamples < 1)
	  scene->numPixelSamples = 1;
      }
      redisplay = true;
      cout << "pixel sampling " <<  scene->numPixelSamples << " x " <<  scene->numPixelSamples << endl;
      break;
    case 'A':
      scene->showAxes = !scene->showAxes;
      redisplay = true;
      break;
    case 'J':
      scene->jitter = !scene->jitter;
      redisplay = true;
      cout << "jittering " << (scene->jitter ? "on" : "off") << endl;
      break;
    case '/':
      cout
	<< endl
	<< "+     increase ray depth" << endl
	<< "-     decrease ray depth" << endl
	<< "P     increase pixel sampling" << endl
	<< "p     decrease pixel sampling" << endl
	<< "j     toggle pixel sample jittering" << endl
	<< "a     show/hide axes" << endl
	<< "e     output eye position" << endl
	<< "DEL   delete debugging rays" << endl
	<< "ESC   exit" << endl
	<< endl
	<< "left mouse click             - show rayracing rays from this pixel" << endl
	<< "SHIFT left mouse click       - set debug flag for only this pixel" << endl
	<< "CTRL  left mouse click/drag  - expand pixels at mouse" << endl
	<< "right mouse drag             - rotate viewpoint" << endl
	<< "mouse scroll wheel           - zoom viewpoint" << endl;

      break;
    }
}


void RTwindow::mouseButtonCallback( GLFWwindow* window, int button, int action, int keyModifiers )

{
  // Get mouse position
  
  double xpos, ypos;
  glfwGetCursorPos( window, &xpos, &ypos );

  if (action == GLFW_PRESS) {    // Handle mouse press

    // Start tracking the mouse

    auto static_mouseMovementCallback = [](GLFWwindow* window, double xpos, double ypos ) {
      static_cast<RTwindow*>(glfwGetWindowUserPointer(window))->mouseMovementCallback( window, xpos, ypos );
    };
 
    glfwSetCursorPosCallback( window, static_mouseMovementCallback );

    if (button == GLFW_MOUSE_BUTTON_RIGHT) { // 'arcball' handles right mouse event

      arcballActive = true;
      arcball->mousePress( button, xpos, ypos );

    } else if (button == GLFW_MOUSE_BUTTON_LEFT) {

      scene->buttonDown = button;
      scene->buttonMods = keyModifiers;
      scene->mouse = vec2(xpos,ypos);
      dragging = true;
    }

  } else { 

    // handle mouse release

    if (!draggingWasDone) 
      mouseClick( vec3( xpos, ypos, 0 ), button, keyModifiers );

    glfwSetCursorPosCallback( window, NULL );
    dragging = false;
    draggingWasDone = false;
    arcballActive = false;

    scene->buttonDown = -1;
    scene->buttonMods = 0;
  }
}




// Mouse callbacks

void RTwindow::mouseMovementCallback( GLFWwindow* window, double xpos, double ypos )

{
  if (arcballActive) {
    arcball->mouseDrag( xpos, ypos );
    viewpointChanged = true;
    redisplay = true;
    return;
  }

  if (!dragging)
    return;

  draggingWasDone = true;

  scene->mouse = vec2( xpos, ypos );
}



void RTwindow::mouseScrollCallback( GLFWwindow* window, double xoffset, double yoffset )

{
  arcball->mouseScroll( xoffset, yoffset ); // tell arcball about this
  viewpointChanged = true;
  redisplay = true;
}



void RTwindow::mouseClick( vec3 mouse, int button, int keyModifiers )

{
  if (button == GLFW_MOUSE_BUTTON_LEFT) {

    if (!keyModifiers) {

      scene->storedRays.clear();
      scene->storedRayColours.clear();
      scene->storingRays = true;
      scene->pixelColour( mouse.x, windowHeight-1-mouse.y );
      scene->storingRays = false;

    } else if (keyModifiers & GLFW_MOD_SHIFT) {

      // A right-click on a pixel sets that as the "debugging pixel".
      // When that pixel is raytraced, the 'debug' flag is set.  The
      // 'debug' flag is cleared after that one pixel is raytraced.
      // So other parts of the code can have debugging code based on
      // the status of the 'debug' flag.

      scene->debugPixel = vec2( mouse.x, windowHeight - mouse.y - 1 );
    }

    redisplay = true;
  }
}




void RTwindow::userDraw( mat4 &WCS_to_VCS, mat4 &VCS_to_CCS ) 

{
  viewpointChanged = true;
  scene->renderGL( WCS_to_VCS, VCS_to_CCS );
}



void RTwindow::raytrace( mat4 &MVP ) 

{
  scene->renderRT( viewpointChanged );
  viewpointChanged = false;
}


void RTwindow::readView()

{
  ifstream in( "data/view.txt" );

  if (!in)
    return;

  in >> arcball->V;
  in >> arcball->distToCentre;

  float angle;
  in >> angle;
  fovy = angle/180.0*M_PI;
}


void RTwindow::writeView()

{
  ofstream out( "data/view.txt" );

  out << arcball->V << endl;
  out << arcball->distToCentre << endl;
  out << fovy*180/M_PI << endl;
}
