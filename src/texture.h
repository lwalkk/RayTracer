/* texture.h
 */


#ifndef TEXTURE_H
#define TEXTURE_H

#include "headers.h"
#include <cstring>
#include "seq.h"
#include "linalg.h"

class Texture {

  GLubyte *texmap;		/* texture map */
  int width, height;		/* texmap dimensions */
  bool hasAlpha;		/* true if alpha channel exists */

  void registerWithOpenGL();
  GLubyte *readP6( char *filename );
  //GLubyte *readPNG( char *filename );

  friend class Material;
  friend class WavefrontObj;

 public:

  GLuint textureID;		/* the OpenGL ID for this texture */

  static bool useMipMaps;

  char *name;			/* filename */

  Texture() {}

  Texture( char *filename ) {
    char *p = strrchr( filename, '.' );
    if (p == NULL || strcmp( p, ".ppm" ) == 0)
      texmap = readP6( filename );
#ifdef HAVEPNG
    else
      texmap = readPNG( filename );
#endif
    name = strdup( filename );
    registerWithOpenGL();
  }

  GLuint texID() {
    return textureID;
  }

  void makeActive() {
    glEnable( GL_TEXTURE_2D );
    glBindTexture( GL_TEXTURE_2D, textureID );
    if (hasAlpha) {
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    } else
      glDisable(GL_BLEND);
  }
  
  vec3 texel( float i, float j, float &alpha );

  Texture *findTexture( char *name );

#ifdef HAVEPNG
  unsigned char *readPNG( char *filename );
#endif
};


#endif
