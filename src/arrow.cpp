 /* arrow.cpp
 */


#include "headers.h"

#include <math.h>
#include "arrow.h"


// Render with openGL

#define PI          3.1415926
#define PHI_STEP    PI/16.0

#define HEAD_LENGTH_FACTOR 10	// = multiple of body radius
#define HEAD_RADIUS_FACTOR  2	// = multiple of body radius

#define HEAD_ANGLE  atan2( HEAD_LENGTH_FACTOR, HEAD_RADIUS_FACTOR )


void Arrow::draw( GPUProgram *gpuProg, mat4 &OCS_to_WCS, mat4 &WCS_to_VCS, mat4 &VCS_to_CCS, float length, float radius )

{
  // Count the quads
  //
  // Do it this way to avoid discrepences with roundoff

  int numTriangles = 0;
  for (float phi=0; phi<1.99*PI; phi+=PHI_STEP)
    numTriangles += 5;

  vec3 *attribs = new vec3[2*3*numTriangles]; // 2 attributes x 3 vertices x numTriangles

  vec3 *pos  = &attribs[0];
  vec3 *norm = &attribs[3*numTriangles];

  float bodyLength = length - HEAD_LENGTH_FACTOR * radius;
  float headRadius = HEAD_RADIUS_FACTOR * radius;

  vec3 tip(0,0,length);
  vec3 bodyTip(0,0,bodyLength);
  vec3 tail(0,0,0);
  vec3 nDown(0,0,-1);

  for (float phi=0; phi<1.99*PI; phi+=PHI_STEP) {

    // body normals

    vec3 N0( cos(phi),          sin(phi),          0 );
    vec3 N1( cos(phi+PHI_STEP), sin(phi+PHI_STEP), 0 );

    // body points
      
    vec3 B0t = radius * N0;
    vec3 B1t = radius * N1;
    vec3 B0h = B0t + vec3(0,0,bodyLength);
    vec3 B1h = B1t + vec3(0,0,bodyLength);

    // head points

    vec3 H0 = bodyTip + headRadius * N0;
    vec3 H1 = bodyTip + headRadius * N1;

    vec3 N0h = cos(HEAD_ANGLE) * N0 + sin(HEAD_ANGLE) * vec3(0,0,1);
    vec3 N1h = cos(HEAD_ANGLE) * N1 + sin(HEAD_ANGLE) * vec3(0,0,1);

    // bottom cap

    *(pos++) = tail;
    *(pos++) = B1t;
    *(pos++) = B0t;

    *(norm++) = nDown;
    *(norm++) = nDown;
    *(norm++) = nDown;

    // edge quads

    *(pos++) = B0t;
    *(pos++) = B1t;
    *(pos++) = B1h;

    *(norm++) = N0;
    *(norm++) = N1;
    *(norm++) = N1;

    *(pos++) = B0t;
    *(pos++) = B1h;
    *(pos++) = B0h;

    *(norm++) = N0;
    *(norm++) = N1;
    *(norm++) = N0;

    // underside of arrowhead

    *(pos++) = bodyTip;
    *(pos++) = H1;
    *(pos++) = H0;

    *(norm++) = nDown;
    *(norm++) = nDown;
    *(norm++) = nDown;

    // arrowhead

    *(pos++) = tip;
    *(pos++) = H0;
    *(pos++) = H1;

    *(norm++) = N0h;
    *(norm++) = N0h;
    *(norm++) = N1h;
  }

  // create a VAO

  GLuint VAO;

  glGenVertexArrays( 1, &VAO );
  glBindVertexArray( VAO );

  GLuint VBO;
  glGenBuffers( 1, &VBO );
  glBindBuffer( GL_ARRAY_BUFFER, VBO );
  glBufferData( GL_ARRAY_BUFFER, 2*3*numTriangles*sizeof(vec3), attribs, GL_STATIC_DRAW );

  glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, 0 );
  glEnableVertexAttribArray( 0 );
    
  glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 0, (void*) (sizeof(vec3)*3*numTriangles) );
  glEnableVertexAttribArray( 1 );

  glBindBuffer( GL_ARRAY_BUFFER, 0 );
  glBindVertexArray( 0 );

  // Draw

  mat->setMaterialForOpenGL( gpuProg );

  mat4 MV = WCS_to_VCS * OCS_to_WCS;
  gpuProg->setMat4( "MV", MV );

  mat4 MVP = VCS_to_CCS * MV;
  gpuProg->setMat4( "MVP", MVP );

  glBindVertexArray( VAO );
  glDrawArrays( GL_TRIANGLES, 0, 3*numTriangles );
  glBindVertexArray( 0 );

  // Free things

  glDeleteBuffers( 1, &VBO );
  glDeleteVertexArrays( 1, &VAO );

  delete[] attribs;
}
