/* light.h
 */


#ifndef LIGHT_H
#define LIGHT_H


#include "linalg.h"
#include "sphere.h"


class Light {

 public:

  vec3 position;
  vec3 colour;

  Sphere *sphere;		// Used to show light position in renderGL()

  Light() {
    sphere = NULL;
  }

  void draw( GPUProgram *prog, mat4 &WCS_to_VCS, mat4 &VCS_to_CCS, vec3 lightDir );

  friend ostream& operator << ( ostream& stream, Light const& obj );
  friend istream& operator >> ( istream& stream, Light & obj );
};

#endif
