// drawSegs.h
//
// Create a class instance:
//
//    Segs *segs = new Segs();
//
// Use it:
//
//    segs->drawOneSeg( tail, head, MVP );


#ifndef DRAW_SEGS_H
#define DRAW_SEGS_H

#include "headers.h"
#include "gpuProgram.h"


class Segs {

  static const char *fragmentShader;
  static const char *vertexShader;

  GPUProgram *setupShaders();

  GPUProgram *gpuProg;
  
 public:

  Segs() { 
    gpuProg = setupShaders();
  };

  // Main function
  
  void drawSegs( GLuint primitiveType, vec3 *pts, vec3 *colours, vec3 *norms, int nPts, mat4 &MV, mat4 &MVP, vec3 lightDir );

  // Variant: no normals
  
  void drawSegs( GLuint primitiveType, vec3 *pts, vec3 *colours, int nPts, mat4 &MV, mat4 &MVP, vec3 lightDir ) {
    drawSegs( primitiveType, pts, colours, NULL, nPts, MV, MVP, lightDir );
  }

  // Variant: single colour
  
  void drawSegs( GLuint primitiveType, vec3 *pts, vec3 colour, vec3 *norms, int nPts, mat4 &MV, mat4 &MVP, vec3 lightDir ) {

    vec3 *colours = new vec3[ nPts ];
    for (int i=0; i<nPts; i++)
      colours[i] = colour;

    drawSegs( primitiveType, pts, colours, norms, nPts, MV, MVP, lightDir );    

    delete[] colours;
  }


  // Variant: single colour, no normals
  
  void drawSegs( GLuint primitiveType, vec3 *pts, vec3 colour, int nPts, mat4 &MV, mat4 &MVP, vec3 lightDir ) {

    vec3 *colours = new vec3[ nPts ];
    for (int i=0; i<nPts; i++)
      colours[i] = colour;

    drawSegs( primitiveType, pts, colours, NULL, nPts, MV, MVP, lightDir );    

    delete[] colours;
  }

  // Variant: one segment only, white.

  void drawOneSeg( vec3 tail, vec3 head, mat4 &MV, mat4 &MVP, vec3 lightDir ) {

    vec3 pts[2]    = { tail, head };
    vec3 colours[2] = { vec3(1,1,1), vec3(1,1,1) }; 

    drawSegs( GL_LINES, pts, colours, NULL, 2, MV, MVP, lightDir );
  }
};

#endif
