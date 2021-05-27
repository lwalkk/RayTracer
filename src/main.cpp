/* A basic raytracer
 *
 * You can move the viewpoint with the left mouse button, and
 * zoom with the middle mouse button.  With the mouse in the
 * window, you can press '?' to see the options.
 *
 * Whenever nothing is happening in the window, the program starts
 * raytracing the current scene, and draws that as soon as it's done.
 * You can move the viewpoint again, or press a button, and it'll
 * start raytracing again from the new position.
 */


#include "headers.h"

#include <cstdlib>
#include <ctype.h>
#include <fstream>
#include "rtWindow.h"
#include "scene.h"
#include "material.h"
#include "seq.h"
#include "gpuProgram.h"
#include "font.h"
#include "pixelZoom.h"


// window dimensions

int windowWidth  = 800;
int windowHeight = 600;


Scene      *scene;
RTwindow   *rtWindow;
Axes       *axes;
Segs       *segs;
PixelZoom  *pixelZoom;
GPUProgram *gpuProg;

char *filename[2] = { NULL, NULL }; // from command line


void skipComments( istream &in );
void parseOptions( int argc, char **argv );


// Error callback

void errorCallback( int error, const char* description )

{
  cerr << "Error " << error << ": " << description << endl;
  exit(1);
}


// Callbacks for when window size changes

void windowReshapeCallback( GLFWwindow* window, int width, int height )

{
  windowWidth = width;
  windowHeight = height;
}

void framebufferReshapeCallback( GLFWwindow* window, int width, int height )

{
  glViewport( 0, 0, width, height );
}


// Main program


int main( int argc, char **argv )

{
  if (argc < 2) {
    cerr << "Usage: " << argv[0] << " sceneFile ..." << endl;
    exit(1);
  }

  // Initialize the window

  glfwSetErrorCallback( errorCallback );
  
  GLFWwindow* window;

  if (!glfwInit())
    return 1;
  
#ifdef MACOS
  glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );
  glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 2 );
  glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE );
  glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
#else
  glfwWindowHint( GLFW_CLIENT_API, GLFW_OPENGL_ES_API );
  glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );
  glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 0 );
#endif

  chdir( ".." );

  window = glfwCreateWindow( windowWidth, windowHeight, "Raytracer", NULL, NULL);
  
  if (!window) {
    glfwTerminate();
    return 1;
  }

#if 0
  glfwSetWindowPos(window, 2000, 100);
#endif
  
  glfwMakeContextCurrent( window );
  glfwSwapInterval( 1 );
  gladLoadGLLoader( (GLADloadproc) glfwGetProcAddress );

  glfwSetWindowSizeCallback( window, windowReshapeCallback );
  glfwSetFramebufferSizeCallback( window, framebufferReshapeCallback );

  // Fonts

  initFont( "data/FreeSans.ttf", 20 );
  
  // Set up the scene

  scene = new Scene(); // must exist before parseOptions() is called
  parseOptions( argc, argv );

  rtWindow = new RTwindow( 20, 50, 1200, 800, filename[0], scene, window ); // production

  // rtWindow = new RTwindow( 20, 50, 240, 160, filename[0], scene, window ); // debugging

  scene->setWindow( rtWindow );

  pixelZoom = new PixelZoom();
  
  // Read the scene file

  {
    ifstream in( filename[0] );

    if (!in) {
      cerr << "Error opening " << filename[0] << ".  Check that it exists and that the permissions are set to allow you to read it." << endl;
      exit(1);
    }

    char *basename = strdup(filename[0]);
    char *p = strrchr( basename, '/' );
    if (p != NULL)
      *p = '\0';

    scene->read( basename, in );
  }

  // Output the scene if a second filename is present on the command line

  if (filename[1] != NULL) {
    ofstream out( filename[1] );
    scene->write( out );
  }

  // Main loop

  int prevButtonDown = -1;

  while (!glfwWindowShouldClose( window )) {

    glfwPollEvents();

    mat4 WCS_to_VCS = rtWindow->arcball->V;

    mat4 VCS_to_CCS = perspective( rtWindow->fovy, 
				   windowWidth/(float)windowHeight, 
				   MAX( 0.1, rtWindow->arcball->distToCentre - 100 ),
				   rtWindow->arcball->distToCentre + 100 );

    if (rtWindow->redisplay) {
      rtWindow->viewpointChanged = true;
      scene->renderGL( WCS_to_VCS, VCS_to_CCS );
      glfwSwapBuffers( window );
    }

    scene->renderRT( rtWindow->redisplay ); // parameter == "start raytracing again if window is redisplayed"

    if (scene->stop && (scene->buttonDown == GLFW_MOUSE_BUTTON_LEFT || prevButtonDown != scene->buttonDown))
      scene->draw_RT_and_GL( WCS_to_VCS, VCS_to_CCS );

    prevButtonDown = scene->buttonDown;

    rtWindow->redisplay = false;
  }

  // Clean up

  glfwDestroyWindow( window );
  glfwTerminate();

  return 0;
}



// Parse the command-line options

void parseOptions( int argc, char **argv )

{
  int next_fn = 0;
  
  while (argc > 1) {
    argv++;
    argc--;
    if (argv[0][0] != '-') {

      if (next_fn >= 2)
	cerr << "Only two filenames allowed on command line" << endl;
      else
	filename[ next_fn++ ] = argv[0];

    } else switch( argv[0][1] ) {

    case 'd':			// max depth for ray tracing
      argc--; argv++;
      scene->maxDepth = atoi( *argv );
      break;

    case 't':			// use texture maps?
      scene->useTextureTransparency = !scene->useTextureTransparency;
      break;

    case 'g':			// number of glossy iterations
      argc--; argv++;
      scene->glossyIterations = atoi( *argv );
      break;

    case 'm':			// use mipMaps?
      Texture::useMipMaps = !Texture::useMipMaps;
      break;

    default:
      cerr << "Unrecognized option -" << argv[0][1] << ".  Options are:" << endl;
      cerr << "  -d #   set max depth\n" << endl;
      cerr << "  -t     toggle texture transparency\n" << endl;
      break;
    }
  }

  if (next_fn == 0) {
    cerr << "No input filename provided on command line" << endl;
    abort();
  }
}


// Skip past a comment

int lineNum = 1;

void skipComments( istream &in )

{
  char c;

  // get next non-space

  do {
    in.get(c);
    if (c == '\n')
      lineNum++;
  } while (in && isspace(c));
  in.putback(c);

  // Skip past any comments

  while (c == '#') {
    do
      in.get(c);
    while (in && c != '\n');
    lineNum++;
    do {
      in.get(c);
      if (c == '\n')
	lineNum++;
    } while (in && isspace(c));
    in.putback(c);
  }
}
